// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2013 
// Department of Neural Information Processing, University of Ulm
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#ifndef Miro_SearchPaths_h
#define Miro_SearchPaths_h

#include "miroXml_Export.h"

#include <vector>
#include <string>

namespace Miro
{
  class miroXml_Export SearchPaths
  {
  public:
    typedef std::vector<std::string> StringVector;

    SearchPaths(bool currentPath = true);

    void addMiroEtcPaths();

    /** The absolute path to first existing file that matches the name is returned. */
    std::string findFile(std::string const& name) const;

    void addPath(std::string const& path) {
      m_paths.push_back(path);
    }

    StringVector const& paths() const throw() {
      return m_paths;
    }

  protected:
    StringVector m_paths;
    bool m_currentPath;
  };
}
#endif // Miro_SearchPaths_h
