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

#include "DialogXML.h"
#include "ParameterList.h"

#include "params/Parameter.h"

#include "miroWidgets_Export.h"

// forward declarations
class Q3ListView;
class Q3ListViewItem;
class QPushButton;
class QString;
class QStringList;
class QParameterDialog;

class ConfigFile;

/**
 * This class shows a dialog for the behaviour parameters.
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

  ParameterListDialog(ParameterList::Type _type,
		      Miro::CFG::Parameter const& _parameter,
		      QDomNode const& _parentNode, QDomNode const& _node,
		      ItemXML * _parentItem, ItemXML * _item,
		      QWidget * _parent, const char * _name = NULL);

  void setXML();

protected slots:
  //----------------------------------------------------------------------------
  // protected slots
  //----------------------------------------------------------------------------

  void up();
  void down();
  void add();
  void del();
  void edit();

  void slotDoubleClick(Q3ListViewItem *);
  void contextMenu(Q3ListViewItem*,const QPoint&,int);

protected:
  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  void selectListItem();
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
  Q3ListView * list_;
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
