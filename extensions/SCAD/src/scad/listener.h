#pragma once
/*
 * OpenSCAD listener
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include "document.h"
#include "doc/annotations.h"

#include <globals.h>
#include <SCADParserBaseListener.h>

#include <memory>

namespace scad {

/**
 * This listener is triggered during the parse-tree walk. The resulting synthetic
 * document ownership is then passed to the calling environment through the
 * documentRelease() call.
 */
class Listener : public scad::SCADParserBaseListener, ::doc::Annotation::Setter {
public:
  using Item              = ::doc::Item;
  using Package           = doc::Package;
  using Parameter         = ::doc::Parameter;
  using Parser            = scad::SCADParser;
  using ParserRuleContext = antlr4::ParserRuleContext;
  using Token             = antlr4::Token;

  Listener(const std::filesystem::path &pkg_source,antlr4::BufferedTokenStream *s);

  // return the Document's ownership to the calling environment
  Document::Owner releaseDocument() {return std::move(_document);};

  void enterAssignment(Parser::AssignmentContext *ctx)      override;
  void enterFunction_def(Parser::Function_defContext *ctx)  override;
  void enterLookup(Parser::LookupContext *ctx)              override;
  void enterModule_def(Parser::Module_defContext * ctx)     override;
  void enterParameter(Parser::ParameterContext *ctx)        override;
  void enterPkg(Parser::PkgContext *ctx)                    override;
  void enterIncl(Parser::InclContext *ctx)              override;
  void enterUse(Parser::UseContext *ctx)                override;
  void exitAssignment(Parser::AssignmentContext *ctx)       override;
  void exitFunction_def(Parser::Function_defContext *ctx)   override;
  void exitModule_def(Parser::Module_defContext * ctx)      override;
  void exitParameter(Parser::ParameterContext *ctx)         override;
  void exitPkg(Parser::PkgContext *ctx)                     override;

private:
  template <class Annotable>
  void annotate(Annotable *item, const Token *comment) const {
    static doc::style::Factory factory;
    auto anno = comment->getText();
    if (auto style  = factory(anno); style) {
      auto value  = style->manage(anno);
      if (Option::admonitions())
        mk_admonitions(value);
      set(item->annotation, value);
    }
  }

  std::vector<Token *> leftComments(const ParserRuleContext *ctx) const;
  const Token *backLeftComment(const ParserRuleContext *ctx) const;
  const Token *frontLeftComment(const ParserRuleContext *ctx) const;


  antlr4::BufferedTokenStream *_tokens;
  // the following members represents the actual status variables for the parser
  Package          *curr_package;
  Item::OwnerStack  curr_item;
  Item::OwnerStack  curr_variable;
  Parameter::Owner  curr_parameter;
  /*
   * this is a kludge: is there an official way for retrieving the current
   * language source file from antlr4?
   */
  const std::filesystem::path &_pkg_path;
  Document::Owner   _document;
};

}
