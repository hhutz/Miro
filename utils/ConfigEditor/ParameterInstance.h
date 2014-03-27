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

#ifndef ParameterInstance_h
#define ParameterInstance_h

// This application
#include "miroWidgets/CompoundParameter.h"

// Forward declarations
class ConfigFile;
class QTreeWidgetItem;

/**
 * The ItemXML that represents a named instance of a parameter.
 * A ParameterInstance is a CompoundParameter.
 */
class ParameterInstance : public CompoundParameter
{
  Q_OBJECT
  
  /** The immediate base class. */
  typedef CompoundParameter Super;

public:

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
  
 /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidgetItem.
   * If the QTreeWidget has at least 3 columns, write the type name in column 2.
   * @param[in] _treeWidgetItem pointer to the QTreeWidgetItem for this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _node Reference to the node in the xml document
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  ParameterInstance(QTreeWidgetItem * _treeWidgetItem,
		    QTreeWidgetItem * _pre,
		    QDomNode const& _node,
		    QObject * _parent,
		    const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  /**
   * Inherited method.
   * Move item up in the QTreeWidget and in the document DOM tree.
   * Overrides ItemXML::moveUp()
   */
  virtual void moveUp();

  /**
   * Inherited method.
   * Move item down in the QTreeWidget and in the document DOM tree.
   * Overrides ItemXML::moveDown()
   */
  virtual void moveDown();

  /**
   * Inherited method.
   * Overrides ParameterXML::contextMenu() to add 5 menu items to the context
   * menu: "Set Name", "Set Parameters", "Up", "Down" and "Delete"
   * @param[in,out] _menu the context menu
   */
  virtual void contextMenu(QMenu& _menu);

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------
  
  /** The name of the XML element for a ParameterInstance. */
  static QString const XML_TAG;

  /** The name of the XML attribute for a ParameterInstance's type. */
  static QString const XML_ATTRIBUTE_TYPE;
};

#endif // ParameterInstance_h
