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

// The Qt library
#include <qobject.h>
// The C++ Standard Library
#include <map>

#include "miroWidgets_Export.h"

// Forward declarations
class QMenu;
class QTreeWidget;
class QTreeWidgetItem;

/** 
 * This class represents a QTreeWidgetItem descendant
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

  /**
   * Mapping QTreeWidgetItem instances to Item instances.
   * the Item constructors insert entries and the Item destructor erases
   * entries.
   */
  typedef std::map<QTreeWidgetItem *, Item *> ItemMap;

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor, creating a QTreeWidget sibling item.
  Item(QTreeWidgetItem * _parentItem,
       QTreeWidgetItem * _pre = NULL,
       QObject * _parent = NULL,
       const char * _name = NULL);

  //! Initializing constructor, creating a QTreeWidget toplevel item.
  Item(QTreeWidget * _view,
       QTreeWidgetItem * _pre = NULL,
       QObject * _parent = NULL,
       const char * _name = NULL);

  //! Virtual destructor.
  virtual ~Item();

  //! Accessor for the associated QTreeWidget.
  QTreeWidgetItem * treeWidgetItem();
  //! Const accessor for the associated QTreeWidget.
  QTreeWidgetItem const * treeWidgetItem() const;

  //! Move item up in list view.
  virtual void moveUp();
  //! Move item down in list view.
  virtual void moveDown();
  //! Update the item tree.
  virtual void update();
  //! Populate the provided context menu with entries for the item.
  virtual void contextMenu(QMenu& _menu);

  //----------------------------------------------------------------------------
  // static public methods
  //----------------------------------------------------------------------------

  //! Accesor for the QListViewItem/Item map.
  static ItemMap const& itemMap();
  //! Retrieve the associated Item for a QListView.
  static Item * itemFromTreeWidgetItem(QTreeWidgetItem * _lvi);

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

  //! Pointer to the corresponding QTreeWidgetItem.
  QTreeWidgetItem * treeWidgetItem_;

  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  Item(Item const&);
};

inline
QTreeWidgetItem *
Item::treeWidgetItem() {
  return treeWidgetItem_;
}

inline
const QTreeWidgetItem *
Item::treeWidgetItem() const {
  return treeWidgetItem_;
}
inline
Item::ItemMap const&
Item::itemMap() {
  return itemMap_;
}

#endif
