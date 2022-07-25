/*
 * ADOX scad processor
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "antlr4-runtime.h"
#include "SCADLexer.h"
#include "SCADParser.h"

#include "CLI11.hpp"

#include <iostream>
#include <sstream>

using namespace std;
using namespace scad;
using namespace antlr4;

class ErrorListener : public BaseErrorListener {
  void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
    const std::string &msg, std::exception_ptr e) override;
};

void ErrorListener::syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
  ostringstream s;
  // cout  << "Offending symbol  : " << offendingSymbol->toString() << endl
  //       << "Grammar file name : " << recognizer->getGrammarFileName() << endl;
  s << "Grammar(" << recognizer->getGrammarFileName() << ") Line(" << line << ":" << charPositionInLine << ") Error(" << msg << ")";
  throw std::invalid_argument(s.str());
}

int main(int argc, const char *argv[]) {
  string file;
  string rule;
  CLI::App app{"ADOX tester"};
  app.add_option("file", file, "the OpenSCAD file to be checked")
    ->required()
    ->check(CLI::ExistingFile);
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }

  auto              result = EXIT_SUCCESS;
  ifstream          stream(file);
  ANTLRInputStream  input(stream);
  SCADLexer         lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens())
    cout << token->toString() << endl;

  SCADParser    parser(&tokens);
  parser.removeErrorListeners();
  ErrorListener listener;
  parser.addErrorListener(&listener);

  try {
    tree::ParseTree*  tree = parser.expr();
    cout << tree->toStringTree(&parser) << endl << endl;
  } catch(const exception &error) {
    cerr << error.what() << endl;
    result = EXIT_FAILURE;
  }
  return result;
}
