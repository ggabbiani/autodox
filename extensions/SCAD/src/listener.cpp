/*
 * OpenSCAD listener
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
// private
#include "listener.h"
// public
#include <SCADLexer.h>
// components
#include <commons/error_info.h>
#include <commons/utils.h>
// system
#include <regex>

using namespace std;

namespace fs=std::filesystem;

namespace {

const antlr4::Token *prevComment(antlr4::BufferedTokenStream *stream, const antlr4::ParserRuleContext *ctx) {
  auto firstToken   = ctx->getStart();
  auto i            = firstToken->getTokenIndex();
  auto leftComments = stream->getHiddenTokensToLeft(i,scad::SCADLexer::COMMENTS);
  const auto *token = leftComments.empty() ? nullptr : leftComments.back();
  return token && token->getTokenIndex()>0 ? token : nullptr;
}

antlr4::Token *firstLeftComment(antlr4::BufferedTokenStream *stream, const antlr4::ParserRuleContext *ctx) {
  auto firstToken   = ctx->getStart();
  auto i            = firstToken->getTokenIndex();
  auto leftComments = stream->getHiddenTokensToLeft(i,scad::SCADLexer::COMMENTS);
  return leftComments.empty() ? nullptr : leftComments.front();
}

const antlr4::Token *nextComment(antlr4::BufferedTokenStream *stream, const antlr4::ParserRuleContext *ctx) {
  auto lastToken      = ctx->getStop();
  auto i              = lastToken->getTokenIndex();
 /*
  * Collect all hidden tokens (any off-default channel) to the right of
  * the current token up until we see a token on DEFAULT_TOKEN_CHANNEL
  * or EOF.
  */
  auto rightComments  = stream->getHiddenTokensToRight(i,scad::SCADLexer::COMMENTS);
  return rightComments.empty() ? nullptr : rightComments.front();
}

}

namespace scad {

Listener::Listener(const fs::path &pkg_source,antlr4::BufferedTokenStream *s)
: _tokens(s), _pkg_path(pkg_source), _document(make_unique<::Document>(pkg_source)) {
  // TODO: make an utility with the code below
  // auto world = _tokens->get(0,_tokens->size());
  // for(const auto *token: world) {
  //   spdlog::debug("{0}[{1}]: {2}",token->getChannel(),token->getTokenIndex(),token->getText());
  // }
}

void Listener::enterPkg(Parser::PkgContext *ctx) {
  curr_package = new doc::Package(_pkg_path);
  curr_item.push(::doc::Item::Owner(curr_package));

  // annotate if any comment at source' beginning
  if (auto comment  = firstLeftComment(_tokens, ctx))
    annotate(curr_package, comment);
}

void Listener::exitPkg(Parser::PkgContext *ctx) {
  auto &pkg  = curr_item.top();
#ifndef NDEBUG
  auto key  = pkg->documentKey();
#endif // NDEBUG

  if (auto [i,success] = _document->index.emplace(move(pkg)); !success)
    throw std::domain_error(ERR_INFO+"Duplicate key «"+(*i)->documentKey()+"» in same document");
  curr_item.pop();
  curr_package  = nullptr;
}

void Listener::enterIncl(Parser::InclContext *ctx) {
  try {
    // change into directory of the current package
    cwd pwd(_pkg_path.parent_path());

    string      inc_file = ctx->FILE()->getText();
    fs::path    inc_path = inc_file.substr(1,inc_file.length()-2); // eliminates angular brackets
    error_code  error;  // we manage fs error: no need for exception here...
    auto        inc_canonical = fs::canonical(inc_path,error);

    if (!static_cast<bool>(error) && is_sub_of(inc_canonical,Option::sroot())) {
      auto requisite = fs::relative(inc_canonical,Option::sroot());
      curr_package->includes.emplace((requisite.parent_path()/requisite.stem()).string());
    }
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(ctx)));
  }
}

void Listener::enterUse(SCADParser::UseContext *ctx) {
  // change into directory of the current package
  cwd pwd(_pkg_path.parent_path());

  string      use_file = ctx->FILE()->getText();
  fs::path    use_path = use_file.substr(1,use_file.length()-2); // eliminates angular brackets
  error_code  error;  // we manage fs error: no need for exception here...
  auto        use_canonical = fs::canonical(use_path,error);

  if (!static_cast<bool>(error) && is_sub_of(use_canonical,Option::sroot())) {
    auto requisite = fs::relative(use_canonical,Option::sroot());
    curr_package->uses.emplace((requisite.parent_path()/requisite.stem()).string());
  }
}

void Listener::enterFunction_def(Parser::Function_defContext *ctx) {
  auto identifier = ctx->ID()->getText();
  bool nested     = dynamic_cast<doc::Module*>(curr_item.top().get())!=nullptr;
  auto item       = new doc::Function(identifier,nested);
  item->parent    = curr_package;
  assert(curr_package);
  curr_item.emplace(item);
}

void Listener::exitFunction_def(Parser::Function_defContext *ctx)  {
  auto &func = curr_item.top();
#ifndef NDEBUG
  auto key  = func->documentKey();
#endif // NDEBUG

  if (!func->nested && !func->privateId()) {
    // annotate if any comment just before function definition
    if (auto comment  = prevComment(_tokens, ctx))
      annotate(func.get(), comment);
    if (auto [i,success] = _document->index.emplace(move(func)); !success)
      throw std::domain_error(ERR_INFO+"Duplicate key «"+(*i)->documentKey()+"» in same document");
  }
  curr_item.pop();
}

void Listener::enterModule_def(Parser::Module_defContext * ctx) {
  auto identifier = ctx->ID()->getText();
  bool nested     = dynamic_cast<doc::Module*>(curr_item.top().get());
  auto item       = make_unique<doc::Module>(identifier,nested);
  assert(curr_package);
  item->parent    = curr_package;
  curr_item.emplace(item.release());
}

void Listener::exitModule_def(Parser::Module_defContext * ctx) {
  auto &mod = curr_item.top();
#ifndef NDEBUG
  auto  key = mod->documentKey();
#endif // NDEBUG

  if (!mod->nested && !mod->privateId()) {
    // annotate if any comment just before function definition
    if (auto comment  = prevComment(_tokens, ctx); comment)
      annotate(mod.get(), comment);
    if (auto [i,success] = _document->index.emplace(move(mod)); !success)
      throw std::domain_error(ERR_INFO+"Key «"+(*i)->documentKey()+"» already present in document");
  }
  curr_item.pop();
}


void Listener::enterParameter(Parser::ParameterContext *ctx) {
  assert(curr_parameter.get()==nullptr);
  curr_parameter  = make_unique<::doc::Parameter>();
}

void Listener::exitParameter(Parser::ParameterContext *ctx) {
#ifndef NDEBUG
  auto PACKAGE  = curr_package->name;
  auto ITEM     = curr_item.top()->name;
  auto TYPE     = curr_item.top()->type;
  auto PARAM    = curr_parameter->name;
  auto LEFT     = prevComment(_tokens, ctx);
  auto RIGHT    = nextComment(_tokens, ctx);
#endif // NDEBUG
  if (!curr_item.empty()) {
    if (auto comment = Option::orthodox() ? prevComment(_tokens, ctx) : nextComment(_tokens, ctx))
      annotate(curr_parameter.get(),comment);
    curr_item.top()->parameters.push_back(move(curr_parameter));
  }
}

void Listener::enterLookup(Parser::LookupContext *ctx) {
  auto value = ctx->ID()->getText();
  if (curr_parameter) {
    if (dynamic_cast<Parser::ParameterContext*>(ctx->parent))
      curr_parameter->name = value;
  }
}

void Listener::enterAssignment(Parser::AssignmentContext *ctx) {
  auto id       = ctx->ID()->getText();
  auto defaults = ctx->expr()->getText();
  if (dynamic_cast<Parser::StatContext*>(ctx->parent)) {
    auto nested   = dynamic_cast<doc::Module*>(curr_item.top().get())!=nullptr;
    auto variable = new doc::Variable(id,defaults,nested);
    assert(curr_package);
    variable->parent        = curr_package;
    curr_variable.push(::doc::Item::Owner(variable));
  } else if (curr_parameter) {
    curr_parameter->name      = id;
    curr_parameter->defaults  = defaults;
  }
}

void Listener::exitAssignment(Parser::AssignmentContext *ctx) {
  if (dynamic_cast<Parser::StatContext*>(ctx->parent) && curr_variable.size()) {
    if (auto &var = curr_variable.top(); !var->nested && !var->privateId()) {
      if (auto comment = prevComment(_tokens, ctx))
        annotate(curr_variable.top().get(),comment);
      if (auto [i,success] = _document->index.emplace(move(var)); !success)
        throw std::domain_error(ERR_INFO+"Key «"+(*i)->documentKey()+"» already present in document");
    }
    curr_variable.pop();
  }
}

}