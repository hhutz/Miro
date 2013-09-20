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
#include "ItemXML.h"

#include <q3popupmenu.h>
#include <qobject.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <q3listview.h>

#include <cassert>

QString const ItemXML::XML_ATTRIBUTE_KEY = "name";

ItemXML::ItemXML(QDomNode const& _node,
		 Q3ListViewItem * _parentItem, Q3ListViewItem * _pre,
		 QObject * _parent, const char * _name) :
  Super(_parentItem, _pre, _parent, _name),
  node_(_node),
  widget_(NULL),
  modified_(false)
{
  assert(!node_.isNull());
}

ItemXML::ItemXML(QDomNode const& _node,
		 Q3ListView * _view, Q3ListViewItem * _pre,
		 QObject * _parent, const char * _name) :
  Super(_view, _pre, _parent, _name),
  node_(_node),
  widget_(NULL),
  modified_(false)
{
  assert(!node_.isNull());
}

ItemXML::~ItemXML()
{
  delete widget_;
}

bool 
ItemXML::modified() const
{
  bool m = modified_;
  
  if (!children().isEmpty()) {
    QListIterator<QObject*> it(children());
    while (!m && it.hasNext() ) {
      ItemXML const * item = dynamic_cast<ItemXML const *>(it.next());
      if (item != NULL) {
	m |= item->modified();
      }
    }
  }
  return m;
}

void 
ItemXML::setModified(bool _modified, bool _recurse) 
{
  modified_ = _modified;

  if (_recurse && !children().isEmpty()) {
    QListIterator<QObject*> it(children());
    while (it.hasNext()) {
      ItemXML * item = dynamic_cast<ItemXML *>(it.next());
      if (item != NULL) {
	item->setModified(_modified, _recurse);
      }
    }
  }
}

void 
ItemXML::contextMenu(Q3PopupMenu& _menu)
{
  _menu.insertItem("Set Name", this, SLOT(slotRename()));
  _menu.insertSeparator();
}

void
ItemXML::slotRename()
{
  QDomElement e = node_.toElement();
  assert(!e.isNull());

  bool ok = false;
  QString text = 
    QInputDialog::getText(tr( "Rename Entry" ),
			  tr( "New name:" ),
			  QLineEdit::Normal,
			  name(), &ok, NULL);
  if (ok && 
      !text.isEmpty() && 
      text != name()) {
    rename(text);
  }
}

void
ItemXML::moveUp()
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
ItemXML::moveDown()
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

bool
ItemXML::deletable()
{
  int rc = QMessageBox::Ok;
  // Ask for confirmation in case of child elements.
  if (!node_.firstChild().isNull()) {
    rc = QMessageBox::warning(NULL, 
			      "Delete Element",
			      "Deleting this element will delete all its child elements, too." ,
			      QMessageBox::Ok, QMessageBox::Cancel);
  }
  return rc == QMessageBox::Ok;
}

void
ItemXML::slotDelete()
{
  if (deletable())
    deleteItem();
}

//----------------------------------------------------------------------------
// public methods
//----------------------------------------------------------------------------

void 
ItemXML::deleteItem()
{
  // schedule for deletion
  // this will also remove the listviewitem
  deleteLater();

  // remove this node at the parent node
  // as this is not taken care of in
  // the dtor of this class
  QDomNode n = node_.parentNode();
  assert(!n.isNull());
  n.removeChild(node_);

  // tell the parent, that he is modified
  // as he will not be able to ask us about it anymore
  ItemXML * p = 
    dynamic_cast<ItemXML *>(parent());
  if (p != NULL) {
    p->setModified();
  }
}

void 
ItemXML::rename(QString const& _name)
{
  // convert to element
  QDomElement element = node_.toElement();
  assert(!element.isNull());

  // test for identity
  QString name = element.attribute(XML_ATTRIBUTE_KEY);
  if (name != _name) {
    // rename element
    element.setAttribute(XML_ATTRIBUTE_KEY, _name);
    setName(_name.latin1());
    listViewItem()->setText(0, _name);
    setModified();
  }

  if (widget_)
    widget_->update();
}

void
ItemXML::moveTo(QDomNode& _parent, QDomNode& _predecessor)
{
  // the corresponding syntax in DOM looks slightly different:
  _parent.insertAfter(node_, _predecessor);
  setModified();
}

void
ItemXML::updateWidget() 
{
  if (widget_ != NULL) {
    widget_->update();
  }
}

void
ItemXML::updateWidgetGeometry() 
{
  if (widget_ != NULL) {
    widget_->updateGeometry();
  }
}
