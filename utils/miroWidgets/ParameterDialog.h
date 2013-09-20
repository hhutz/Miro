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
#ifndef ParameterDialog_h
#define ParameterDialog_h

#include "DialogXML.h"
#include "params/Type.h"

#include <vector>

#include "miroWidgets_Export.h"

// forward declarations
class ParameterEdit;
class ConfigFile;

//! Parameter edit dialogs of the configuration framework.
/** 
 * As every parameter type in the framework is a compound parameter,
 * this dialog is actually a compound parameter dialog.
 */
class miroWidgets_Export ParameterDialog : public DialogXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef DialogXML Super;
  //! A vector of edit widgets.
  typedef std::vector<ParameterEdit *> EditVector;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  ParameterDialog(Miro::CFG::Type const& _parameterType,
		  QDomNode const& _parentNode, QDomNode const& _node,
		  ItemXML * _parentItem, ItemXML * _item,
		  QWidget * _parent, const char * _name);

  //! Update the xml tree according to the dialog entries.
  virtual void setXML();

protected:
  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  //! Initialize the dialog.
  void initDialog();

  //----------------------------------------------------------------------------
  // puprotected members
  //----------------------------------------------------------------------------

  //! Singleton instance of the config file.
  ConfigFile * config_;

  //! Type description of the underling parameter.
  Miro::CFG::Type parameterType_;
  //! Parameter type elements.
  Miro::CFG::ParameterVector params_;
  
private:
  //! Vector holding all the edit fields of the nested types.
  EditVector editFelds_;
};

#endif
