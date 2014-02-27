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
// Enable migration from Qt v3 to Qt v4
#define LSB_Q3LISTVIEWITEM
#define LSB_Q3POPUPMENU

#ifndef ParameterInstance_h
#define ParameterInstance_h

#include "miroWidgets/CompoundParameter.h"
#ifdef LSB_Q3POPUPMENU
class QMenu;
#else
//Added by qt3to4:
#include <Q3PopupMenu>
#endif

// forward declarations
class ConfigFile;

#ifdef LSB_Q3LISTVIEWITEM
class QTreeWidgetItem;
#else
class Q3ListViewItem;
#endif

class ParameterInstance : public CompoundParameter
{
  Q_OBJECT
  
  typedef CompoundParameter Super;
public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------
#ifdef LSB_Q3LISTVIEWITEM
  ParameterInstance(QTreeWidgetItem * _listViewItem, QTreeWidgetItem * _pre,
#else
  ParameterInstance(Q3ListViewItem * _listViewItem, Q3ListViewItem * _pre,
#endif
		    QDomNode const& _node,
		    QObject * _parent, const char * _name);

  //----------------------------------------------------------------------------
  // public inherited methods
  //----------------------------------------------------------------------------

  //! Inherited method.
  virtual void moveUp();
  //! Inherited method.
  virtual void moveDown();
  //! Inherited method.
#ifdef LSB_Q3POPUPMENU
  virtual void contextMenu(QMenu& _menu);
#else
  virtual void contextMenu(Q3PopupMenu& _menu);
#endif

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------
  static QString const XML_TAG;
  static QString const XML_ATTRIBUTE_TYPE;
};

#endif // ParameterInstance_h
