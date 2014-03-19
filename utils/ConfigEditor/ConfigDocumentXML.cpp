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

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMenu>
#include <qstring.h>

#include <algorithm>
#include <cassert>

const QString ConfigDocumentXML::XML_DOCTYPE("MiroConfigDocument");
const QString ConfigDocumentXML::XML_TAG("config");


//------------------------------------------------------------------------------
// public methods

ConfigDocumentXML::ConfigDocumentXML(QDomDocument const& _document,
				     QTreeWidget * _treeWidget,
				     QObject * _parent, const char * _name) :
  Super(_document, _treeWidget, _parent, _name),
  menuAddSection_(NULL)
{
}

ConfigDocumentXML::~ConfigDocumentXML()
{}

//----------------------------------------------------------------------------  
// inherited public methods

void
ConfigDocumentXML::contextMenu(QMenu& _menu)
{
  // The context menu has one QAction, "Add Section"
  // The "Add Section" menu is a submenu of the menu passed as argument.
  // Its signal is invoked when it is activated, not triggered.
  menuAddSection_ = _menu.addMenu(tr("Add Section"));

  // Construct a list of all siblings of the QTreeWidgetItem
  Miro::CFG::QStringVector childSections;
  // Fetch the QTreeWidgetItem
  QTreeWidgetItem * const pTreeWidgetItem = treeWidgetItem();
  assert(pTreeWidgetItem != NULL);
  // For each child of the QTreeWidgetItem
  for (int i = 0; i < pTreeWidgetItem->childCount(); ++i)
  {
      // Fetch the ith sibling
      const QTreeWidgetItem * const pChildTreeWidgetItem =
	pTreeWidgetItem->child(i);
      assert(pChildTreeWidgetItem != NULL);
      // Fetch the sibling's label text
      const QString text = pChildTreeWidgetItem->text(0);
      // Append the label text to the QStringVector
      childSections.push_back(text);
  }
  // The QStringVector should have as many elements as the parent has children
  assert(static_cast<size_t>(pTreeWidgetItem->childCount()) == 
	 static_cast<size_t>(childSections.size()));


  // submenu: add all section names not yet available in the document
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

    // The name of the Action
    const QString name = *first;
    // From "http://qt-project.org/doc/qt4-8/qaction.html":
    // "We recommend that actions are created as children of the window they
    // are used in. In most cases, actions will be children of the application's
    // main window."
    // This class doesn't have a handle to any window.
    QWidget * const pActionParent = NULL;
    QAction * const pAction = new QAction(name, pActionParent);
    connect(pAction, SIGNAL(activated(int)), this, SLOT(onAddSection(int)));
    // Add the Action to the Menu
    menuAddSection_->addAction(pAction);
  }
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
	      treeWidgetItem(), NULL, 
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
    QTreeWidgetItem * pre = NULL;
    while (!n1.isNull()) {
      QDomElement e = n1.toElement();
      if (!e.isNull() &&
	  e.tagName() == Section::XML_TAG) {
	Section * section =
	  new Section(e, 
		      treeWidgetItem(), pre,
		      this, e.attribute("name"));
	pre = section->treeWidgetItem();
      }
      n1 = n1.nextSibling();
    }
  }
}
