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
#include "ConfigDocument.h"
#include "Exception.h"
#include "Log.h"
#include "SearchPaths.h"
#include "Robot.h"

#include "miro/ConfigParameters.h"

#include <qfile.h>
#include <qdom.h>
#include <qfileinfo.h>
#include <qdir.h>

#include <sstream>
#include <cmath>
#include <cstdlib>

#include "qt_compatibility.h"

namespace Miro
{

  ConfigDocument::ConfigDocument() :
      document_(NULL)
  {}

  ConfigDocument::ConfigDocument(QDomDocument * _document) :
      document_(_document)
  {}

  ConfigDocument::~ConfigDocument()
  {
    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      delete(*first);
    }
    delete document_;
  }

  void ConfigDocument::init(QDomDocument * _document) throw(Exception)
  {
    delete document_;
    document_ = _document;
  }

  void ConfigDocument::init(std::string const& _fileName,
                            SearchPaths const& _paths)
  throw(Exception)
  {
    delete document_;
    document_ = NULL;

    fullName_ = _paths.findFile(_fileName);
    if (fullName_.length() == 0) {
      MIRO_LOG_OSTR(LL_ERROR,
                    "ConfigDocument: File not found: " <<
                    _fileName << std::endl <<
                    "No config file processing.");
    }
    else {
      MIRO_LOG_OSTR(LL_NOTICE,
                    "ConfigDocument: " << fullName_);
      QFile f(fullName_.c_str());

      if (!f.open(IO_ReadOnly)) {
        throw CException(errno, "Error opening" + fullName_);
      }
      QString parsingError;
      int line = 0;
      int column = 0;
      document_ = new QDomDocument("MiroConfigDocument");
      if (!document_->setContent(&f, &parsingError, &line, &column)) {
        f.close();
        std::stringstream ostr;
        ostr << "error parsing " << fullName_ << std::endl
        << " in line " << line << " "
        << ", column " << column << std::endl
        << qPrintable(parsingError) << std::endl;
        throw Exception(ostr.str());
      }
      f.close();
    }

    collectIncludeDocuments(_paths);
  }

  void
  ConfigDocument::fini()
  {
    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      delete(*first);
    }
    childDocuments_.clear();

    delete document_;
    document_ = NULL;
    section_ = "";
  }

  ConfigDocument::StringVector
  ConfigDocument::getInstances(const std::string& _type)
  {
    StringVector names;

    if (document_ != NULL) {
      QString section = section_.c_str();
      QString type = _type.c_str();

      // get the root nodes first child
      QDomNode n1 = document_->documentElement().firstChild();
      while (!n1.isNull()) {
        QDomElement e1 = n1.toElement();
        if (!e1.isNull() &&
                    ((n1.nodeName() == "section" &&
                      e1.attribute("name") == section) ||
                     (n1.nodeName() == section))) {

          QDomNode n2 = n1.firstChild();
          while (!n2.isNull()) {
            QDomElement e2 = n2.toElement();
            if (!e2.isNull() &&
		n2.nodeName() == "instance" &&
		e2.attribute("type") == type) {
	      QString n = e2.attribute("name");
	      Robot::substitute(n);
              std::string name = qPrintable(n);
              if (name != "")
                names.push_back(name);
            }
            n2 = n2.nextSibling();
          }
        }
        n1 = n1.nextSibling();
      }
    }

    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      StringVector is = (*first)->getInstances(_type);
      names.insert(names.end(), is.begin(), is.end());
    }

    return names;
  }

  void
  ConfigDocument::getParameters(const std::string& _category,
                                ConfigParameters& _parameters)
  {
    if (document_ != NULL) {
      int processed = recGetParameters(_category, _parameters);

      if (processed == 0) {
        MIRO_LOG_OSTR(LL_WARNING,
                      "ConfigDocument: No parameters found for " <<
                      _category << " in section " << section_ <<
                      "\nConfigDocument: Using defaults!");
      }
      else if (processed > 1) {
        MIRO_LOG_OSTR(LL_WARNING,
                      "ConfigDocument: Multiple parameter sections found for " <<
                      _category << " in section " << section_);
      }
    }
  }

  int
  ConfigDocument::recGetParameters(const std::string& _category,
                                   ConfigParameters& _parameters)
  {
    int processed = 0;

    if (document_ != NULL) {
      QString section = section_.c_str();
      QString category = _category.c_str();

      // get the root nodes first child
      QDomNode n1 = document_->documentElement().firstChild();
      while (!n1.isNull()) {
        QDomElement e1 = n1.toElement();
        if (!e1.isNull() &&
                    ((n1.nodeName() == "section" &&
                      e1.attribute("name") == section) ||
                     (n1.nodeName() == section))) {

          QDomNode n2 = n1.firstChild();
          while (!n2.isNull()) {
            QDomElement e2 = n2.toElement();
            if (!e2.isNull() &&
                        ((n2.nodeName() == "parameter" &&
                          e2.attribute("name") == category) ||
                         (n2.nodeName() == category))) {
              _parameters <<= n2;
              ++processed;
            }
            n2 = n2.nextSibling();
          }
        }
        n1 = n1.nextSibling();
      }
    }

    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      processed += (*first)->recGetParameters(_category, _parameters);
    }

    return processed;
  }


  void
  ConfigDocument::getType(const std::string& _type,
                          const std::string& _name,
                          ConfigParameters& _parameters)
  {
    if (document_ != NULL) {
      int processed = recGetType(_type, _name, _parameters);

      if (processed == 0) {
        MIRO_LOG_OSTR(LL_WARNING,
                      "ConfigDocument: No parameters found for " <<
                      _type << ", " <<
                      _name << " in section " << section_ <<
                      "\nConfigDocument: Using defaults!");
      }
      else if (processed > 1) {
        MIRO_LOG_OSTR(LL_WARNING,
                      "ConfigDocument: Multiple parameter sections found for " <<
                      _name << " in section " << section_);
      }
    }
  }

  int
  ConfigDocument::recGetType(const std::string& _type,
                             const std::string& _name,
                             ConfigParameters& _parameters)
  {
    int processed = 0;

    if (document_ != NULL) {
      QString section = section_.c_str();
      QString type = _type.c_str();
      QString name = _name.c_str();

      // get the root nodes first child
      QDomNode n1 = document_->documentElement().firstChild();
      while (!n1.isNull()) {
        QDomElement e1 = n1.toElement();
        if (!e1.isNull() &&
                    ((n1.nodeName() == "section" &&
                      e1.attribute("name") == section) ||
                     (n1.nodeName() == section))) {

          QDomNode n2 = n1.firstChild();
          while (!n2.isNull()) {
            QDomElement e2 = n2.toElement();
            if (!e2.isNull() &&
		n2.nodeName() == "instance" &&
		e2.attribute("type") == type) { 
	      QString n = e2.attribute("name");
	      Robot::substitute(n);
	      if (n == name) {
		_parameters <<= n2;
		++processed;
	      }
            }
            n2 = n2.nextSibling();
          }
        }
        n1 = n1.nextSibling();
      }
    }

    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      processed += (*first)->recGetType(_type, _name, _parameters);
    }

    return processed;
  }

  void
  ConfigDocument::getInstance(const std::string& _name,
                              ConfigParameters& _parameters)
  {
    if (document_ != NULL) {
      int processed = recGetInstance(_name, _parameters);

      if (processed == 0) {
        MIRO_LOG_OSTR(LL_WARNING,
                      "ConfigDocument: No parameters found for " <<
                      _name << " in section " << section_ <<
                      "\nConfigDocument: Using defaults!");
      }
      else if (processed > 1) {
        MIRO_LOG_OSTR(LL_WARNING,
                      "ConfigDocument: Multiple parameter sections found for " <<
                      _name << " in section " << section_);
      }
    }
  }

  int
  ConfigDocument::recGetInstance(const std::string& _name,
                                 ConfigParameters& _parameters)
  {
    int processed = 0;

    if (document_ != NULL) {
      QString section = section_.c_str();
      QString name = _name.c_str();

      // get the root nodes first child
      QDomNode n1 = document_->documentElement().firstChild();
      while (!n1.isNull()) {
        QDomElement e1 = n1.toElement();
        if (!e1.isNull() &&
                    ((n1.nodeName() == "section" &&
                      e1.attribute("name") == section) ||
                     (n1.nodeName() == section))) {

          QDomNode n2 = n1.firstChild();
          while (!n2.isNull()) {
            QDomElement e2 = n2.toElement();
            if (!e2.isNull() &&
		n2.nodeName() == "instance") {
	      QString n = e2.attribute("name");
	      Robot::substitute(n);
	      if (n == name) {
		_parameters <<= n2;
		++processed;
	      }
	    }
            n2 = n2.nextSibling();
          }
        }
        n1 = n1.nextSibling();
      }
    }

    DocumentVector::const_iterator first, last = childDocuments_.end();
    for (first = childDocuments_.begin(); first != last; ++first) {
      processed += (*first)->recGetInstance(_name, _parameters);
    }

    return processed;
  }

  void
  ConfigDocument::collectIncludeDocuments(SearchPaths const& _paths)
  {
    if (document_ != NULL) {
      // get the root nodes first child
      QDomNode n1 = document_->documentElement().firstChild();
      while (!n1.isNull()) {
        QDomElement e1 = n1.toElement();
        if (!e1.isNull() &&
                    n1.nodeName() == "include") {
          if (e1.attribute("file") == "") {
            throw Exception("ConfigDocument: <include> tag without file attribute.");
          }

          QString name = e1.attribute("file");
          ConfigDocument * doc = new ConfigDocument();


	  SearchPaths paths(_paths);

	  // add absolute file path to search path, to allow for "file local" search
	  if (fullName_.length() > 0) {
	    QString fn(fullName_.c_str());
	    QFileInfo fileInfo(fn);
	    QDir dir = fileInfo.absolutePath();
	    paths.addPath(std::string(dir.absolutePath().toLatin1()));
	  }

          doc->init(std::string(name.toLatin1()), paths);
          childDocuments_.push_back(doc);
        }
        n1 = n1.nextSibling();
      }
    }
  }
}
