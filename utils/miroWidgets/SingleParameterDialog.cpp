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
#include "SingleParameterDialog.h"
#include "SimpleParameter.h"
#include "SimpleParameterEdit.h"
#include "DeferredParameterEdit.h"

#include "params/Parameter.h"

#include <qstring.h>
#include <q3hgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <q3scrollview.h>
//Added by qt3to4:
#include <Q3GridLayout>

#include <cassert>

/**
 * @param parameter, the parameter description of the type
 * @param node, the qdom node in the document, containing the node
 * @param parentItem, the parent item in the list view
 * @param item, the item itself in the list view
 * @param parent, the parent widget of the dialog
 * @param name, the name of the dialog
 */
SingleParameterDialog::
SingleParameterDialog(Miro::CFG::Parameter const& _parameter,
		      QDomNode const& _node,
		      ItemXML * _parentItem,
		      ItemXML * _item,
		      QWidget * _parent, const char * _name) :
  Super(_node.parentNode(), _node,
	_parentItem, _item,
	_parent, _name),
  parameter_(_parameter)
{
  assert(!_node.isNull());

  Q3GridLayout * gridLayout = 
    new Q3GridLayout(frame_, 1, 3, 2, 5, "gridLayout"); 

  // add parameter struct:
  QLabel * name = new QLabel(frame_);
  QString n = parameter_.name_;
  n[0] = n[0].upper();
  name->setText(n);
  gridLayout->addWidget(name, 0, 0);


  // search existing entry
  QDomNode parameterNode;
  if (!node_.isNull()) {
    parameterNode = node_.firstChild();
    while(!parameterNode.isNull()) {
      QDomElement e = parameterNode.toElement();
      if (!e.isNull() &&
	  e.attribute("name") == n)
	break;
      parameterNode = parameterNode.nextSibling();
    }
  }

  SimpleParameter::Type editType =
    SimpleParameter::typeFromName(parameter_.type_);


  assert(editType != SimpleParameter::NONE);

  edit_ = new SimpleParameterEdit(editType,
				  parameter_,
				  parentNode_,
				  node_,
				  parentItem_,
				  item_,
				  frame_,
				  n.latin1());
  
  // add measure
  QLabel * measure = new QLabel(frame_);
  if (!parameter_.measure_.isEmpty()) {
    measure->setText(parameter_.measure_);
    QToolTip::add(measure, (parameter_.type_ != "angle")? parameter_.type_ : QString("double"));
  }
  else
    measure->setText(parameter_.type_);

  gridLayout->addWidget(measure, 0, 2);
  gridLayout->addWidget(edit_->editWidget(), 0, 1);

  frame_->sizeHint();
  groupBox_->sizeHint(); // we need this to get the right size (hutz)
  groupBox_->setTitle("Parameters");
}

void
SingleParameterDialog::setXML()
{
  setModified(false);

  if (edit_->modified()){
    edit_->setXML();
    setModified(edit_->modified());
  }
}

