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
#define QT_ALTERNATE_QTSMANIP

#include "ConfigDocumentXML.h"
#include "Section.h"

#include "miroWidgets/ConfigFile.h"

#include "params/Generator.h"

#include <q3listview.h>
#include <q3popupmenu.h>
#include <qstring.h>

#include <algorithm>
#include <iostream>
#include <cassert>


const QString ConfigDocumentXML::XML_DOCTYPE("MiroConfigDocument");
const QString ConfigDocumentXML::XML_TAG("config");


//------------------------------------------------------------------------------
// public methods

ConfigDocumentXML::ConfigDocumentXML(QDomDocument const& _document, 
				     Q3ListView * _listView,
				     QObject * _parent, const char * _name) :
  Super(_document, _listView, _parent, _name),
  menuAddSection_(NULL)
{
}

ConfigDocumentXML::~ConfigDocumentXML()
{}

//----------------------------------------------------------------------------  
// inherited public methods

void
ConfigDocumentXML::contextMenu(Q3PopupMenu& _menu)
{
  menuAddSection_ = new Q3PopupMenu(&_menu);

  _menu.insertItem("Add Section", menuAddSection_);

  Miro::CFG::QStringVector childSections;
  Q3ListViewItem * item = listViewItem()->firstChild();
  while (item != NULL) {
    childSections.push_back(item->text(0));
    item = item->nextSibling();
  }

  // submenu: add all section names
  // not yet available in the document
  Miro::CFG::QStringVector sections =
    ConfigFile::instance()->description().groups();
  Miro::CFG::QStringVector::const_iterator first, last = sections.end();
  for (first = sections.begin(); first != last; ++first) {

    // if this section already exists, don't add it
    Miro::CFG::QStringVector::const_iterator i = 
      std::find(childSections.begin(), childSections.end(), *first);
    if (i != childSections.end())
      continue;

    // count the final parameter classes in the section
    int itemCount = 0;
    Miro::CFG::GroupMap::const_iterator f, l;
    ConfigFile::instance()->description().getGroupedTypes(*first, f, l);
    for (; f != l; ++f) {
      if (f->second.isFinal()) {
	++itemCount;
      }
    }

    // if this section is empty, don't add it
    if (itemCount == 0)
      continue;

    menuAddSection_->insertItem(*first);
  }
  connect(menuAddSection_, SIGNAL(activated(int)),
	  this, SLOT(onAddSection(int)));
}

//----------------------------------------------------------------------------  
// public slots

void 
ConfigDocumentXML::onAddSection(int _n)
{
  QDomElement config = document_.documentElement();
  QDomElement e = document_.createElement(Section::XML_TAG);
  e.setAttribute(Section::XML_ATTRIBUTE_KEY, menuAddSection_->text(_n));

  QDomNode n = config.firstChild();
  QDomNode newChild; 

  if (config.firstChild().isNull())
    newChild = config.appendChild(e);
  else
    newChild = config.insertBefore(e, n);

  assert(!newChild.isNull());
  new Section(newChild, 
	      listViewItem(), NULL, 
	      this, menuAddSection_->text(_n));
  setModified();
}

//------------------------------------------------------------------------------
// protected methods

void
ConfigDocumentXML::parse()
{
  QDomNode n = document_.firstChild();
  if (!n.isNull()) {
    QDomNode n1 = n.firstChild();
    Q3ListViewItem * pre = NULL;
    while (!n1.isNull()) {
      QDomElement e = n1.toElement();
      if (!e.isNull() &&
	  e.tagName() == Section::XML_TAG) {
	Section * section =
	  new Section(e, 
		      listViewItem(), pre, 
		      this, e.attribute("name"));
	pre = section->listViewItem();
      }
      n1 = n1.nextSibling();
    }
  }
}
