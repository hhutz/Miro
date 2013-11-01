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
  Singleton<SearchPaths::StringList> SearchPaths::s_etcPaths;

  using namespace std;

  /**
   * @param useCurrentPath use current path when searching for files
   * @param useMiroEtcPaths use list of statically defined etc paths when searching for files
   * @param etcPath path to be added to etc path list
   */
  SearchPaths::SearchPaths(bool useCurrentPath, bool useMiroEtcPaths, const std::string& etcPath) :
    m_useCurrentPath(useCurrentPath),
    m_useMiroEtcPaths(useMiroEtcPaths)
  {
    initMiroEtcPaths(etcPath);
  }

  /**
   * init with default search path and set useCurrentPath to true
   */
  SearchPaths::SearchPaths(const std::string& etcPath) :
    m_useCurrentPath(true),
    m_useMiroEtcPaths(true)
  {
    initMiroEtcPaths(etcPath);
  }

  /**
   * if default paths is empty, initialize with
   *  - ${MIRO_ROOT}/etc (if MIRO_ROOT environment variable is defined)
   *  - MIRO_INSTALL_PREFIX/etc
   * then prepend etcPath to the default paths
   */
  void SearchPaths::initMiroEtcPaths(const std::string& etcPath)
  {
    if(s_etcPaths()->size() == 0) {
      char* miroRoot = ACE_OS::getenv("MIRO_ROOT");
      if (miroRoot) {
        s_etcPaths()->push_back(std::string(miroRoot) + std::string("/etc"));
      }
      s_etcPaths()->push_back(string(MIRO_INSTALL_PREFIX "/etc"));
    }
    prependMiroEtcPath(etcPath);
  }

  bool listContains(SearchPaths::StringList& strList, const std::string& str)
  {
    for(SearchPaths::StringList::iterator it = strList.begin(); it != strList.end(); ++it) {
      if(str == *it)
        return true;
    }
    return false;
  }

  int listRemove(SearchPaths::StringList& strList, const std::string& str)
  {
    int retVal = 0;
    for(SearchPaths::StringList::iterator it = strList.begin(); it != strList.end(); ) {
      if(str == *it) {
        it = strList.erase(it);
        retVal++;
      }
      else {
        it++;
      }
    }
    return retVal;
  }

  /**
   * Prepend a path to the list of default search paths.
   * @param etcPath path to prepended
   * @param replace by default, the path list will not be modified if
   *                etcPath is already in the list. If replace is true, etcPath
   *                will be moved to the top of the list
   *
   * @see initEtcPaths
   */
  void
  SearchPaths::prependMiroEtcPath(const std::string& etcPath, bool replace)
  {
    QFileInfo fi(etcPath.c_str());
    string canonicalPath = fi.canonicalFilePath().toStdString();
    if(!canonicalPath.empty()) {
      if(listContains(*s_etcPaths(), canonicalPath)) {
        if(replace) {
          listRemove(*s_etcPaths(), canonicalPath);
          s_etcPaths()->push_front(canonicalPath);
        }
      }
      else {
        s_etcPaths()->push_front(canonicalPath);
      }
    }
  }

  /** @return number of paths removed from list */
  int SearchPaths::removePath(std::string const& path)
  {
    return listRemove(m_paths, path);
  }

  /**
   * The first existing file that matches the name is returned.
   * Search order is:
   *  - currentPath (if m_useCurrentPath is true)
   *  - path list
   *  - default etc path list (if m_useMiroEtcPaths is true)
   */
  std::string
  SearchPaths::findFile(std::string const& name) const
  {
    string fullName;

    if (m_useCurrentPath) {
      QFileInfo f(name.c_str());
      if (f.exists())
        return f.absoluteFilePath().toAscii().data();
    }

    for(StringList::const_iterator it = m_paths.begin(); it != m_paths.end(); ++it) {
      fullName = *it + "/" + name;
      QFileInfo f(fullName.c_str());
      if (f.exists())
        return f.absoluteFilePath().toAscii().data();
    }

    if (m_useMiroEtcPaths) {
      for(StringList::const_iterator it = s_etcPaths()->begin(); it != s_etcPaths()->end(); ++it) {
        fullName = *it + "/" + name;
        QFileInfo f(fullName.c_str());
        if (f.exists())
          return f.absoluteFilePath().toAscii().data();
      }
    }

    StringVector paths = this->paths();
    for (StringVector::iterator it = paths.begin(); it != paths.end(); ++it) {
      MIRO_LOG_OSTR(LL_ERROR, "File not found: " <<  *it << "/" << name);
    }
    return string();
  }

  SearchPaths::StringVector SearchPaths::paths() const throw()
  {
    int sz = m_paths.size() + (m_useMiroEtcPaths ? s_etcPaths()->size() : 0);
    StringVector retVal;
    retVal.reserve(sz);

    for(StringList::const_iterator it = m_paths.begin(); it != m_paths.end(); ++it) {
      retVal.push_back(*it);
    }
    if(m_useMiroEtcPaths) {
      for(StringList::const_iterator it = s_etcPaths()->begin(); it != s_etcPaths()->end(); ++it) {
        retVal.push_back(*it);
      }
    }
    return retVal;
  }

}
