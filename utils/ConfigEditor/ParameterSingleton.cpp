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
#include "ParameterSingleton.h"

#include <cassert>

ParameterSingleton::ParameterSingleton(Q3ListViewItem * _listViewItem, 
				       Q3ListViewItem * _pre,
				       QDomNode const& _node,
				       QObject * _parent, const char * _name) :
  Super(type(_node, XML_ATTRIBUTE_KEY),
	_node, 
	_listViewItem, _pre, _parent, _name)
{
}

void
ParameterSingleton::moveUp()
{
  QDomElement element = node_.toElement();
  assert(!element.isNull());
  QDomNode pre = node_.previousSibling();
  while (!pre.isNull()) {
    QDomElement e = pre.toElement();
    if (!e.isNull() &&
	e.tagName() == element.tagName()) {
      break;
    }
    pre = pre.previousSibling();
  }

  if (!pre.isNull()) {
    Super::moveUp();
    node_.parentNode().insertBefore(node_, pre);
    setModified();
  }
}

void
ParameterSingleton::moveDown()
{
  QDomElement element = node_.toElement();
  assert(!element.isNull());

  QDomNode succ = node_.nextSibling();
  while (!succ.isNull()) {
    QDomElement e = succ.toElement();
    if (!e.isNull() &&
	e.tagName() == element.tagName()) {
      break;
    }
    succ = succ.nextSibling();
  }

  if (!succ.isNull()) {
    Super::moveDown();
    node_.parentNode().insertAfter(node_, succ);
    setModified();
  }
}

