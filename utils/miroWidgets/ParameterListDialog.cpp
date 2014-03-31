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

// This module
#include "ParameterListDialog.h"
// This application
#include "SimpleParameter.h"
#include "CompoundParameter.h"
#include "SingleParameterDialog.h"
#include "ParameterDialog.h"
#include "ConfigFile.h"
#include "miro/Exception.h"
#include "params/Generator.h"
// The Qt library
#include <QGroupBox>
#include <QWidget>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <QTreeWidget>
#include <QMenu>
// The C++ Standard Library
#include <cassert>

ParameterListDialog::ParameterListDialog(ParameterList::Type _type,
					 Miro::CFG::Parameter const& _parameter,
					 QDomNode const& _parentNode,
					 QDomNode const& _node,
					 ItemXML * _parentItem,
					 ItemXML * _item,
					 QWidget * _parent,
					 const char * _name) :
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
  // create a copy of the parameter's xml tree
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

  list_ = new QTreeWidget();
  assert(list_ != NULL);

  // Column 0 is the serial enumerator and column 1 is the value
  list_->setColumnCount(2);
  QStringList headerLabels;
  headerLabels << "Position" << parameter_.type_;
  list_->setHeaderLabels(headerLabels);

  // Always sort in order of the serial enumerator
  list_->setSortingEnabled(false);

  /// @todo What is the QTreeWidget counterpart of this?
  // list_->setResizeMode(QTreeWidget::AllColumns);
  list_->setRootIsDecorated(true);

  // Add the QTreeWidget to the QGroupBox's Layout
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
  // add the QTreeWidgetItems

  // Find if the ParameterList elements are SimpleParameter or CompountParameter
  // If CompoundParameter, store the parameter type in
  // ParameterListDialog::nestedType_
  if (SimpleParameter::typeFromName(parameter_.type_) !=
      SimpleParameter::NONE) {
    // It is a SimpleParameter
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
    // It is a CompoundParameter
  }
  
  // For each element of the ParameterList
  // The first element of the ParameterList
  QDomNode n = tmpNode_.firstChild();
  // The predecessor of the QTreeWidgetItem to be created; NULL for the first
  QTreeWidgetItem * pre = NULL;
  while (!n.isNull()) {
    // Create a ParameterXML and a QTreeWidgetItem for this DOM tree node
    QDomElement e = n.toElement();
    if (!e.isNull() &&
	e.tagName() == ParameterXML::XML_TAG) {
      // The DOM tree node is for a <parameter> element
      
      // The SimpleParameter or CompoundParameter to be created
      ParameterXML * newParam = NULL;

      // The name of the Item is its serial enumerator
      QString indexName;
      indexName.setNum(index_);

      if (nestedCompound_ == false) {
	// It is a SimpleParameter
	// The <parameter> tag must have a "value" attribute
	if (!e.hasAttribute(SimpleParameter::XML_ATTRIBUTE_VALUE))
	  throw Miro::Exception(QString("Parameter tag without value in (" + 
					parameter_.type_ + ") " + name()));

	/// @todo This variable is not used; remove it
	QString value = e.attribute(SimpleParameter::XML_ATTRIBUTE_VALUE);
	newParam = new SimpleParameter(parameter_,     // Parameter description
				       n,              // DOM tree node
				       list_,          // QTreeWidget
				       pre,            // QTreeWidgetItem
				       &collector_,    // QObject
				       indexName);     // Serial enumerator name
      }
      else {
	// It is a CompoundParameter
	newParam = new CompoundParameter(*nestedType_, // Paameter description
					 n,            // DOM tree node
					 list_,        // QTreeWidget
					 pre,          // QTreeWidgetItem
					 &collector_,  // QObject
					 indexName);   // Serial enumerator name
      }

      // Initialize the ParameterXML (necessary if CompoundParameter)
      newParam->init();
      // The just-created QTreeWidgetItem becomes predecessor of the next one
      pre = newParam->treeWidgetItem();
      // Increment the serial enumerator
      ++index_;
    }
    // Advance to the next DOM tree node
    n = n.nextSibling();
  }


  //----------------------------------------------------------------------------
  // connect the dialogs functionality

  // enable/disable buttons
  selectListItem();
  connect(list_, 
	  SIGNAL(customContextMenuRequested(const QPoint&)),
	  this,
	  SLOT(contextMenu(const QPoint&)));
  connect(list_,
	  SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
	  this,
	  SLOT(slotDoubleClick(QTreeWidgetItem *, int)));
  connect(addButton, SIGNAL(clicked()), SLOT(add()));
  connect(delButton_, SIGNAL(clicked()), SLOT(del()));
  connect(editButton_, SIGNAL(clicked()), SLOT(edit()));

  groupBox_->sizeHint(); // we need this to get the right size (hutz)
  groupBox_->setTitle((type_ == ParameterList::SET)? "Set" : "Vector");
}

void
ParameterListDialog::setXML()
{
  // Precondition
  assert(item_ != 0);

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
    // Store as the derived class type to avoid having to downcast
    ParameterList * const pParameterList =
      new ParameterList(parameter,
			      node,
			      parentItem_->treeWidgetItem(), pre,
			      parentItem_, p);
    assert(pParameterList != 0);
    // Store the ParameterList* as an ItemXML* in DialogXML::item_
    item_ = pParameterList;
    assert(item_ != 0);
    pParameterList->init();
  }
}

void
ParameterListDialog::selectListItem()
{
  // Precondition
  assert(list_ != NULL);

  bool currentItemExists = false;
  // If QTreeWidget list_ has children
  if (list_->topLevelItemCount() > 0)
  {
    // The current QTreeWidgetItem
    QTreeWidgetItem * const pCurrentTreeWidgetItem = list_->currentItem();
    if (pCurrentTreeWidgetItem != 0) {
      currentItemExists = true;
      // Select the current QTreeWidgetItem
      pCurrentTreeWidgetItem->setSelected(true);
    }
  }
  
  delButton_->setEnabled(currentItemExists);
  editButton_->setEnabled(currentItemExists);
}

void 
ParameterListDialog::add()
{
  // Precondition
  assert(list_ != NULL);
  ParameterXML * newParam = NULL;

  // All the QTreeWidgetItems selected when the "Add..." button was clicked
  const QList<QTreeWidgetItem*> selectedItems = list_->selectedItems();
  // The first selected Item, if any
  // The new Item will be added at the selected location
  QTreeWidgetItem * pre = selectedItems.isEmpty() ? NULL : selectedItems[0];

  // The ItemXML the corresponds to the selected QTreeWidgetItem
  ItemXML * preItem = NULL;

  if (pre != NULL) {
    // A QTreeWidgetItem was selected. Replace it with its root ancestor.
    while (pre->parent()) {
      pre = pre->parent();
    }
    assert(pre != NULL);

    // Look up the ItemXML that is mapped to the selected QTreeWidgetItem's root
    Item::ItemMap::const_iterator i = Item::itemMap().find(pre);
    if (i != Item::itemMap().end())
    {
      preItem = dynamic_cast<ItemXML * >(i->second);
      // If the QTreeWidgetItem* is in the map, it must be an ItemXML*
      assert(preItem != 0);
    }
  }

  //----------------------------------------------------------------------------
  // Insert a <parameter> element into the document's DOM tree
  // It will be deleted afterwards if nothing was entered (cancel operation)
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
    // DialogXML::item_ must have been set before this can be called correctly
    assert(item_ != 0);
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
  // Create the QTreeWidgetItem for this new ItemXML

  // The name of the Item is its serial enumerator
  QString indexName;
  indexName.setNum(index_);

  if (!nestedCompound_) {
    // The Item is a SimpleParameter
    /// @todo This variable is not used; remove it
    const QString value = e.attribute(SimpleParameter::XML_ATTRIBUTE_VALUE);
    newParam = new SimpleParameter(parameter_,     // Parameter description
				   newChild,       // DOM tree node
				   list_,          // QTreeWidget
				   pre,            // QTreeWidgetItem
				   &collector_,    // QObject
				   indexName);     // Serial enumerator name
  }
  else {
    // The Item is a CompoundParameter
    newParam = new CompoundParameter(*nestedType_, // Parameter description
				     newChild,     // DOM tree node
				     list_,        // QTreeWidget
				     pre,          // QTreeWidgetItem
				     &collector_,  // QObject
				     indexName);   // Serial enumerator name
  }
  assert(newParam != NULL);

  newParam->init();
  ++index_;

  setModified(true);

  QTreeWidgetItem * const pTreeWidgetItem = newParam->treeWidgetItem();
  assert(pTreeWidgetItem != NULL);

  pTreeWidgetItem->setSelected(true);
  selectListItem();
  renumberEntries();
}

void 
ParameterListDialog::del()
{
  QTreeWidgetItem * item; 
  if ((item = list_->currentItem()) != NULL) {
    // There is a currently-selected item.
    // Look up the Item using the QTreeWidgetItem as key
    Item::ItemMap::const_iterator i = Item::itemMap().find(item);
    // The Item must be found
    assert(i != Item::itemMap().end());

    // 
    ItemXML * const ix = dynamic_cast<ItemXML * > (i->second);
    // It must be an ItemXML
    assert(ix != 0);
    //
    ix->deleteItem();
    setModified(true);
  }
 
  selectListItem();
}

void
ParameterListDialog::edit()
{
  QTreeWidgetItem * item = list_->currentItem();
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
ParameterListDialog::slotDoubleClick(QTreeWidgetItem * _item, int)
{
  Item::ItemMap::const_iterator i = Item::itemMap().find(_item);
  assert(i != Item::itemMap().end());

  ParameterXML * p = dynamic_cast<ParameterXML *>(i->second);
  assert(p != NULL);

  p->setParameters();
}

void
ParameterListDialog::contextMenu(const QPoint& pos)
{
  // Precondition
  assert(list_ != NULL);

  // Map the position onto the selected QTreeWidgetItem
  QTreeWidgetItem * const _item = list_->itemAt(pos);
  assert(_item != 0);

  // Map the selected QTreeWidgetItem onto its Item
  Item::ItemMap::const_iterator i = Item::itemMap().find(_item);
  assert(i != Item::itemMap().end());
  const std::pair<QTreeWidgetItem*, Item*>& p = *i;
  Item * const pItem = p.second;
  assert(pItem != 0);

  // Populate this context menu based on the Item
  QMenu menu(NULL);
  pItem->contextMenu(menu);

  // Show the menu
  menu.exec(pos);
}

void
ParameterListDialog::renumberEntries() 
{
  // Precondition
  assert(list_ != NULL);

  // For each top-level item
  for (int i = 0; i < list_->topLevelItemCount(); ++i)
  {
    // Serially enumerate the children of the top-level item
    QTreeWidgetItem * const pTreeWidgetItem = list_->topLevelItem(i);
    assert(pTreeWidgetItem != 0);

    QString number;
    number.setNum(i);
    pTreeWidgetItem->setText(0, number);
  }
}
