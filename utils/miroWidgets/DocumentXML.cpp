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

#include "DocumentXML.h"

#include "miro/Exception.h"

#include <qfile.h>
#include <qobject.h>
#include <q3listview.h>

#include <QTextStream>

#include <cassert>

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

DocumentXML::DocumentXML(QDomDocument const& _document,
			 Q3ListView * _listView, 
			 QObject * _parent, const char * _name) :
  Super(_document, _listView, NULL, _parent, ""),
  document_(_document)
{}

DocumentXML::~DocumentXML()
{}

void
DocumentXML::init(QString const& _rootTag)
{
  clear();
  setName("");

  // init default document
  QDomElement root = document_.createElement(_rootTag);
  QDomNode n = document_.appendChild( root );
  assert(!n.isNull());

  listViewItem()->setText(0, _rootTag);
  listViewItem()->setText(2, document_.doctype().name());
  listViewItem()->setOpen(true);

  setModified(false, true);
}

void
DocumentXML::initXML(QString const& _xml)
{
  clear();
  setName("");

  QString error;
  int line;
  int column;
  if (!document_.setContent(_xml, &error, &line, &column)) {
    QString l, c;
    l.setNum(line);
    c.setNum(column);
    throw Miro::Exception(QString("XML parsing error!\n" + error + "in line " + l + ", column " + c));
  }

  setModified(false, true);
}

void
DocumentXML::clear()
{
  document_.removeChild(document_.documentElement());
  setModified(true);

  if(!children().isEmpty()) {
    QObjectList childList = children();
    while(!childList.isEmpty()) {
      delete childList.takeFirst();
    }
  }
}

//------------------------------------------------------------------------------
// document methods

void
DocumentXML::setName(const QString& _name)
{
  if (name() != _name) {
    Super::setName(_name);
    setModified(true);
  }
}

void 
DocumentXML::loadXML(const QString& _name) 
{
  clear();
  setModified(false, true);

  QFile f(_name);
  if (!f.open(QIODevice::ReadOnly)) {
    throw Miro::Exception(QString("DocumentXML::loadXML: Error: file not found!"));
  }
  if (!document_.setContent(&f)) {
    throw Miro::Exception(QString("DocumentXML::loadXML: Error: XML parsing error!"));
  }
  Super::setName(_name);
}


void
DocumentXML::saveXML()
{
  QFile f(name() );
  if (!f.open(QIODevice::WriteOnly)) {
    throw Miro::Exception(QString("DocumentXML::loadXML: Error: file error!"));
  }
  QTextStream ts(&f);
  document_.save(ts, 2);
  setModified(false, true);
}
