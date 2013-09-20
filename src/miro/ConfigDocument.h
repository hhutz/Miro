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
#ifndef Miro_ConfigDocument_h
#define Miro_ConfigDocument_h

#include "Exception.h"
#include "SearchPaths.h"

#include "miroXml_Export.h"

#include <string>
#include <vector>

// forward declarations
class QDomDocument;

namespace Miro
{
  // forward declarations
  class ConfigParameters;

  class miroXml_Export ConfigDocument
  {
  public:
    typedef std::vector<std::string> StringVector;

    //! Default constructor.
    ConfigDocument();
    //! Initializing constructor. ConfigParameters takes ownership of pointer.
    ConfigDocument(QDomDocument * _document);
    ~ConfigDocument();

    void init(std::string const& _defaultName = std::string(""),
              SearchPaths const& _paths = SearchPaths())
    throw(Exception);
    void init(QDomDocument * _document) throw(Exception);

    void fini();

    void setSection(std::string const& _section);

    StringVector getInstances(std::string const& _type);

    void getInstance(std::string const& _name,
                     ConfigParameters& parameters);

    void getType(std::string const& _type,
                 std::string const& _name,
                 ConfigParameters& parameters);

    void getParameters(std::string const& _name,
                       ConfigParameters& parameters);


  protected:
    typedef std::vector<ConfigDocument *> DocumentVector;

    int recGetParameters(std::string const& _name,
                         ConfigParameters& parameters);
    int recGetType(std::string const& _type,
                   std::string const& _name,
                   ConfigParameters& parameters);
    int recGetInstance(std::string const& _name,
                       ConfigParameters& parameters);

    void collectIncludeDocuments(SearchPaths const& paths);

    QDomDocument * document_;
    std::vector<ConfigDocument *> childDocuments_;

    std::string section_;
    std::string fullName_;
  };

  inline
  void
  ConfigDocument::setSection(const std::string& _section)
  {
    section_ = _section;
    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      (*first)->setSection(_section);
    }
  }
}
#endif // Miro_ConfigDocument_h
