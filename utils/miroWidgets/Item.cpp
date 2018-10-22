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
#include <QTreeWidget>
#include <cassert>

Item::ItemMap Item::itemMap_;

//----------------------------------------------------------------------------
// public methods
//----------------------------------------------------------------------------

Item::Item(QTreeWidgetItem * _parentItem,
	   QTreeWidgetItem * _pre,
	   QObject * _parent,
	   QString const& _name) :
  Super(_parent),
  treeWidgetItem_(new QTreeWidgetItem(_parentItem, _pre))
{
  setObjectName(_name);
  // Label the QTreeWidgetItem with the Item's name
  treeWidgetItem()->setText(0, _name);
  // Map the QTreeWidgetItem to the Item that it represents
  itemMap_.insert(std::make_pair(treeWidgetItem_, this));
}

Item::Item(QTreeWidget * _view, QTreeWidgetItem * _pre,
	   QObject * _parent, QString const& _name) :
  Super(_parent),
  treeWidgetItem_(new QTreeWidgetItem(_view, _pre))
{
  setObjectName(_name);
  treeWidgetItem()->setText(0, _name);
  // Map the QTreeWidgetItem to the Item that it represents
  itemMap_.insert(std::make_pair(treeWidgetItem_, this));
}

Item::~Item()
{
  //  cout << objectName() << " deleting children" << endl;

  if (!children().isEmpty()) {
    QObjectList childList = children();
    while(!childList.isEmpty() ) {
      delete childList.takeFirst();
    }
  }

  //  cout << objectName() << " deleting listviewitem" << endl;

  delete treeWidgetItem_;
  // Remove the QTreeWidgetItem from the map to Item
  itemMap_.erase(treeWidgetItem_);

  //  cout << objectName() << " deleting" << endl;
}

void
Item::update()
{
}

void
Item::contextMenu(QMenu&)
{
  // The context menu is empty.
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

  if (thisIndex != 0)
  {
    // This item has a predecessor, so it can be moved up
    const int newIndex = thisIndex - 1;
    pParentTreeWidgetItem->removeChild(pThisTreeWidgetItem);
    pParentTreeWidgetItem->insertChild(newIndex, pThisTreeWidgetItem);
  }
}

void
Item::moveDown()
{
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

  if (thisIndex != pParentTreeWidgetItem->childCount() - 1)
  {
    // This item has a successor, so it can be moved up
    const int newIndex = thisIndex + 1;
    pParentTreeWidgetItem->removeChild(pThisTreeWidgetItem);
    pParentTreeWidgetItem->insertChild(newIndex, pThisTreeWidgetItem);
  }
}

/**
 * Return the Item with precedes this Item.
 * It is the Item corresponding the the predecessor of this item's
 * QTreeViewItem.
 */
Item *
Item::predecessor()
{
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
}

Item *
Item::successor()
{
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
}

Item *
Item::itemFromTreeWidgetItem(QTreeWidgetItem * _lvi)
{
  if (_lvi == NULL)
    return NULL;

  ItemMap::const_iterator i = itemMap_.find(_lvi);
  assert(i != itemMap_.end());
  return i->second;
}
