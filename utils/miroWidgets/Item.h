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
#ifndef Item_h
#define Item_h

#include <qobject.h>
//Added by qt3to4:
#include <Q3PopupMenu>

#include <map>

#include "miroWidgets_Export.h"

// forward declarations
class Q3PopupMenu;
class Q3ListView;
class Q3ListViewItem;

//! This class represents a QListViewItem descendand
/** 
 * As we want a QObject as base class for signals and slots,
 * we need to tie the QListViewItem and the Item class together
 * by the use of a std::map. Sorry, for the inconvenience.
 */
class miroWidgets_Export Item : public QObject
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef QObject Super;

public:
  //----------------------------------------------------------------------------
  // public types
  //----------------------------------------------------------------------------

  // Mapping QListViewItem instances to Item instances.
  typedef std::map<Q3ListViewItem *, Item *> ItemMap;

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor, creating a QListView sibling item.
  Item(Q3ListViewItem * _parentItem, Q3ListViewItem * _pre = NULL,
       QObject * _parent = NULL, const char * _name = NULL);
  //! Initializing constructor, creating a QListView toplevel item.
  Item(Q3ListView * _view, Q3ListViewItem * _pre = NULL,
       QObject * _parent = NULL, const char * _name = NULL);
  //! Virtual destructor.
  virtual ~Item();

  //! Accessor for the associated list view.
  Q3ListViewItem * listViewItem();
  //! Const accessor for the associated list view.
  Q3ListViewItem const * listViewItem() const;

  //! Move item up in list view.
  virtual void moveUp();
  //! Move item down in list view.
  virtual void moveDown();
  //! Update the item tree.
  virtual void update();
  //! Populate the provided context menu with entries for the item.
  virtual void contextMenu(Q3PopupMenu& _menu);

  //----------------------------------------------------------------------------
  // static public methods
  //----------------------------------------------------------------------------

  //! Accesor for the QListViewItem/Item map.
  static ItemMap const& itemMap();
  //! Retriev the associated Item for a QListView.
  static Item * itemFromListViewItem(Q3ListViewItem * _lvi);

public slots:
  //----------------------------------------------------------------------------
  // public sots
  //----------------------------------------------------------------------------

  //! Move the element before the predecessor element.
  void up();
  //! Move the element behind the successor element.
  void down();

protected:
  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  //! Get the pedecessor item in the QListView.
  Item * predecessor();
  //! Get the successor item in the QListView.
  Item * successor();

  //----------------------------------------------------------------------------
  // protected members
  //----------------------------------------------------------------------------

  //! The map instance for QListViewItem/Item association.
  static ItemMap itemMap_;

private:
  //----------------------------------------------------------------------------
  // private members
  //----------------------------------------------------------------------------

  //! Pointer to the corresponding QListViewItem.
  Q3ListViewItem * listViewItem_;

  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  Item(Item const&);
};

inline
Q3ListViewItem *
Item::listViewItem() {
  return listViewItem_;
}
inline
Q3ListViewItem const *
Item::listViewItem() const {
  return listViewItem_;
}
inline
Item::ItemMap const&
Item::itemMap() {
  return itemMap_;
}

#endif
