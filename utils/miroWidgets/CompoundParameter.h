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

#ifndef CompoundParameter_h
#define CompoundParameter_h

// This application
#include "ParameterXML.h"
#include "params/Type.h"

#include "miroWidgets_Export.h"

class QTreeWidgetItem;

/**
 * Class representing compound parameter items.
 * A compound parameter a structure,  with a fixed number of elements of
 * possibliy-differing data types.
 */
class miroWidgets_Export CompoundParameter : public ParameterXML
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
  // public methods
  //----------------------------------------------------------------------------

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidgetItem.
   * If the QTreeWidget has at least 3 columns, write the type name in column 2.
   * @param[in] _type the type of the QDomNode
   * @param[in] _node Reference to the node in the xml document
   * @param[in] _parentItem pointer to the QTreeWidgetItem for this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  CompoundParameter(Miro::CFG::Type const& _type,
		    QDomNode const& _node,
		    QTreeWidgetItem * _treeWidgetItem,
		    QTreeWidgetItem * _pre,
		    QObject * _parent,
		    const char * _name);
 /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidget (top level).
   * If the QTreeWidget has at least 3 columns, write the type name in column 2.
   * @param[in] _type the type of the QDomNode
   * @param[in] _node Reference to the node in the xml document
   * @param[in] _view the QTreeWidget which contains the QTreeWidgetItem for
   * this CompoundParameter
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   * representing the item.
   */
  CompoundParameter(Miro::CFG::Type const& _type,
		    QDomNode const& _node,
		    QTreeWidget * _list, QTreeWidgetItem * _pre,
		    QObject * _parent, const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  /**
   * Inherited method.
   * Overrides ParameterXML::init().
   * Create the QWidgetItem subtree for the compound parameter using the
   * specification in the DOM tree.
   * @throw Miro::Exception if a child's ?
   * the n
   * @throw Miro::Exception if a child's type name is unknown
   */
  virtual void init();

  /**
   * Inherited method.
   * Show a dialog to get the value for each field of the CompoundParameter.
   * If successful, copy the changes to the DOM tree and notify that the
   * CompoundParameter has been modified.
   * Overrides ParameterXML::setParameters().
   */
  virtual void setParameters();

  //----------------------------------------------------------------------------
  // public static methods
  //----------------------------------------------------------------------------

  /**
   * Translates a XML node and name into a CFG::Type.
   * Return an enum value for the attribute named _name in the given QDomNode.
   * @param[in] _node a QDomNode corresponding the the CompoundParameter
   * @param[in] _attribute the name of an attribute of the QDomNode
   * @return a Miro::CFG::Type for the supplied attribute.
   * @throw Miro::Exception if the node's attribute's value can not be mapped
   * to a Miro::CFG::Type
   */
  static Miro::CFG::Type const& type(QDomNode const& _node,
				     QString const& _attribute);

protected:

  /**
   * Type description of the compound parameter, supplied by the derived
   * class.
   */
  Miro::CFG::Type type_;
};

#endif
