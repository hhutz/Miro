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

#ifndef SimpleParameter_h
#define SimpleParameter_h

// This application
#include "ParameterXML.h"
#include "params/Parameter.h"
// The Qt library
#include <qstring.h>

#include "miroWidgets_Export.h"

// Forward declarations
class ConfigFile;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * A SimpleParameter is either a primitive C++ type or its typedef, an enum,
 * an std::string or a user-defined class.
 */
class miroWidgets_Export SimpleParameter : public ParameterXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  /**
   * Base class type.
   * The immediate base class.
   */
  typedef ParameterXML Super;

public:

  //----------------------------------------------------------------------------
  // public types
  //----------------------------------------------------------------------------

  /** Designates the SimpleType which can be directly entered. */
  enum Type {
    NONE,
    BOOL, CHAR, SHORT, USHORT, INT, UINT, LONG, ULONG,
    FLOAT, DOUBLE,
    ANGLE, MIRO_ANGLE, STRING, TEXT,
    ACE_TIME_VALUE, ACE_INET_ADDR,
    ENUMERATION, ENUMERATIONMULTIPLE
  };

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidgetItem.
   * If the QTreeWidget has three or more columns, write the parameter type in
   * column 2.
   * @param[in] _param ?
   * @param[in] _node reference to the node in the DOM tree
   * @param[in] _parentItem pointer to the QTreeWidgetItem for the parent of
   * this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  SimpleParameter(Miro::CFG::Parameter const& _param,
		  QDomNode const& _node,
		  QTreeWidgetItem * _parentItem,
		  QTreeWidgetItem * _pre,
		  QObject * _parent,
		  const char * _name);

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidget.
   * If the QTreeWidget has three or more columns, write the parameter type in
   * column 2.
   * @param[in] _param ?
   * @param[in] _node reference to the node in the DOM tree
   * @param[in] _list pointer to the QTreeWidget for the parent of this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  SimpleParameter(Miro::CFG::Parameter const& _param,
		  QDomNode const& _node,
		  QTreeWidget * _list,
		  QTreeWidgetItem * _pre,
		  QObject * _parent,
		  const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  /**
   * Inherited method.
   * Show a dialog to get the value for the SimpleParameter.
   * If successful, copy the changes to the DOM tree and notify that the
   * SimpleParameter has been modified.
   * Overrides ParameterXML::setParameters().
   */
  virtual void setParameters();

  //----------------------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------------------

  /**
   * Map the print name of the SimpleParameter's type onto its corresponding
   * Type enum value.
   * @param[in] _name the print name of the SimpleParameter's type
   */
  static Type typeFromName(QString const& _name);

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  /** The name of the SimpleParameter's attribute for its value. */
  static QString const XML_ATTRIBUTE_VALUE;

protected:

  /** The description of the SimpleParameter, as passed in upon construction. */
  Miro::CFG::Parameter param_;
};

#endif
