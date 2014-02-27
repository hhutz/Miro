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
#define LSB_Q3LISTVIEW
#define LSB_Q3LISTVIEWITEM
#define LSB_Q3POPUPMENU

#ifndef Item_h
#define Item_h

#include <qobject.h>
#ifdef LSB_Q3POPUPMENU
#else
//Added by qt3to4:
#include <Q3PopupMenu>
#endif

#include <map>

#include "miroWidgets_Export.h"

// forward declarations
#ifdef LSB_Q3POPUPMENU
class QMenu;
#else
class Q3PopupMenu;
#endif
#ifdef LSB_Q3LISTVIEW
class QTreeWidget;
#else
class Q3ListView;
#endif
#ifdef LSB_Q3LISTVIEWITEM
class QTreeWidgetItem;
#else
class Q3ListViewItem;
#endif

//! This class represents a QTreeWidgetItem descendant
/** 
 * As we want a QObject as base class for signals and slots,
 * we need to tie the QTreeWidgetItem and the Item class together
 * by the use of a std::map. Sorry for the inconvenience.
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
#ifdef LSB_Q3LISTVIEWITEM
  typedef std::map<QTreeWidgetItem *, Item *> ItemMap;
#else
  typedef std::map<Q3ListViewItem *, Item *> ItemMap;
#endif

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor, creating a QListView sibling item.
#ifdef LSB_Q3LISTVIEWITEM
  Item(QTreeWidgetItem * _parentItem, QTreeWidgetItem * _pre = NULL,
       QObject * _parent = NULL, const char * _name = NULL);
#else
  Item(Q3ListViewItem * _parentItem, Q3ListViewItem * _pre = NULL,
       QObject * _parent = NULL, const char * _name = NULL);
#endif
  //! Initializing constructor, creating a QListView toplevel item.
#if defined(LSB_Q3LISTVIEWITEM) && defined(LSB_Q3LISTVIEW)
  Item(QTreeWidget * _view, QTreeWidgetItem * _pre = NULL,
       QObject * _parent = NULL, const char * _name = NULL);
#else
  Item(Q3ListView * _view, Q3ListViewItem * _pre = NULL,
       QObject * _parent = NULL, const char * _name = NULL);
#endif
  //! Virtual destructor.
  virtual ~Item();

#ifdef LSB_Q3LISTVIEWITEM
  //! Accessor for the associated list view.
  QTreeWidgetItem * treeWidgetItem();
  //! Const accessor for the associated list view.
  QTreeWidgetItem const * treeWidgetItem() const;
#else
  //! Accessor for the associated list view.
  Q3ListViewItem * listViewItem();
  //! Const accessor for the associated list view.
  Q3ListViewItem const * listViewItem() const;
#endif

  //! Move item up in list view.
  virtual void moveUp();
  //! Move item down in list view.
  virtual void moveDown();
  //! Update the item tree.
  virtual void update();
  //! Populate the provided context menu with entries for the item.
#ifdef LSB_Q3POPUPMENU
  virtual void contextMenu(QMenu& _menu);
#else
  virtual void contextMenu(Q3PopupMenu& _menu);
#endif

  //----------------------------------------------------------------------------
  // static public methods
  //----------------------------------------------------------------------------

  //! Accesor for the QListViewItem/Item map.
  static ItemMap const& itemMap();
  //! Retrieve the associated Item for a QListView.
#ifdef LSB_Q3LISTVIEWITEM
  static Item * itemFromTreeWidgetItem(QTreeWidgetItem * _lvi);
#else
  static Item * itemFromListViewItem(Q3ListViewItem * _lvi);
#endif

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

#ifdef LSB_Q3LISTVIEWITEM
  //! Pointer to the corresponding QTreeWidgetItem.
  QTreeWidgetItem * treeWidgetItem_;
#else
  //! Pointer to the corresponding QListViewItem.
  Q3ListViewItem * listViewItem_;
#endif

  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  Item(Item const&);
};

#ifdef LSB_Q3LISTVIEWITEM
inline
QTreeWidgetItem *
Item::treeWidgetItem() {
  return treeWidgetItem_;
}
#else
inline
Q3ListViewItem *
Item::listViewItem() {
  return listViewItem_;
}
#endif
#ifdef LSB_Q3LISTVIEWITEM
inline
const QTreeWidgetItem *
Item::treeWidgetItem() const {
  return treeWidgetItem_;
#else
inline
Q3ListViewItem const *
Item::listViewItem() const {
  return listViewItem_;
#endif
}
inline
Item::ItemMap const&
Item::itemMap() {
  return itemMap_;
}

#endif
