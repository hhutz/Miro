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

#include "Singleton.h"

#include "miroXml_Export.h"

#include <list>
#include <string>
#include <vector>

namespace Miro
{
  class miroXml_Export SearchPaths
  {
  public:
    typedef std::vector<std::string> StringVector;
    typedef std::list<std::string> StringList;

    SearchPaths(bool useCurrentPath = true, bool useMiroEtcPaths = true, const std::string& etcPath = "");
    SearchPaths(const std::string& etcPath);

    static void prependMiroEtcPath(const std::string& etcPath, bool replace = false);
    /** @deprecated use setUseMiroEtcPaths(true) */
    void addMiroEtcPaths() {
      m_useMiroEtcPaths = true;
    }
    
    /** The absolute path to first existing file that matches the name is returned. */
    std::string findFile(std::string const& name) const;

    void prependPath(std::string const& path) {
      m_paths.push_front(path);
    }
    void appendPath(std::string const& path) {
      m_paths.push_back(path);
    }
    int removePath(std::string const& path);
    /** @deprecated use appendPath */
    void addPath(std::string const& path) {
      appendPath(path);
    }

    StringVector paths() const throw();

    void setUseMiroEtcPaths(bool state) {
      m_useMiroEtcPaths = state;
    }
    bool useMiroEtcPaths() const {
      return m_useMiroEtcPaths;
    }

    void setUseCurrentPath(bool state) {
      m_useCurrentPath = state;
    }
    bool useCurrentPath() const {
      return m_useCurrentPath;
    }

  protected:
    StringList   m_paths;
    bool         m_useCurrentPath;
    bool         m_useMiroEtcPaths;
    
    void initMiroEtcPaths(const std::string& etcPath);
    
    static Singleton<StringList> s_etcPaths;
  };
}

MIROXML_SINGLETON_DECLARE(ACE_Singleton, Miro::SearchPaths::StringList, ACE_SYNCH_RECURSIVE_MUTEX);

#endif // Miro_SearchPaths_h
