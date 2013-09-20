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
#ifndef SimpleParameterEdit_h
#define SimpleParameterEdit_h

#include "ParameterEdit.h"
#include "SimpleParameter.h"

#include <string>
#include <vector>

#include "miroWidgets_Export.h"

// forward declarations
class QString;
class QLineEdit;
class QTextEdit;
class QComboBox;
class Q3ListBox;
class ConfigFile;


class miroWidgets_Export SimpleParameterEdit : public ParameterEdit
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef ParameterEdit Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  SimpleParameterEdit(SimpleParameter::Type _type,
		      Miro::CFG::Parameter const& _parameter,
		      QDomNode& _parentNode, QDomNode& _node,
		      ItemXML * _parentItem, ItemXML * _item,
		      QWidget * _parent, const char * _name);

  //! Inherited interface.
  virtual void setXML();
  //! Inherited interface.
  virtual bool modified() const;

protected slots:
  void typeBoxModified();
  void listBoxModified();

protected:

  std::vector<std::string> fullDef2StringVector(std::string const& _string);
  std::vector<std::string> fullDef2StringVector(QString const& _string);
  std::vector<std::string> tokenizer(std::string _values);

  //----------------------------------------------------------------------------
  // puprotected members
  //----------------------------------------------------------------------------

  ConfigFile * config_;

  SimpleParameter::Type type_;

  QLineEdit * lineEdit_;
  QTextEdit * textEdit_;
  QComboBox * typeBox_;
  Q3ListBox * listBox_;
  bool typeBoxModified_;
  bool listBoxModified_;
  QString textEditText_;
};

#endif // SimpleParameterEdit_h
