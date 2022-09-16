#pragma once
/*
 * language extension declarations
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * ODOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ODOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ODOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include "orthodocs/extensions.h"

namespace scad {

class Extension : public language::Extension {
public:
  static constexpr const char *ID = "scad";
  Extension() : language::Extension(ID) {}
  orthodocs::Document *parse(const std::filesystem::path &source) const override;
private:
  static ExtensionLoader<Extension> loader;
};

} // namespace scad
