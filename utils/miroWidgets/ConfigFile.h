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
#ifndef ConfigFile_h
#define ConfigFile_h

#include <miro/Singleton.h>

#include <qstring.h>
#include <qstringlist.h>

#include "miroWidgets_Export.h"

// forward declarations
namespace Miro {
  namespace CFG {
    class Generator;
    class Parser;
  }
}

class miroWidgets_Export ConfigFileName
{
public:
  ConfigFileName();

  QString const & fileName() const;
  void setFileName(QString const& _name);

  static Miro::Singleton<ConfigFileName> instance;

protected:
  QString fileName_;

private:
  //  ~ConfigFileName();
};

inline
QString const&
ConfigFileName::fileName() const {
  return fileName_;
}

inline
void
ConfigFileName::setFileName(QString const& _name) {
  fileName_ = _name;
}


class miroWidgets_Export ConfigFile
{
public:
  //! Initializing constructor.
  ConfigFile();
  //! Destructor
  ~ConfigFile();
  
  void setDescriptionFiles(const QStringList& _files);
  const QStringList& getDescriptionFiles() const;
  
  static Miro::Singleton<ConfigFile> instance;
  
  const Miro::CFG::Generator& description() const;

protected:
  void readConfigFile();
  void writeConfigFile();
  void parseDescriptionFiles();

  //! fully qualified name of the config file
  QString configFile_;
  //! list of description files 
  QStringList descriptionFiles_;

  Miro::CFG::Generator * generator_;
  Miro::CFG::Parser * handler_;

private:
};

inline
const QStringList&
ConfigFile::getDescriptionFiles() const {
  return descriptionFiles_;
}

inline
const Miro::CFG::Generator&
ConfigFile::description() const {
  return *generator_;
}

typedef ACE_Singleton<ConfigFile, ACE_SYNCH_RECURSIVE_MUTEX> ConfigFileSingleton;
MIROWIDGETS_SINGLETON_DECLARE(ACE_Singleton, ConfigFile, ACE_SYNCH_RECURSIVE_MUTEX);

#endif
