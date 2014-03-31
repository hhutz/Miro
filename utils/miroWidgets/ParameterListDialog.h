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

#ifndef ParameterListDialog_h
#define ParameterListDialog_h

// This application
#include "DialogXML.h"
#include "ParameterList.h"
#include "params/Parameter.h"

#include "miroWidgets_Export.h"

// Forward declarations
class ConfigFile;
class QParameterDialog;
class QPushButton;
class QString;
class QStringList;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * This class shows a dialog for the behaviour parameters.
 * A ParamaterList is an std::vector or std::set of parameters.
 */
class miroWidgets_Export ParameterListDialog : public DialogXML
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef DialogXML Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  /**
   * The only constructor.
   * Make and store a copy of the DOM tree.
   * Create a QTreeWidget
   * @param[in] _type an enum specifying the data type of the value being edited
   * @param[in] _parameter a struct representing the parameter being edited
   * @param[in] _parentNode the parent of _node
   * @param[in] _node the node in the DOM tree being edited
   * @param[in] _parentItem the ItemXML corresponding to _parentNode
   * @param[in] _item the ItemXML corresponding to _node
   * @param[in] _parent the parent of this dialog widget
   * @param[in] _name the name of this dialog widget
   */
  ParameterListDialog(ParameterList::Type _type,
		      Miro::CFG::Parameter const& _parameter,
		      QDomNode const& _parentNode,
		      QDomNode const& _node,
		      ItemXML * _parentItem,
		      ItemXML * _item,
		      QWidget * _parent,
		      const char * _name = NULL);

  void setXML();

protected slots:

  //----------------------------------------------------------------------------
  // protected slots
  //----------------------------------------------------------------------------

  /** Not used, and has an empty function body. */
  void up();

  void down();

  /** Not used, and has an empty function body. */
  void add();

  /**
   * The slot invoked when the user clicks the button labeled "Remove".
   */
  void del();

  void edit();

  void slotDoubleClick(QTreeWidgetItem *, int);

  /**
   * The slot invoked when the user right clicks on a QTreeWidgetItem, causing
   * the context menu to be shown.
   * @param[in] pos the position of the mouse click
   */
  void contextMenu(const QPoint& pos);

protected:

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  /**
   * If the QListWidget has any QListWidgetItems, select the current one and
   * enable the "Edit" and "Remove" buttons.
   */
  void selectListItem();

  /** Serially enumerate each child of the top-level QListWidgetItem. */
  void renumberEntries();

  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Type description of the parameter.
  ParameterList::Type type_;

  //! Description of the parameter.
  Miro::CFG::Parameter parameter_;

  //! Flag indicating that the nested type is actually a compound itself.
  bool nestedCompound_;

  //! Type description of the nested type.
  Miro::CFG::Type const * nestedType_;

  unsigned int index_;

  //! The list view.
  QTreeWidget * list_;

  //! The delete button.
  QPushButton * delButton_;

  //! The edit button.
  QPushButton * editButton_;

  //! Dummy object for memory management.
  /** Making it the parent of a QObject descandand cleans the child
      up, on destruction of this instance. */
  QObject collector_;

  //! A temporary document, holding a copy of the XML node.
  QDomDocument tmpDocument_;

  //! The parent node in the temporary document.
  QDomNode tmpParentNode_;

  //! The temporary dublicate of the node.
  QDomNode tmpNode_;
};

#endif
