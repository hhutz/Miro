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
#include "Item.h"

#include <qstring.h>
#include <qobject.h>
#ifdef LSB_Q3LISTVIEW
#include <QTreeWidget>
#else
#include <q3listview.h>
#endif
#ifdef LSB_Q3POPUPMENU
#include <QMenu>
#else
//Added by qt3to4:
#include <Q3PopupMenu>
#endif

#include <cassert>

Item::ItemMap Item::itemMap_;

//----------------------------------------------------------------------------
// public methods
//----------------------------------------------------------------------------

#ifdef LSB_Q3LISTVIEWITEM
Item::Item(QTreeWidgetItem * _parentItem, QTreeWidgetItem * _pre,
	   QObject * _parent, const char * _name) :
  Super(_parent, _name),
  treeWidgetItem_(new QTreeWidgetItem(_parentItem, _pre))
{
  treeWidgetItem()->setText(0, name());
  itemMap_.insert(std::make_pair(treeWidgetItem_, this));
}
#else
Item::Item(Q3ListViewItem * _parentItem, Q3ListViewItem * _pre,
	   QObject * _parent, const char * _name) :
  Super(_parent, _name),
  listViewItem_(new Q3ListViewItem(_parentItem, _pre))
{
  listViewItem()->setText(0, name());
  itemMap_.insert(std::make_pair(listViewItem_, this));
}
#endif


#if defined(LSB_Q3LISTVIEWITEM) && defined(LSB_Q3LISTVIEW)
Item::Item(QTreeWidget * _view, QTreeWidgetItem * _pre,
	   QObject * _parent, const char * _name) :
  Super(_parent, _name),
  treeWidgetItem_(new QTreeWidgetItem(_view, _pre))
{
  treeWidgetItem()->setText(0, name());
  itemMap_.insert(std::make_pair(treeWidgetItem_, this));
}
#else
Item::Item(Q3ListView * _view, Q3ListViewItem * _pre,
	   QObject * _parent, const char * _name) :
  Super(_parent, _name),
  listViewItem_(new Q3ListViewItem(_view, _pre))
{
  listViewItem()->setText(0, name());
  itemMap_.insert(std::make_pair(listViewItem_, this));
}
#endif


Item::~Item()
{
  //  cout << name() << " deleting children" << endl;

  if (!children().isEmpty()) {
    QObjectList childList = children();
    while(!childList.isEmpty() ) {
      delete childList.takeFirst();
    }
  }

  //  cout << name() << " deleting listviewitem" << endl;

#ifdef LSB_Q3LISTVIEWITEM
  delete treeWidgetItem_;
  itemMap_.erase(treeWidgetItem_);
#else
  delete listViewItem_;
  itemMap_.erase(listViewItem_);
#endif

  //  cout << name() << " deleting" << endl;
}

void
Item::update()
{
}

void 
#ifdef LSB_Q3POPUPMENU
Item::contextMenu(QMenu&)
#else
Item::contextMenu(Q3PopupMenu&)
#endif
{
}

//------------------------------------------------------------------------------
// public slots
//------------------------------------------------------------------------------

void 
Item::up()
{
  moveUp();
}

void
Item::down()
{
  moveDown();
}

void
Item::moveUp()
{
#ifdef LSB_Q3LISTVIEWITEM
  // Map this Item onto its QTreeWidgetItem
  QTreeWidgetItem * const pThisTreeWidgetItem = treeWidgetItem();
  assert(pThisTreeWidgetItem != NULL);

  // Fetch this Item's parent QTreeWidgetItem.
  // If the QTreeWidgetItem doesn't supply it, use its QTreeWidget
  QTreeWidgetItem * const pParentTreeWidgetItem =
    pThisTreeWidgetItem->parent() ?
    pThisTreeWidgetItem->parent() :
    pThisTreeWidgetItem->treeWidget()->invisibleRootItem();
  assert(pParentTreeWidgetItem != NULL);

  // Fetch the index of pThisTreeWidgetItem wrt pParentTreeWidgetItem
  const int thisIndex =
    pParentTreeWidgetItem->indexOfChild(pThisTreeWidgetItem);
  assert((0 <= thisIndex) && (thisIndex < pParentTreeWidgetItem->childCount()));

  if (thisIndex > 1)
  {
    // This item can be inserted after thisIndex - 2
    pParentTreeWidgetItem->removeChild(pThisTreeWidgetItem);
    pParentTreeWidgetItem->insertChild(thisIndex - 2, pThisTreeWidgetItem);
  }
  else if (thisIndex == 1)
  {
    // Fetch the predecessor, which is at index thisIndex - 1 == 0
    QTreeWidgetItem * const pPredecessorTreeWidgetItem =
      pParentTreeWidgetItem->child(0);
    // The predecessor can be inserted after this item, which will have index
    // 0 after removing the predecessor
    pParentTreeWidgetItem->removeChild(pPredecessorTreeWidgetItem);
    pParentTreeWidgetItem->insertChild(0, pPredecessorTreeWidgetItem);
  }
#else
  // find the pre predecessor of the item
  Q3ListViewItem * prePre = NULL;
  Q3ListViewItem * pre = NULL;
  Q3ListViewItem * self = NULL;

  // get the parents' first child
  self = (listViewItem()->parent() == NULL)?
    listViewItem()->listView()->firstChild() :
    listViewItem()->parent()->firstChild();

  // find ourselves
  while (self != listViewItem()) {
    assert(self != NULL);

    prePre = pre;
    pre = self;
    self = self->nextSibling();
  }

  if (pre != NULL) {
    if (prePre != NULL) {
      listViewItem()->moveItem(prePre);
    }
    else {
      pre->moveItem(listViewItem());
    }
  }
#endif
}

void
Item::moveDown()
{
#ifdef LSB_Q3LISTVIEWITEM
  // Map this Item onto its QTreeWidgetItem
  QTreeWidgetItem * const pThisTreeWidgetItem = treeWidgetItem();
  assert(pThisTreeWidgetItem != NULL);

  // Fetch this Item's parent QTreeWidgetItem.
  // If the QTreeWidgetItem doesn't supply it, use its QTreeWidget
  QTreeWidgetItem * const pParentTreeWidgetItem =
    pThisTreeWidgetItem->parent() ?
    pThisTreeWidgetItem->parent() :
    pThisTreeWidgetItem->treeWidget()->invisibleRootItem();
  assert(pParentTreeWidgetItem != NULL);

  // Fetch this QTreeWidgetItem's index
  const int thisIndex =
    pParentTreeWidgetItem->indexOfChild(pThisTreeWidgetItem);

  if (thisIndex < pParentTreeWidgetItem->childCount() - 1)
  {
    // It has a successor
    // 
    // This QTreeWidgetItem can be inserted after its successor, which will
    // have index thisIndex after removing this QTreeWidgetItem
    pParentTreeWidgetItem->removeChild(pThisTreeWidgetItem);
    pParentTreeWidgetItem->insertChild(thisIndex, pThisTreeWidgetItem);
  }
#else
  Q3ListViewItem * succ = listViewItem()->nextSibling();
  if (succ != NULL) {
    listViewItem()->moveItem(succ);
  }
#endif
}

/**
 * Return the Item with precedes this Item.
 * It is the Item corresponding the the predecessor of this item's
 * QTreeViewItem.
 */
Item *
Item::predecessor()
{
#ifdef LSB_Q3LISTVIEWITEM
  // Map this Item onto its QTreeWidgetItem
  QTreeWidgetItem * const pThisTreeWidgetItem = treeWidgetItem();
  assert(pThisTreeWidgetItem != NULL);

  // Fetch this Item's parent QTreeWidgetItem.
  // If the QTreeWidgetItem doesn't supply it, use its QTreeWidget
  QTreeWidgetItem * const pParentTreeWidgetItem =
    pThisTreeWidgetItem->parent() ?
    pThisTreeWidgetItem->parent() :
    pThisTreeWidgetItem->treeWidget()->invisibleRootItem();
  assert(pParentTreeWidgetItem != NULL);

  // Fetch the index of pThisTreeWidgetItem wrt pParentTreeWidgetItem
  const int thisIndex =
    pParentTreeWidgetItem->indexOfChild(pThisTreeWidgetItem);
  assert((0 <= thisIndex) && (thisIndex < pParentTreeWidgetItem->childCount()));

  // Fetch the predecessor; NULL if there is none
  QTreeWidgetItem * const pPredecessorTreeWidgetItem =
    (thisIndex == 0) ?  NULL : pParentTreeWidgetItem->child(thisIndex - 1);

  // Map the predecessor's QTreeWidgetItem onto its Item
  Item * const result = itemFromTreeWidgetItem(pPredecessorTreeWidgetItem);

  return result;

  // Fetch the index of this
#else
  // find the pre predecessor of the item
  Q3ListViewItem * pre = NULL;
  Q3ListViewItem * self = NULL;

  // get the parents first child
  self = (listViewItem()->parent() == NULL)?
    listViewItem()->listView()->firstChild() :
    listViewItem()->parent()->firstChild();

  // find ourselves
  while (self != listViewItem()) {
    assert(self != NULL);

    pre = self;
    self = self->nextSibling();
  }

  return itemFromListViewItem(pre);
#endif
}

Item *
Item::successor()
{
#ifdef LSB_Q3LISTVIEWITEM
  // Map this Item onto its QTreeWidgetItem
  QTreeWidgetItem * const pThisTreeWidgetItem = treeWidgetItem();
  assert(pThisTreeWidgetItem != NULL);

  // Fetch this Item's parent QTreeWidgetItem.
  // If the QTreeWidgetItem doesn't supply it, use its QTreeWidget
  QTreeWidgetItem * const pParentTreeWidgetItem =
    pThisTreeWidgetItem->parent() ?
    pThisTreeWidgetItem->parent() :
    pThisTreeWidgetItem->treeWidget()->invisibleRootItem();
  assert(pParentTreeWidgetItem != NULL);

  // Fetch the index of pThisTreeWidgetItem wrt pParentTreeWidgetItem
  const int thisIndex =
    pParentTreeWidgetItem->indexOfChild(pThisTreeWidgetItem);
  assert((0 <= thisIndex) && (thisIndex < pParentTreeWidgetItem->childCount()));
  
  QTreeWidgetItem * const pSuccessorTreeWidgetItem =
    (thisIndex < pParentTreeWidgetItem->childCount() - 1) ?
    // It has a successor
    pParentTreeWidgetItem->child(thisIndex + 1) :
    // There is no successor
    NULL;

  // Map the succcessor QTreeWidgetItem onto its Item
  Item * const result = itemFromTreeWidgetItem(pSuccessorTreeWidgetItem);
  return result;
#else
  return itemFromListViewItem(listViewItem_->nextSibling());
#endif
}

Item *
#ifdef LSB_Q3LISTVIEWITEM
Item::itemFromTreeWidgetItem(QTreeWidgetItem * _lvi)
#else
Item::itemFromListViewItem(Q3ListViewItem * _lvi)
#endif
{
  if (_lvi == NULL)
    return NULL;

  ItemMap::const_iterator i = itemMap_.find(_lvi);
  assert(i != itemMap_.end());
  return i->second;
}
