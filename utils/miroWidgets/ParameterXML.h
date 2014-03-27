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

#ifndef ParameterXML_h
#define ParameterXML_h

// This application
#include "ItemXML.h"
#include "params/Type.h"
// The Qt library
#include <qstring.h>

#include "miroWidgets_Export.h"

// Forward declarations
class ConfigFile;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * Base class of all parameter objects of the configuration framework.
 * There are three immediate derived classes: CompoundParameter for structures,
 * ParameterList for std::vector of parameters and SimpleParameter for primitive
 * types, std::string and user-defiend classes.
 */
class miroWidgets_Export ParameterXML : public ItemXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  /**
   * Base class type.
   * The immediate base class.
   */
  typedef ItemXML Super;
public:

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidgetItem.
   * @param[in] _node Reference to the node in the xml document
   * @param[in] _parentItem pointer to the QTreeWidgetItem for the parent of
   * this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  ParameterXML(QDomNode const& _node,
	       QTreeWidgetItem * _parentItem,
	       QTreeWidgetItem * _pre,
	       QObject * _parent,
	       const char * _name);

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidget.
   * @param[in] _node Reference to the node in the xml document
   * @param[in] _view the QTreeWidget which contains the QTreeWidgetItem for
   * this ItemXML
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  ParameterXML(QDomNode const& _node,
	       QTreeWidget * _view,
	       QTreeWidgetItem * _pre,
	       QObject * _parent,
	       const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  /**
   * Inherited method.
   * @warning This does NOT override DocumentXML::init(), since it has a
   * different signature. It overloads it, doing nothing.
   */
  virtual void init();

  /**
   * Inherited method.
   * Overrides DocumentXML::contextMenu() to add 4 menu items to the context
   * menu: "Set Parameters", "Up", "Down" and "Delete".
   * @param[in,out] _menu the context menu
   */
  virtual void contextMenu(QMenu& _menu);

  /**
   * Inherited method.
   * ParameterXML does not inherit this member function from a base class.
   * It defines the virtual member function.
   * The default implementation does nothing.
   */
  virtual void setParameters();

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  /**
   * The parameter tag in the xml tree.
   * The name of the XML element in the DOM tree.
   */
  static QString const XML_TAG;

public slots:

  //----------------------------------------------------------------------------
  // public slots
  //----------------------------------------------------------------------------

  /**
   * The slot, when invoked, calls virtual member function
   * ParameterXML::setParameters().
   */
  void slotSetParameters();

protected:

  //----------------------------------------------------------------------------
  // puprotected members
  //----------------------------------------------------------------------------

  /** A new instance of class ConfigFile. */
  ConfigFile * config_;

private:

  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------

  /** Disable the copy constructor. */
  ParameterXML(ParameterXML const&);
};

#endif // ParameterXML_h
