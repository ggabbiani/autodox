/*
 * OpenSCAD source file processor
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

#pragma once

#include "writers.h"
#include "antlr4-runtime.h"

#include <filesystem>
#include <iostream>
#include <memory>

namespace scad {

/**
 * Process OpenSCAD sources producing the Document via a scad parser listener
 */
class Processor {
public:
  explicit Processor(doc::Writer *writer) : _writer(writer) {
  }

  void writeToC() {
    _writer->toc(_toc);
  }

  void writeGraph() {
    _writer->graph(_toc);
  }

  /**
   * Elaborate an OpenSCAD source and produce a Markdown document
   *
   * TODO: other kind of formatter (i.e not only Markdown)
   */
  void operator () (
    // OpenSCAD source file relative to source root or absolute
    const std::filesystem::path &source
  );
private:
  std::unique_ptr<doc::Writer> _writer;
  doc::ToC _toc;
};

}
