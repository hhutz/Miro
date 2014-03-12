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

#include "ParameterListDialog.h"
#include "SimpleParameter.h"
#include "CompoundParameter.h"
#include "SingleParameterDialog.h"
#include "ParameterDialog.h"
#include "ConfigFile.h"

#include "miro/Exception.h"
#include "params/Generator.h"

#include <QGroupBox>
#include <QWidget>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#ifdef LSB_Q3LISTVIEW
#include <QTreeWidget>
#else
#include <q3listview.h>
#endif
#include <QMenu>

#include <cassert>

ParameterListDialog::ParameterListDialog(ParameterList::Type _type,
					 Miro::CFG::Parameter const& _parameter,
					 QDomNode const& _parentNode,
					 QDomNode const& _node,
					 ItemXML * _parentItem,
					 ItemXML * _item,
					 QWidget * _parent, const char * _name) :
  Super(_parentNode, _node, 
	_parentItem, _item,
	_parent, _name, TRUE),       // TRUE = modal dialog
  type_(_type),
  parameter_(_parameter),
  nestedCompound_(true),
  nestedType_(NULL),
  index_(0),
  list_(NULL),
  delButton_(NULL),
  editButton_(NULL),
  collector_()
{

  //----------------------------------------------------------------------------
  // create a copy of the parameters xml tree
  tmpParentNode_ = tmpDocument_.createElement("section");
  tmpDocument_.appendChild(tmpParentNode_);

  QDomElement e = tmpDocument_.createElement(ParameterXML::XML_TAG);
  e.setAttribute(ParameterXML::XML_ATTRIBUTE_KEY, name());
  tmpNode_ = e;
  if (!node_.isNull())
    tmpNode_ = node_.cloneNode();
  tmpParentNode_.appendChild(tmpNode_);


  //----------------------------------------------------------------------------
  // complete the dialog widgets

  // this is a widget of the parent constructor that we do not use
  delete frame_;

#ifdef LSB_Q3LISTVIEW
  list_ = new QTreeWidget();
  assert(list_ != NULL);
  list_->setColumnCount(2);
  QStringList headerLabels;
  headerLabels << "Position" << parameter_.type_;
  list_->setHeaderLabels(headerLabels);
  list_->setSortingEnabled(false);
  /// @todo What is the QTreeWidget counterpart of this?
  // list_->setResizeMode(QTreeWidget::AllColumns);
#else
  list_ = new Q3ListView(groupBox_, "list");
  list_->addColumn("Position");
  list_->addColumn(parameter_.type_);
  list_->setSorting(-1);
  list_->setResizeMode(Q3ListView::AllColumns);
#endif
  list_->setRootIsDecorated(true);
  assert(groupBox_ != NULL);
  assert(groupBox_->layout() != NULL);
  groupBox_->layout()->addWidget(list_);

  // Create the box to hold the buttons
  QWidget * const fileButtonsBox = new QWidget;
  assert(fileButtonsBox != NULL);

  // Create the button box's layout
  QHBoxLayout * fileButtonsBoxLayout = new QHBoxLayout;
  assert(fileButtonsBoxLayout != NULL);
  fileButtonsBox->setLayout(fileButtonsBoxLayout);

  // Create the Add button and add it to the layout
  const QString addButtonText("Add...");
  QPushButton * const addButton = new QPushButton(addButtonText);
  assert(addButton != NULL);
  fileButtonsBoxLayout->addWidget(addButton);

  // Create the Delete button and add it to the layout
  const QString delButtonText("Remove");
  delButton_ = new QPushButton(delButtonText);
  assert(delButton_ != NULL);
  fileButtonsBoxLayout->addWidget(delButton_);

  // Create the Edit button and add it to the layout
  const QString editButtonText("Edit");
  editButton_ = new QPushButton(editButtonText);
  assert(editButton_ != NULL);
  fileButtonsBoxLayout->addWidget(editButton_);

  assert(groupBox_ != NULL);
  assert(groupBox_->layout() != NULL);
  groupBox_->layout()->addWidget(fileButtonsBox);

  //----------------------------------------------------------------------------
  // add the list view items

  // get nested parameter type category
  if (SimpleParameter::typeFromName(parameter_.type_) !=
      SimpleParameter::NONE) {
    nestedCompound_ = false;
  }
  else {
    nestedType_ = 
      ConfigFile::instance()->description().getType(parameter_.type_);
    if (nestedType_ == NULL) {
      throw Miro::Exception(QString("Parameter description for " + 
				    parameter_.type_ +
				    " not found.\nCheck whether the relevant description file is loaded (1)."));
    }
  }
  
  // iterate over all members
  QDomNode n = tmpNode_.firstChild();
#ifdef LSB_Q3LISTVIEW
  QTreeWidgetItem * pre = NULL;
#else
  Q3ListViewItem * pre = NULL;
#endif
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull() &&
	e.tagName() == ParameterXML::XML_TAG) {
      
      ParameterXML * newParam = NULL;

      QString indexName;
      indexName.setNum(index_);

      if (nestedCompound_ == false) {

	if (!e.hasAttribute(SimpleParameter::XML_ATTRIBUTE_VALUE))
	  throw Miro::Exception(QString("Parameter tag without value in (" + 
					parameter_.type_ + ") " + name()));

	
	QString value = e.attribute(SimpleParameter::XML_ATTRIBUTE_VALUE);
	newParam = new SimpleParameter(parameter_,
				       n,
				       list_, pre,
				       &collector_, indexName);
      }
      else {
	newParam = new CompoundParameter(*nestedType_,
					 n,
					 list_, pre,
					 &collector_, indexName);
      }

      newParam->init();
      pre = newParam->treeWidgetItem();
      ++index_;
    }
    n = n.nextSibling();
  }


  //----------------------------------------------------------------------------
  // connect the dialogs functionality

  // enable/disable buttons
  selectListItem();
#ifdef LSB_Q3LISTVIEW
  connect(list_, 
	  SIGNAL(contextMenuRequested(QTreeWidgetItem *, const QPoint&, int)),
	  this,
	  SLOT(contextMenu(QTreeWidgetItem *, const QPoint&, int)));
  connect(list_, SIGNAL(doubleClicked(QTreeWidgetItem *)),
	  this, SLOT(slotDoubleClick(QTreeWidgettem *)));
#else
  connect(list_, 
	  SIGNAL(contextMenuRequested(Q3ListViewItem *, const QPoint&, int)),
	  this,
	  SLOT(contextMenu(Q3ListViewItem *, const QPoint&, int)));
  connect(list_, SIGNAL(doubleClicked(Q3ListViewItem *)),
	  this, SLOT(slotDoubleClick(Q3ListViewItem *)));
#endif
  connect(addButton, SIGNAL(clicked()), SLOT(add()));
  connect(delButton_, SIGNAL(clicked()), SLOT(del()));
  connect(editButton_, SIGNAL(clicked()), SLOT(edit()));

  groupBox_->sizeHint(); // we need this to get the right size (hutz)
  groupBox_->setTitle((type_ == ParameterList::SET)? "Set" : "Vector");
}

void
ParameterListDialog::setXML()
{
  // no edit -> nothing to be done
  if (!modified())
    return;

  // delete entry if edit field is empty
  if (tmpNode_.isNull() || 
      tmpNode_.firstChild().isNull()) {

    if (!node_.isNull()) {
      parentNode_.removeChild(node_);
      if (item_)
	item_->deleteLater();
    }
    else {
      setModified(false);
    }
    return;
  }  

  // crete a node if necessary
  if (node_.isNull()) {

    assert(!parentNode_.ownerDocument().isNull());
    QDomElement e = parentNode_.ownerDocument().createElement(ParameterXML::XML_TAG);

    e.setAttribute(ParameterXML::XML_ATTRIBUTE_KEY, name());
    node_ = parentNode_.appendChild(e);

    assert(!node_.isNull());
  }

  //--------------------------------------
  // replace node by new content

#ifdef LSB_Q3LISTVIEW
  // Fetch the QTreeWidgetItem corresponding to item_
  QTreeWidgetItem * const pThisTreeWidgetItem = item_->treeWidgetItem();
  // Fetch the parent of pThisTreeWidgetItem
  QTreeWidgetItem * const pParentTreeWidgetItem = pThisTreeWidgetItem->parent();
  // The pTreeWidgetItem that precedes pThisTreeWidgetItem, if it exists
  QTreeWidgetItem * pre = NULL;
  if (pParentTreeWidgetItem)
  {
    // Find the index of pThisTreeWidgetItem wrt pParentTreeWidgetItem
    const int thisIndex =
      pParentTreeWidgetItem->indexOfChild(pThisTreeWidgetItem);
    if ((1 <= thisIndex) && (thisIndex < pParentTreeWidgetItem->childCount()))
    {
      const int predecessorIndex = thisIndex - 1;
      pre = pParentTreeWidgetItem->child(predecessorIndex);
    }
  }
#else
  // remember the predecessor
  Q3ListViewItem * pre = NULL;
  if (item_) {
    Q3ListViewItem * parent = item_->treeWidgetItem()->parent();
    if (parent != NULL) {
      pre = parent->firstChild();
      while (pre != NULL) {
	if (pre->nextSibling() == item_->treeWidgetItem())
	  break;
	pre = pre->nextSibling();
      }
    }
  }
#endif

  // delete the current content
  if (item_)
    item_->deleteLater();

  // replace the xml subtree
  QDomNode node = tmpNode_.cloneNode();
  QDomNode oldNode = node_.parentNode().replaceChild(node, node_);
  node_ = node;

  assert(!oldNode.isNull());

  // reconstruct the listview if available
  if (parentItem_) {
    // generate original parameter
    Miro::CFG::Parameter parameter;

    assert(type_ != ParameterList::NONE);
    if (type_ == ParameterList::VECTOR) {
      parameter.type_ = "std::vector<" + parameter_.type_ + ">";
    }
    else if (type_ == ParameterList::SET) {
      parameter.type_ = "std::set<" + parameter_.type_ + ">";
    }

    parameter.name_ = parameter_.name_;
    parameter.default_ = "";
    parameter.fullDefault_ = "";
    parameter.measure_ = parameter_.measure_;
    parameter.description_ = parameter_.description_;

    QString p = node.toElement().attribute(ParameterXML::XML_ATTRIBUTE_KEY);
    item_ = new ParameterList(parameter,
			      node,
			      parentItem_->treeWidgetItem(), pre,
			      parentItem_, p);
    dynamic_cast<ParameterXML *>(item_)->init();
  }
}

void
ParameterListDialog::selectListItem()
{
#ifdef LSB_Q3LISTVIEW
  // Precondition
  assert(list_ != NULL);
  // If list_ has no children
  QTreeWidgetItem * const pCurrentTreeWidgetItem = list_->currentItem();
  const bool currentItemExists = (pCurrentTreeWidgetItem != NULL);
  if (currentItemExists) {
    // Select the current QTreeWidgetItem
    pCurrentTreeWidgetItem->setSelected(true);
  }
  
  delButton_->setEnabled(currentItemExists);
  editButton_->setEnabled(currentItemExists);
#else
  if (list_->childCount() != 0) {
    Q3ListViewItem * item = list_->currentItem();
    list_->setSelected(item, true);
  }

  delButton_->setEnabled(list_->currentItem() != NULL);
  editButton_->setEnabled(list_->currentItem() != NULL);
#endif

}

void 
ParameterListDialog::add()
{
  ParameterXML * newParam = NULL;
#ifdef LSB_Q3LISTVIEW
  // Precondition
  assert(list_ != NULL);

  const QList<QTreeWidgetItem*> selectedItems = list_->selectedItems();
  // The first selected item, if any
  QTreeWidgetItem * pre = selectedItems.isEmpty() ? NULL : selectedItems[0];
#else
  Q3ListViewItem * pre = list_->selectedItem();
#endif
  ItemXML * preItem = NULL;

  if (pre != NULL) {
    while (pre->parent()) {
      pre = pre->parent();
    }

    Item::ItemMap::const_iterator i = Item::itemMap().find(pre);
    if (i != Item::itemMap().end())
      preItem = dynamic_cast<ItemXML * >(i->second);
  }

  //----------------------------------------------------------------------------
  // insert element into document
  // it will be deleted afterwards, if nothing was entered.
  QDomElement e = tmpDocument_.createElement(ParameterXML::XML_TAG);

  // determine where to put it
  QDomNode n = tmpNode_.firstChild();
  if (preItem != NULL) { // is no direct child of item_->node()
    n = preItem->node();
    assert(!n.isNull());
  }

  QDomNode newChild = tmpNode_.insertAfter(e, n);

  assert(!e.isNull());
  assert(!newChild.isNull());

  //----------------------------------------------------------------------------
  // set parameter value

  DialogXML * dialog = NULL;

  if (!nestedCompound_) {
    dialog = new SingleParameterDialog(parameter_,
				       newChild,
				       item_, NULL, NULL, name());
  }
  else {
    dialog = new ParameterDialog(*nestedType_,
				 newChild.parentNode(), newChild, 
				 item_, NULL, NULL, name());
  }
  int rc = dialog->exec();
  if (rc == QDialog::Accepted) {
    dialog->setXML();
    if (dialog->modified())
      setModified(true);
  }
  delete dialog;

  e = newChild.toElement();
  assert(!e.isNull());

  // if nothing is entered, remove the node
  // (that is: no value tag and no childs)
  if (!e.hasAttribute(SimpleParameter::XML_ATTRIBUTE_VALUE) &&
      e.firstChild().isNull()) {
    tmpNode_.removeChild(newChild);
    return;
  }


  //----------------------------------------------------------------------------
  //create listview item

  QString indexName;
  indexName.setNum(index_);

  if (!nestedCompound_) {
    QString value = e.attribute(SimpleParameter::XML_ATTRIBUTE_VALUE);
    newParam = new SimpleParameter(parameter_,
				   newChild, 
				   list_, pre,
				   &collector_, indexName);
  }
  else {
    newParam = new CompoundParameter(*nestedType_, 
				     newChild,
				     list_, pre,
				     &collector_, indexName);
  }

  newParam->init();
  ++index_;

  setModified(true);
#ifdef LSB_Q3LISTVIEW
  assert(newParam != NULL);
  QTreeWidgetItem * const pTreeWidgetItem = newParam->treeWidgetItem();
  assert(pTreeWidgetItem != NULL);
  pTreeWidgetItem->setSelected(true);
#else
  list_->setSelected(newParam->treeWidgetItem(), true);
#endif
  selectListItem();
  renumberEntries();
}

void 
ParameterListDialog::del()
{
#ifdef LSB_Q3LISTVIEW
  QTreeWidgetItem * item; 
#else
  Q3ListViewItem * item; 
#endif
  if ((item = list_->currentItem()) != NULL) {
    Item::ItemMap::const_iterator i = Item::itemMap().find(item);
    assert(i != Item::itemMap().end());


    ItemXML * ix = dynamic_cast<ItemXML * > (i->second);
    ix->deleteItem();
    setModified(true);
  }
 
  selectListItem();
}

void
ParameterListDialog::edit()
{
#ifdef LSB_Q3LISTVIEW
  QTreeWidgetItem * item = list_->currentItem();
#else
  Q3ListViewItem * item = list_->currentItem();
#endif
  if (item != NULL) {
    Item::ItemMap::const_iterator i = Item::itemMap().find(item);
    assert(i != Item::itemMap().end());

    ParameterXML * p = dynamic_cast<ParameterXML *>(i->second);
    assert(p != NULL);

    p->setParameters();

    if (p->modified())
      setModified(true);
  }
}

void
ParameterListDialog::up()
{}

void
ParameterListDialog::down()
{}


void
#ifdef LSB_Q3LISTVIEW
ParameterListDialog::slotDoubleClick(QTreeWidgetItem * _item)
#else
ParameterListDialog::slotDoubleClick(Q3ListViewItem * _item)
#endif
{
  Item::ItemMap::const_iterator i = Item::itemMap().find(_item);
  assert(i != Item::itemMap().end());

  ParameterXML * p = dynamic_cast<ParameterXML *>(i->second);
  assert(p != NULL);

  p->setParameters();
}

void
#ifdef LSB_Q3LISTVIEW
ParameterListDialog::contextMenu(QTreeWidgetItem * _item, const QPoint& pos, int)
#else
ParameterListDialog::contextMenu(Q3ListViewItem * _item, const QPoint& pos, int)
#endif
{
  Item::ItemMap::const_iterator i = Item::itemMap().find(_item);
  assert(i != Item::itemMap().end());

  QMenu menu(NULL);
#ifdef LSB_Q3LISTVIEW
  const std::pair<QTreeWidgetItem*, Item*>& p = *i;
#else
  const std::pair<Q3ListViewItem*, Item*>& p = *i;
#endif
  Item * const pItem = p.second;
  assert(pItem != 0);
  // Populate this context menu based on the Item
  pItem->contextMenu(menu);

  menu.exec(pos);
}

void
ParameterListDialog::renumberEntries() 
{
#ifdef LSB_Q3LISTVIEW
  // Precondition
  assert(list_ != NULL);

  for (int i = 0; i < list_->invisibleRootItem()->childCount(); ++i)
  {
    QTreeWidgetItem * const pTreeWidgetItem =
      list_->invisibleRootItem()->child(i);
    assert(pTreeWidgetItem != 0);
    QString number;
    number.setNum(i);
    pTreeWidgetItem->setText(0, number);
  }
#else
  int counter = 0;
  QString number;
  Q3ListViewItem * item = list_->firstChild();
  while (item != NULL) {
    number.setNum(counter);
    item->setText(0, number);

    item = item->nextSibling();
    ++counter;
  }
#endif
}
