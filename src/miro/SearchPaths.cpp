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
#include "SearchPaths.h"

#include "MiroConfig.h"
#include "Log.h"

#include <ace/OS_NS_stdlib.h>

#include <QFileInfo>

#include <cstdlib>
#include <ace/OS_NS_stdlib.h>


namespace Miro
{
  using namespace std;

  SearchPaths::SearchPaths(bool currentPath) :
    m_currentPath(currentPath)
  {
  }

  void
  SearchPaths::addMiroEtcPaths()
  {
    char* miroRoot = ACE_OS::getenv("MIRO_ROOT");
    if (miroRoot) {
      m_paths.push_back(std::string(miroRoot) + std::string("/etc"));
    }

    m_paths.push_back(string(MIRO_INSTALL_PREFIX "/etc"));
  }


  /** The first existing file that matches the name is returned. */
  std::string
  SearchPaths::findFile(std::string const& name) const
  {
    string fullName;
    StringVector::const_reverse_iterator first, last = m_paths.rend();

    {
      QFileInfo f(name.c_str());
      if (m_currentPath && f.exists())
	return f.absoluteFilePath().toAscii().data();
    }

    for (first = m_paths.rbegin(); first != last; ++first) {
      fullName = *first + "/" + name;
      QFileInfo f(fullName.c_str());
      if (f.exists())
        return f.absoluteFilePath().toAscii().data();
    }

    for (first = m_paths.rbegin(); first != last; ++first) {
      MIRO_LOG_OSTR(LL_ERROR, "File not found: " <<  *first << "/" << name);
    }
    return string();
  }
}
