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
#include "EventView.h"
#include "FileSet.h"

#include "miro/Log.h"
#include "miro/TimeHelper.h"

#include <orbsvcs/CosNotifyChannelAdminC.h>

#include <q3listview.h>
#include <qlayout.h>
#include <q3vgroupbox.h>
//Added by qt3to4:
#include <QHideEvent>
#include <Q3VBoxLayout>

#include <sstream>

EventView::EventView(FileSet * _fileSet, unsigned int _history, char const * _name) :
    Super(NULL, _name),
    fileSet_(_fileSet),
    list_(NULL),
    last_(NULL),
    internalSetSelection_(false),
    history_(_history)
{
  setCaption("Event View");

  Q3VBoxLayout * topBox = new Q3VBoxLayout(this, 0, -1, "boxLayout");

  Q3VGroupBox * eventBox = new Q3VGroupBox(this, "eventBox");
  list_ = new Q3ListView(eventBox, "list");

  list_->addColumn("Time Stamp");
  list_->addColumn("Domain Name");
  list_->addColumn("Type Name");
  list_->addColumn("Event Name");
  list_->setSorting(-1);
  list_->setAllColumnsShowFocus(true);

  topBox->addSpacing(10);
  topBox->addWidget(eventBox);
  eventBox->setTitle("Events");

  // connect to file set events
  FileSet::FileVector::iterator first, last = fileSet_->fileVector().end();
  for (first = fileSet_->fileVector().begin(); first != last; ++first) {
    connect(*first, SIGNAL(newEvent(const QString&,const QString&,const QString&,const QString&)),
            this, SLOT(insertEvent(const QString&,const QString&,const QString&,const QString&)));
  }

  connect(list_, SIGNAL(selectionChanged(Q3ListViewItem *)),
          this, SLOT(selectionChanged(Q3ListViewItem *)));

  fileSet_->getEvents(fileSet_->coursorTime(), history_);

  show();
}

EventView::~EventView()
{
}

void
EventView::insertEvent(QString const& _stamp,
                       QString const& _domain, QString const& _type, QString const& _event)
{
  internalSetSelection_ = true;

  if (list_->selectedItem() == NULL &&
              list_->firstChild() != NULL) {
    list_->setSelected(list_->firstChild(), true);
  }

  // insert at the beginning
  if (list_->firstChild() == NULL ||
              _stamp < list_->firstChild()->text(0)) {

    new Q3ListViewItem(list_,
                       _stamp, _domain, _type, _event);
    list_->setSelected(list_->firstChild(), true);
    list_->ensureItemVisible(list_->firstChild());

    if (last_ == NULL)
      last_ = list_->firstChild();
  }
  // insert at the end
  else if (last_->text(0) < _stamp) {
    last_ = new Q3ListViewItem(list_, last_,
                               _stamp, _domain, _type, _event);
    list_->setSelected(last_, true);
    list_->ensureItemVisible(last_);
  }

  // insert somewher in between
  else if (list_->selectedItem() != NULL &&
              list_->selectedItem()->text(0) != _stamp) {
    Q3ListViewItem * item = list_->selectedItem();
    MIRO_ASSERT(item != NULL);

    // after current selection
    if (_stamp > item->text(0)) {

      Q3ListViewItem * prev = item;
      while (_stamp > item->text(0)) {
        prev = item;
        item = item->nextSibling();
      }

      // insert
      if (_stamp != item->text(0)) {
        item = new  Q3ListViewItem(list_, prev,
                                   _stamp, _domain, _type, _event);
      }
    }
    // before current selection
    else {
      while (_stamp < item->text(0)) {
        Q3ListViewItem * prev = list_->firstChild();
        while (prev->nextSibling() != item) {
          prev = prev->nextSibling();
          MIRO_ASSERT(prev != NULL);
        }
        item = prev;
      }

      if (_stamp != item->text(0)) {
        Q3ListViewItem * prev = list_->firstChild();
        while (prev->nextSibling() != item) {
          prev = prev->nextSibling();
          MIRO_ASSERT(prev != NULL);
        }
        item = new Q3ListViewItem(list_, prev,
                                  _stamp, _domain, _type, _event);
      }
    }
    list_->setSelected(item, true);
  }

  pruneHistory();
  internalSetSelection_ = false;
}

void
EventView::setHistory(unsigned int _size)
{
  MIRO_ASSERT(_size > 0);
  if (_size > history_) {
    history_ = _size;
    fileSet_->getEvents(eventTime(list_->selectedItem()), history_);
  }
  else {
    history_ = _size;
    pruneHistory();
  }
}

void
EventView::hideEvent(QHideEvent * )
{
  // delete on hiding
  this->deleteLater();
}

void
EventView::pruneHistory()
{
  // chop at the beginning till selected event
  int overlap = list_->childCount() - (int)history_;
  while (overlap > 0 &&
              list_->firstChild() != list_->selectedItem()) {
    if (list_->firstChild() == last_)
      last_ = NULL;
    delete list_->firstChild();
    --overlap;
  }
  /*
  if (overlap > 0) {
    QListViewItem * item = list_->firstChild();
    for (unsigned int i = 0; i < history_; ++i) {
      item = item->nextSibling();
    }

    std::vector<QListViewItem *> v;
    v.reserve(overlap);
    while (item != NULL) {
      v.push_back(item);
      item = item->nextSibling();
    }
    while (v.size() > 0) {
      delete v.back();
      v.pop_back();
    }
  }
  */
}

void
EventView::selectionChanged(Q3ListViewItem * _item)
{
  if (!internalSetSelection_) {
    ACE_Time_Value t = eventTime(_item);
    fileSet_->coursorTimeRel(t);
  }
}

void
EventView::excludeEvent(const QString& _domainName, const QString& _typeName)
{
  Q3ListViewItem * item = list_->firstChild();
  Q3ListViewItem * pre = NULL;
  while (item != NULL) {
    Q3ListViewItem * next = item->nextSibling();
    if (item->text(1) == _domainName &&
                item->text(2) == _typeName) {
      // if the item to delete is selected
      if (list_->selectedItem() == item) {
        // we take the previous one if possible
        if (pre != NULL) {
          list_->setSelected(pre, true);
        }
        // otherwise the next
        // if this is not possible, the list will
        // be empty afterwards
        else if (next != NULL) {
          list_->setSelected(next, true);
        }
      }
      delete item;
    }
    else {
      pre = item;
    }
    item = next;
  }

  // set new last pointer
  // if we deleted the last element, it is
  // corrected, otherwise it points to the last element
  last_ = pre;
}

void
EventView::includeEvent(const QString&, const QString&)
{

}

ACE_Time_Value
EventView::eventTime(Q3ListViewItem * _item)
{
  ACE_Time_Value t = ACE_Time_Value::zero;
  if (_item != NULL) {
    std::istringstream istr((char const *)_item->text(0));
    istr >> t;
  }
  return t;
}
