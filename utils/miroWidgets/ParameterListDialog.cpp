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
#include "ParameterListDialog.h"
#include "SimpleParameter.h"
#include "CompoundParameter.h"
#include "SingleParameterDialog.h"
#include "ParameterDialog.h"
#include "ConfigFile.h"

#include "miro/Exception.h"
#include "params/Generator.h"

#include <q3groupbox.h>
#include <q3vgroupbox.h>
#include <q3hgroupbox.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <q3listview.h>
#include <q3popupmenu.h>

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

  list_ = new Q3ListView(groupBox_, "list");
  list_->addColumn("Position");
  list_->addColumn(parameter_.type_);
  list_->setSorting(-1);
  list_->setResizeMode(Q3ListView::AllColumns);
  list_->setRootIsDecorated(true);

  Q3HBox * fileButtonsBox = new Q3HBox(groupBox_, "fileButtons");
  QPushButton * addButton = new QPushButton("Add...", fileButtonsBox);
  delButton_ = new QPushButton("Delete", fileButtonsBox);
  editButton_ = new QPushButton("Edit", fileButtonsBox);


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
  Q3ListViewItem * pre = NULL;
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
      pre = newParam->listViewItem();
      ++index_;
    }
    n = n.nextSibling();
  }


  //----------------------------------------------------------------------------
  // connect the dialogs functionality

  // enable/disable buttons
  selectListItem();

  connect(list_, 
	  SIGNAL(contextMenuRequested(Q3ListViewItem *, const QPoint&, int)),
	  this,
	  SLOT(contextMenu(Q3ListViewItem *, const QPoint&, int)));
  connect(list_, SIGNAL(doubleClicked(Q3ListViewItem *)),
	  this, SLOT(slotDoubleClick(Q3ListViewItem *)));
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

  // remember the predecessor
  Q3ListViewItem * pre = NULL;
  if (item_) {
    Q3ListViewItem * parent = item_->listViewItem()->parent();
    if (parent != NULL) {
      pre = parent->firstChild();
      while (pre != NULL) {
	if (pre->nextSibling() == item_->listViewItem())
	  break;
	pre = pre->nextSibling();
      }
    }
  }

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
			      parentItem_->listViewItem(), pre,
			      parentItem_, p);
    dynamic_cast<ParameterXML *>(item_)->init();
  }
}

void
ParameterListDialog::selectListItem()
{
  if (list_->childCount() != 0) {
    Q3ListViewItem * item = list_->currentItem();
    list_->setSelected(item, true);
  }

  delButton_->setEnabled(list_->currentItem() != NULL);
  editButton_->setEnabled(list_->currentItem() != NULL);
}

void 
ParameterListDialog::add()
{
  ParameterXML * newParam = NULL;
  Q3ListViewItem * pre = list_->selectedItem();
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
  // it will be deleted afterwards, if nothing was enteres.
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
  list_->setSelected(newParam->listViewItem(), true);
  selectListItem();
  renumberEntries();
}

void 
ParameterListDialog::del()
{
  Q3ListViewItem * item; 
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
  Q3ListViewItem * item = list_->currentItem();
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
ParameterListDialog::slotDoubleClick(Q3ListViewItem * _item)
{
  Item::ItemMap::const_iterator i = Item::itemMap().find(_item);
  assert(i != Item::itemMap().end());

  ParameterXML * p = dynamic_cast<ParameterXML *>(i->second);
  assert(p != NULL);

  p->setParameters();
}

void
ParameterListDialog::contextMenu(Q3ListViewItem * _item, const QPoint& pos, int)
{
  Item::ItemMap::const_iterator i = Item::itemMap().find(_item);
  assert(i != Item::itemMap().end());

  Q3PopupMenu menu(NULL, "plistpopu");
  i->second->contextMenu(menu);

  menu.exec(pos);
}

void
ParameterListDialog::renumberEntries() 
{
  int counter = 0;
  QString number;
  Q3ListViewItem * item = list_->firstChild();
  while (item != NULL) {
    number.setNum(counter);
    item->setText(0, number);

    item = item->nextSibling();
    ++counter;
  }
}
