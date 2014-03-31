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
#include "ParameterList.h"
// This application
#include "SimpleParameter.h"
#include "CompoundParameter.h"
#include "ConfigFile.h"
#include "ParameterListDialog.h"
#include "params/Generator.h"
#include "miro/Exception.h"
// The Qt library
#include <QTreeWidget>
#include <QTreeWidgetItem>
// The C++ Standard Library
#include <cassert>

namespace 
{
  struct Pair {
    char const * name;
    ParameterList::Type type;
  };

  Pair typeMap [2] = {
    { "std::vector<", ParameterList::VECTOR },
    { "std::set<",    ParameterList::SET }
  };
}


ParameterList::Type 
ParameterList::typeFromName(QString const& _type)
{
  for (unsigned int i = 0; i < 2; ++i) {
    if (_type.startsWith(typeMap[i].name))
      return typeMap[i].type;
  }
  return ParameterList::NONE;
}

ParameterList::ParameterList(Miro::CFG::Parameter const& _param,
			     QDomNode const& _node,
			     QTreeWidgetItem * _parentItem,
			     QTreeWidgetItem * _pre,
			     QObject * _parent,
			     const char * _name) :
  Super(_node, _parentItem, _pre, _parent, _name),
  param_(_param),
  type_(typeFromName(_param.type_))
{
  assert(type_ != NONE);

  if (treeWidgetItem()->treeWidget()->columnCount() > 2)
  {
    treeWidgetItem()->setText(2, param_.type_);
  }

  // get the nested parameter type
  int len = param_.type_.length();
  int vlen = (type_ == VECTOR)? 
    QString("std::vector<").length() :
    QString("std::set<").length();
  len --; // tailing >
  len -= vlen;
  nestedTypeName_ = param_.type_.mid(vlen, len);
}

ParameterList::ParameterList(Miro::CFG::Parameter const& _param,
			     QDomNode const& _node,
			     QTreeWidget * _list,
			     QTreeWidgetItem * _pre,
			     QObject * _parent, const char * _name) :
  Super(_node, _list, _pre, _parent, _name),
  param_(_param),
  type_(typeFromName(_param.type_))
{
  assert(type_ != NONE);

  if (treeWidgetItem()->treeWidget()->columnCount() > 2)
  {
    treeWidgetItem()->setText(2, param_.type_);
  }

  // get the nested parameter type
  int len = param_.type_.length();
  int vlen = (type_ == VECTOR)? 
    QString("std::vector<").length() :
    QString("std::set<").length();
  len --; // tailing >
  len -= vlen;
  nestedTypeName_ = param_.type_.mid(vlen, len);
}

void 
ParameterList::init()
{
  // get nested parameter type cathegory
  bool nestedCompound = true;
  Miro::CFG::Type const * nestedType = NULL;

  if (SimpleParameter::typeFromName(nestedTypeName_) !=
      SimpleParameter::NONE) {
    nestedCompound = false;
  }
  else {
    nestedType = config_->description().getType(nestedTypeName_);
    if (nestedType == NULL) {
      throw Miro::Exception(QString("Parameter description for " + 
				    nestedTypeName_ +
				    " not found.\nCheck whether the relevant description file is loaded (2)."));
    }
  }
  
  unsigned int index = 0;
  QDomNode n = node().firstChild();
  QTreeWidgetItem * pre = NULL;
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull() &&
	e.tagName() == ParameterXML::XML_TAG) {
      
      ParameterXML * newParam = NULL;

      QString indexName;
      indexName.setNum(index);

      if (nestedCompound == false) {

	// generate nested parameter description
	Miro::CFG::Parameter nestedParameter;
	nestedParameter.type_ = nestedTypeName_;
	nestedParameter.name_ = param_.name_;
	nestedParameter.default_ = "";
	nestedParameter.fullDefault_ = "";
	nestedParameter.measure_ = param_.measure_;
	nestedParameter.description_ = param_.description_;

	if (!e.hasAttribute(SimpleParameter::XML_ATTRIBUTE_VALUE))
	  throw Miro::Exception(QString("Parameter tag without value in (" + 
					param_.type_ + ") " + name()));

	
	QString value = e.attribute(SimpleParameter::XML_ATTRIBUTE_VALUE);
	newParam = new SimpleParameter(nestedParameter, n, treeWidgetItem(),
				       pre, this, indexName);
      }
      else {
	newParam = new CompoundParameter(*nestedType, n, treeWidgetItem(), pre,
					 this, indexName);
      }

      if (newParam != NULL) {
	newParam->init();
	pre = newParam->treeWidgetItem();
	++index;
      }
    }
    n = n.nextSibling();
  }
}

void
ParameterList::setParameters()
{
  // Precondition
  assert(treeWidgetItem() != NULL);

  // Create a parameter description for the ParameterList element's type
  // (the "nested" type)
  Miro::CFG::Parameter nestedParameter;
  nestedParameter.type_ = nestedTypeName_;
  nestedParameter.name_ = param_.name_;
  nestedParameter.default_ = "";
  nestedParameter.fullDefault_ = "";
  nestedParameter.measure_ = param_.measure_;
  nestedParameter.description_ = param_.description_;

  ItemXML * parentItem = NULL;
  QTreeWidgetItem * const p = treeWidgetItem()->parent();
  std::cout << "The parent " << p << std::endl;
  Item::ItemMap::const_iterator i = Item::itemMap().find(p);
  // If the ParameterList's parent is a QTreeWidgetItem, it will be in the map
  if (i != Item::itemMap().end())
  {
    // The ParameterList's parent is a QTreeWidgetItem.
    // The object to which it is mapped must be an ItemXML*.
    parentItem = dynamic_cast<ItemXML *>(i->second);
    assert(parentItem != NULL);
  }

  // Create the dialog to edit the ParameterList
  ParameterListDialog
    dialog(type_,
	   nestedParameter,        // description of the nested parameter
	   node_.parentNode(),     // parent of the edited DOME tree node
	   node_,                  // the edited DOME tree node
	   parentItem,             // the parent ItemXML of the QTreeWidgetItem
	   this,                   // this ParameterList
	   NULL,                   // the parent of the ParameterListDialog
	   nestedParameter.name_); // print name of the nested Parameter's type
  int rc = dialog.exec();
  if (rc == QDialog::Accepted) {
    dialog.setXML();
    if (dialog.modified())
      setModified(true);
  }
}
