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
#include "ParameterEdit.h"

#include "params/Parameter.h"

#include <qwidget.h>

#include <cassert>

QString const ParameterEdit::XML_TAG_PARAMETER = "parameter";
QString const ParameterEdit::XML_ATTRIBUTE_KEY = "name";
QString const ParameterEdit::XML_ATTRIBUTE_VALUE = "value";


ParameterEdit::ParameterEdit(Miro::CFG::Parameter const& _parameter,
			     QDomNode& _parentNode, QDomNode& _node,
			     ItemXML * _parentItem, ItemXML * _item,
			     QWidget * _parent, const char * _name) :
  Super(_parent, _name),
  parameter_(_parameter),
  parentNode_(_parentNode),
  node_(_node),
  parentItem_(_parentItem),
  item_(_item)
{
  assert(!parentNode_.isNull());
}
