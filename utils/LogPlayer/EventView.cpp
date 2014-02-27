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
#define LSB_Q3VBOXLAYOUT
#define LSB_Q3VGROUPBOX

#include "EventView.h"
#include "FileSet.h"

#include "miro/Log.h"
#include "miro/TimeHelper.h"

#include <orbsvcs/CosNotifyChannelAdminC.h>

#ifdef LSB_Q3LISTVIEW
#include <QTreeWidget>
#else
#include <q3listview.h>
#endif
#include <qlayout.h>
#ifdef LSB_Q3VGROUPBOX
#include <Q3GroupBox>
#else
#include <q3vgroupbox.h>
#endif
//Added by qt3to4:
#include <QHideEvent>
#ifdef LSB_Q3VBOXLAYOUT
#include <QVBoxLayout>
#else
#include <Q3VBoxLayout>
#endif

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

#ifdef LSB_Q3VBOXLAYOUT
  // Create the Group
  QWidget * const pTopBoxParent = this;
  QVBoxLayout * topBox = new QVBoxLayout(pTopBoxParent);
  assert(topBox != NULL);
  const int margin = 0;
  topBox->setContentsMargins(margin, margin, margin, margin);
  const int spacing = -1;
  topBox->setSpacing(spacing);
#else
  Q3VBoxLayout * topBox = new Q3VBoxLayout(this, 0, -1, "boxLayout");
#endif // LSB_Q3VBOXLAYOUT

#ifdef LSB_Q3VGROUPBOX
  // Create the Group Box
  const QString groupBoxTitle("Events");
  QWidget * const pGroupBoxParent = this;
  QGroupBox * eventBox = new QGroupBox(groupBoxTitle, pGroupBoxParent);
  assert(eventBox != NULL);

  // Create the Vertical Box Layout
  QVBoxLayout * const pGroupBoxLayout = new QVBoxLayout;
  assert(pGroupBoxLayout != NULL);

  // Assign the Vertical Box Layout to the Group Box
  eventBox->setLayout(pGroupBoxLayout);

  // Create the List View
#ifdef LSB_Q3LISTVIEW
  list_ = new QTreeWidget();
#else
  list_ = new Q3ListView();
#endif // LSB_Q3LISTVIEW

  // Add the ListView to the Group Box's Vertical Box Layout
  pGroupBoxLayout->addWidget(list_);
#else // LSB_Q3VGROUPBOX
  Q3VGroupBox * eventBox = new Q3VGroupBox(this, "eventBox");
#ifdef LSB_Q3LISTVIEW
  list_ = new QTreeWidget(eventBox, "list");
  assert(list_ != NULL);
  const int columnCount = 4;
  list_->setColumnCount(columnCount);
  QStringList headerLabels;
  headerLabels << "Time Stamp" << "Domain Name" << "Type Name" << "Event Name";
  list_->setHeaderLabels(headerLabels);
  list_->setSortingEnabled(true);
#else
  list_ = new Q3ListView(eventBox, "list");
  list_->addColumn("Time Stamp");
  list_->addColumn("Domain Name");
  list_->addColumn("Type Name");
  list_->addColumn("Event Name");
  list_->setSorting(-1);
#endif // LSB_Q3LISTVIEW
  list_->setAllColumnsShowFocus(true);
#endif // LSB_Q3VGROUPBOX

#ifdef LSB_Q3VGROUPBOX
  // Title set in constructor
  topBox->addSpacing(10);
  topBox->addWidget(eventBox);
#else
  eventBox->setTitle("Events");
  topBox->addSpacing(10);
  topBox->addWidget(eventBox);
#endif

  // connect to file set events
  FileSet::FileVector::iterator first, last = fileSet_->fileVector().end();
  for (first = fileSet_->fileVector().begin(); first != last; ++first) {
    connect(*first, SIGNAL(newEvent(const QString&,const QString&,const QString&,const QString&)),
            this, SLOT(insertEvent(const QString&,const QString&,const QString&,const QString&)));
  }

#ifdef LSB_Q3LISTVIEWITEM
  connect(list_, SIGNAL(selectionChanged(QTreeWidgetItem *)),
          this, SLOT(selectionChanged(QTreeWidgetItem *)));
#else
  connect(list_, SIGNAL(selectionChanged(Q3ListViewItem *)),
          this, SLOT(selectionChanged(Q3ListViewItem *)));
#endif

  fileSet_->getEvents(fileSet_->coursorTime(), history_);

  show();
}

EventView::~EventView()
{
}

#ifdef LSB_Q3LISTVIEW
void
EventView::insertEvent(QString const& _stamp,
                       QString const& _domain,
		       QString const& _type,
		       QString const& _event)
{
  // A new item will be created, and it will become the selected item
  internalSetSelection_ = true;

  // Precondition
  assert(list_ != NULL);

  // The selected QTreeWidget item(s), if any
  const QList<QTreeWidgetItem*> selectedItems = list_->selectedItems();
  // The root QTreeWidget
  QTreeWidgetItem * const pRoot = list_->invisibleRootItem();
  assert(pRoot != NULL);
  // The number of items at the top level
  const int childCount = pRoot->childCount();
  // The first item at the top level, if any
  QTreeWidgetItem * const pFirstTreeWidgetItem =
    (childCount > 0) ? pRoot->child(0) : NULL;
  // If no item is selected and there is a top-level item, select it
  if (selectedItems.isEmpty() && (pFirstTreeWidgetItem != NULL))
  {
    pFirstTreeWidgetItem->setSelected(true);
  }

  // Case 1 of 3: insert at the beginning
  if (
      // The QTreeWidget has no children
      (pFirstTreeWidgetItem == NULL) ||
      // The time stamp is less than that of the earliest child item
      (_stamp < pRoot->child(0)->text(0)))
  {
    // Create the new item
    QTreeWidget * const pParent = list_;
    QStringList strings;
    strings << _stamp << _domain << _type << _event;
    QTreeWidgetItem * const pNewTreeWidgetItem = 
      new QTreeWidgetItem(pParent, strings);
    // Insert the new item at the beginning of the list
    list_->insertTopLevelItem(0, pNewTreeWidgetItem);
    // Select it
    pNewTreeWidgetItem->setSelected(true);
    // Make it visible
    list_->scrollToItem(pNewTreeWidgetItem);

    if (last_ == NULL)
    {
      // The first one now shall later be last for the times they are a-changin
      last_ = pNewTreeWidgetItem;
    }
  }

  // Case 2 of 3: Insert at the end (which is distinct from the beginning)
  // The time stamp is greater than that of the lateest item, which must exist
  else if (last_->text(0) < _stamp)
  {
    // Create the new item
    QTreeWidget * const pParent = list_;
    QStringList strings;
    strings << _stamp << _domain << _type << _event;
    QTreeWidgetItem * const pNewTreeWidgetItem =
      new QTreeWidgetItem(pParent, strings);
    // Insert the new item at the end of the list
    list_->addTopLevelItem(pNewTreeWidgetItem);
    last_ = pNewTreeWidgetItem;
    // Select it
    pNewTreeWidgetItem->setSelected(true);
    // Make it visible
    list_->scrollToItem(pNewTreeWidgetItem);
  }

  // Case 3 of 3: Insert relative to the selected item
  else if (
	   // An item is selected
	   !list_->selectedItems().isEmpty() &&
	   // The (first) selected item does not have the same time stamp
           (list_->selectedItems()[0]->text(0) != _stamp))
  {
    // Fetch the (first) selected item
    QTreeWidgetItem * const pSelectedItem = list_->selectedItems()[0];
    MIRO_ASSERT(pSelectedItem != NULL);
    if (_stamp > pSelectedItem->text(0))
    {
      // Case 3a: Insert the new item after the (first) selected item
      // Find the first item after the selected one later than the time stamp
      int index = list_->indexOfTopLevelItem(pSelectedItem);
      while (_stamp > pRoot->child(index)->text(0))
      {
	index++;
      }
      QTreeWidgetItem * const pSuccessor = pRoot->child(index);
      assert(pSuccessor != NULL);
      assert(_stamp <= pSuccessor->text(0));
      assert(index > 0);

      if (_stamp != pSuccessor->text(0))
      {
	// Create the new item
	QTreeWidget * const pParent = list_;
	QStringList strings;
	strings << _stamp << _domain << _type << _event;
        QTreeWidgetItem * const pNewItem =
	  new QTreeWidgetItem(pParent, strings);
	assert(pNewItem != NULL);
	// Insert it
	pRoot->insertChild(index - 1, pNewItem);
	// Select it
	pNewItem->setSelected(true);
      }
    }
    else
    {
      // Case 3b: Insert the new item before the (first) selected item
      // Find the first item before the selected one later than the time stamp
      int index = 0;
      while (_stamp < pRoot->child(index)->text(0))
      {
	index++;
      }
      QTreeWidgetItem * const pPredecessor = pRoot->child(index);
      assert(pPredecessor != NULL);
      assert(_stamp >= pPredecessor->text(0));

      if (_stamp != pPredecessor->text(0))
      {
	// Create the new item
	QTreeWidget * const pParent = list_;
	QStringList strings;
	strings << _stamp << _domain << _type << _event;
        QTreeWidgetItem * const pNewItem =
	  new QTreeWidgetItem(pParent, strings);
	assert(pNewItem != NULL);
	// Insert it
	pRoot->insertChild(index, pNewItem);
	// Select it
	pNewItem->setSelected(true);
      }
    }
  }

  pruneHistory();
  internalSetSelection_ = false;
}

#else

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

  // insert it at the end
  else if (last_->text(0) < _stamp) {
    last_ = new Q3ListViewItem(list_, last_,
                               _stamp, _domain, _type, _event);
    list_->setSelected(last_, true);
    list_->ensureItemVisible(last_);
  }

  // insert somewhere in between
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
#endif

void
EventView::setHistory(unsigned int _size)
{
  MIRO_ASSERT(_size > 0);
  if (_size > history_) {
    history_ = _size;
#ifdef LSB_Q3LISTVIEW
    const QList<QTreeWidgetItem*> selectedItems = list_->selectedItems();
    assert(!selectedItems.isEmpty());
    QTreeWidgetItem * const pSelectedTreeWidgetItem = selectedItems[0];
    fileSet_->getEvents(eventTime(pSelectedTreeWidgetItem), history_);
#else
    fileSet_->getEvents(eventTime(list_->selectedItem()), history_);
#endif
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
#ifdef LSB_Q3LISTVIEW
  // Precondition
  assert(list_ != NULL);

  QTreeWidgetItem * const pParentTreeWidgetItem = list_->invisibleRootItem();
  assert(pParentTreeWidgetItem != NULL);
  // The maximum number of QTreeWidgetItems to remove
  const int maxRemovals = pParentTreeWidgetItem->childCount() - (int)history_;
  for (int i = 0; i < maxRemovals; i++)
  {
    // The first remaining child
    QTreeWidgetItem * const pChildTreeWidgetItem =
      pParentTreeWidgetItem->child(0);
    assert(pChildTreeWidgetItem != NULL);
    if (pChildTreeWidgetItem->isSelected())
    {
      // This is where we stop pruning
      break;
    } else {
      pParentTreeWidgetItem->removeChild(pChildTreeWidgetItem);
      delete pChildTreeWidgetItem;
    }
  }
  if (pParentTreeWidgetItem->childCount() == 0)
  {
    // Everything has been pruned, so there is no last item
    last_ = NULL;
  }
#else
  // chop at the beginning till selected event
  int overlap = list_->childCount() - (int)history_;
  while (overlap > 0 &&
              list_->firstChild() != list_->selectedItem()) {
    if (list_->firstChild() == last_)
      last_ = NULL;
    delete list_->firstChild();
    --overlap;
  }
#endif
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
#ifdef LSB_Q3LISTVIEWITEM
EventView::selectionChanged(QTreeWidgetItem * _item)
#else
EventView::selectionChanged(Q3ListViewItem * _item)
#endif
{
  if (!internalSetSelection_) {
    ACE_Time_Value t = eventTime(_item);
    fileSet_->coursorTimeRel(t);
  }
}

void
EventView::excludeEvent(const QString& _domainName, const QString& _typeName)
{
#ifdef LSB_Q3LISTVIEWITEM
  // Precondition
  assert(list_ != NULL);

  QTreeWidgetItem * const pParentTreeWidgetItem = list_->invisibleRootItem();
  assert(pParentTreeWidgetItem != NULL);
  const int childCount = pParentTreeWidgetItem->childCount();
  for (int i = 0; i < childCount; ++i)
  {
    QTreeWidgetItem * const pChildTreeWidgetItem =
      pParentTreeWidgetItem->child(i);
    assert(pChildTreeWidgetItem != NULL);
    if ((pChildTreeWidgetItem->text(1) == _domainName) &&
	(pChildTreeWidgetItem->text(2) == _typeName))
    {
      // It is to be deleted
      if (pChildTreeWidgetItem->isSelected())
      {
	// Move the selection to a neighbor if there is one
	if (i > 1)
        {
	  QTreeWidgetItem * const pPredecessorTreeWidgetItem =
	    pParentTreeWidgetItem->child(i - 1);
	  assert(pPredecessorTreeWidgetItem != NULL);
	  pPredecessorTreeWidgetItem->setSelected(true);
	}
	else if (i < childCount - 1)
        {
	  QTreeWidgetItem * const pSuccessorTreeWidgetItem =
	    pParentTreeWidgetItem->child(i - 1);
	  assert(pSuccessorTreeWidgetItem != NULL);
	  pSuccessorTreeWidgetItem->setSelected(true);
	}
	// Remove the QTreeWidgetItem from the QTreeWidget
	pParentTreeWidgetItem->removeChild(pChildTreeWidgetItem);
	delete pChildTreeWidgetItem;
      }
    }
  }
  const int newChildCount = pParentTreeWidgetItem->childCount();
  last_ = 
    (newChildCount == 0) ?
    NULL :
    pParentTreeWidgetItem->child(newChildCount - 1);
#else
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
#endif
}

void
EventView::includeEvent(const QString&, const QString&)
{

}

ACE_Time_Value
#ifdef LSB_Q3LISTVIEWITEM
EventView::eventTime(QTreeWidgetItem * _item)
#else
EventView::eventTime(Q3ListViewItem * _item)
#endif
{
  ACE_Time_Value t = ACE_Time_Value::zero;
  if (_item != NULL) {
    std::istringstream istr((char const *)_item->text(0));
    istr >> t;
  }
  return t;
}
