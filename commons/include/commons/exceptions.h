#pragma once
/*
 * Common used exceptions.
 *
 * Copyright © 2023 Giampiero Gabbiani (giampiero@gabbiani.org)
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
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <filesystem>
#include <stdexcept>

class FileNotFound : public std::runtime_error {
public:
  explicit FileNotFound(const std::filesystem::path &fname);
};

class OsError : public std::runtime_error {
public:
  explicit OsError(const std::string &message);
};
