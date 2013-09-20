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
#include <q3listview.h>
//Added by qt3to4:
#include <Q3PopupMenu>

#include <cassert>

Item::ItemMap Item::itemMap_;

//----------------------------------------------------------------------------
// public methods
//----------------------------------------------------------------------------

Item::Item(Q3ListViewItem * _parentItem, Q3ListViewItem * _pre,
	   QObject * _parent, const char * _name) :
  Super(_parent, _name),
  listViewItem_(new Q3ListViewItem(_parentItem, _pre))
{
  listViewItem()->setText(0, name());
  itemMap_.insert(std::make_pair(listViewItem_, this));
}

Item::Item(Q3ListView * _view, Q3ListViewItem * _pre,
	   QObject * _parent, const char * _name) :
  Super(_parent, _name),
  listViewItem_(new Q3ListViewItem(_view, _pre))
{
  listViewItem()->setText(0, name());
  itemMap_.insert(std::make_pair(listViewItem_, this));
}

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

  delete listViewItem_;
  itemMap_.erase(listViewItem_);

  //  cout << name() << " deleting" << endl;
}

void
Item::update()
{
}

void 
Item::contextMenu(Q3PopupMenu&)
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
  // find the pre predecessor of the item
  Q3ListViewItem * prePre = NULL;
  Q3ListViewItem * pre = NULL;
  Q3ListViewItem * self = NULL;

  // get the parents first child
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
}

void
Item::moveDown()
{
  Q3ListViewItem * succ = listViewItem()->nextSibling();
  if (succ != NULL) {
    listViewItem()->moveItem(succ);
  }
}

Item *
Item::predecessor()
{
  // find the pre predecessor of the item
  Q3ListViewItem * prePre = NULL;
  Q3ListViewItem * pre = NULL;
  Q3ListViewItem * self = NULL;

  // get the parents first child
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

  return itemFromListViewItem(pre);
}

Item *
Item::successor()
{
  return itemFromListViewItem(listViewItem_->nextSibling());
}

Item *
Item::itemFromListViewItem(Q3ListViewItem * _lvi)
{
  if (_lvi == NULL)
    return NULL;

  ItemMap::const_iterator i = itemMap_.find(_lvi);
  assert(i != itemMap_.end());
  return i->second;
}
