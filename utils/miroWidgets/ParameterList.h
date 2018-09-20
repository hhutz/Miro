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

#ifndef ParameterList_h
#define ParameterList_h

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
 * Class representing vector and set items.
 * A parameter can be an std::vector of a parameter type or and std::set of
 * a parameter type. Unlike a CompoundParameter, all elements must be of the
 * same type and the number of elements is not fixed at compile time.
 */
class miroWidgets_Export ParameterList : public ParameterXML
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

  /**
   * Type of the parameter list.
   */
  enum Type {
    /** Indicates that the type is not yet set. */
    NONE,
    /** Indicates an std::vector of parameters. */
    VECTOR,
    /** Indicates an std::set of parameters. */
    SET};

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidgetItem.
   * If the QTreeWidget has exactly two columns, write the parameter type in
   * column 2.
   * Extract and save the name of the element type.
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
  ParameterList(Miro::CFG::Parameter const& _param,
		QDomNode const& _node,
		QTreeWidgetItem * _parentItem,
		QTreeWidgetItem * _pre,
		QObject * _parent,
		QString const& _name);

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidget.
   * If the QTreeWidget has exactly two columns, write the parameter type in
   * column 2.
   * Extract and save the name of the element type.
   * @param[in] _param ?
   * @param[in] _node reference to the node in the DOM tree
   * @param[in] _list pointer to the QTreeWidget for the parent of this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  ParameterList(Miro::CFG::Parameter const& _param,
		QDomNode const& _node,
		QTreeWidget * _list,
		QTreeWidgetItem * _pre,
		QObject * _parent,
		QString const& _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  /**
   * Inherited method.
   * Overrides ParameterXML::init().
   * Create the QWidgetItem subtree for the ParameterList using the
   * specification in the DOM tree.
   */
  virtual void init();

  /**
   * Inherited method.
   * Show a dialog to get the value for each element of the ParameterList.
   * Overrides ParameterXML::setParameters().
   */
  virtual void setParameters();

  //----------------------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------------------

  /**
   * Convert type string into a Type.
   * @param[in] _type the print name of the type, as extracted from the
   * C++ specification of the std::vector or std::set concrete type
   * @return the Type enum value corresponding to the print name of the type
   */
  static Type typeFromName(QString const& _type);

protected:

  /** Description of the parameter, as passed in upon construction. */
  Miro::CFG::Parameter param_;

  /**
   * Type description of the parameter.
   * A Type enum value corresonding to the type's print name.
   */
  Type type_;

  /**
   * std::vector<int> -> int
   * The print name of the type, as extracted from the C++ specification of the
   * std::vector or std::set concrete type
   */
  QString nestedTypeName_;
};

#endif
