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

#ifndef Section_h
#define Section_h

// This application
#include "miroWidgets/ItemXML.h"
// The Qt library
#include <qstring.h>

// Forward declarations
class QTreeWidgetItem;

/**
 * A Section is a partition of a document, used for organizing. Sections are
 * children of the root DocumentXML item (labeled "config"). Every parameter
 * is a child of a Section, and every Section is a child of the root DocumentXML
 * item.
 */
class Section : public ItemXML
{
  Q_OBJECT
  
  /**
   * This is the immediate base class.
   */
  typedef ItemXML Super;

public:

  /**
   * Initializing constructor.
   * Write the name of the Section in column 2 of the QTreeWidget.
   * Create ItemXML object for each child of the Section.
   * @param[in] _node reference to the node in the DOM tree
   * @param[in] _parentItem pointer to the QTreeWidgetItem for the parent of
   * this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   */
  Section(QDomNode const& _node,
	  QTreeWidgetItem * _parentItem,
	  QTreeWidgetItem * _pre,
	  QObject * _parent,
	  const char * _name);

  //----------------------------------------------------------------------------  
  // inherited public methods
  //----------------------------------------------------------------------------  
  /**
   * Populate with one menu item for each Section for all loaded paameters
   * that hasn't yet been added.
   * Overrides ItemXML::contextMenu().
   * @param[in,out] _menu the context menu
   */
  virtual void contextMenu(QMenu& _menu);

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  /** The name of the XML element for a Section. */
  static QString const XML_TAG;

protected slots:

  /**
   * Add a named instance of a parameter type.
   * The slot invoked when the user selects the "Add Instance" context menu
   * item.
   * Show a QInputDialog soliciting the name of the instance. On success,
   * and if the name is not empty, create a new DOM tree node for the 
   * parameter instance and a new QTreeWidgetItem for it. On failure, show a 
   * QMessageBox with a warning message.
   * @param[in] _n a serial enumerator that becomes the DOM tree node's key
   */
  void onAddInstance(int _n);

  /**
   * Add a default instance of a parameter.
   * The slot invoked when the user selects the "Add Parameter" constext menu
   * item.
   * Create a ne wQTreeWidgetItem for the parameter. On failure, show a
   * QMessageBox with a warning message.
   * @param[in] _n a serial enumerator that becomes the DOM tree node's key
   */
  void onAddParameter(int _n);

protected:

  /**
   * Build the subtree from the XML document.
   * Create the ParameterInstance or ParameterSingleton ItemXML for each
   * child of the Secton's DOM tree node.
   */
  void buildSubtree();

  /** The submenu of parameters that can be added. */
  QMenu * menuAddParameter_;

  /** The submenu of types for which instances can be added. */
  QMenu * menuAddInstance_;
};

#endif
