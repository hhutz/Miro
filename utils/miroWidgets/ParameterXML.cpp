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
#include "ParameterXML.h"
#include "ConfigFile.h"

#ifdef LSB_Q3POPUPMENU
#include <QMenu>
#else
#include <q3popupmenu.h>
#endif
#if defined(LSB_Q3LISTVIEWITEM) && defined(LSB_Q3LISTVIEW)
#include <QTreeWidget>
#include <QTreeWidgetItem>
#else
#include <q3listview.h>
#endif

#include <cassert>

QString const ParameterXML::XML_TAG = "parameter";

ParameterXML::ParameterXML(QDomNode const& _node,
#ifdef LSB_Q3LISTVIEWITEM
			   QTreeWidgetItem * _parentItem, QTreeWidgetItem * _pre,
#else
			   Q3ListViewItem * _parentItem, Q3ListViewItem * _pre,
#endif
			   QObject * _parent, const char * _name) :
  Super(_node, _parentItem, _pre, _parent, _name),
  config_(ConfigFile::instance())
{
  assert(!node().toElement().isNull());
}

ParameterXML::ParameterXML(QDomNode const& _node,
#if defined(LSB_Q3LISTVIEWITEM) && defined(LSB_Q3LISTVIEW)
			   QTreeWidget * _view, QTreeWidgetItem * _pre,
#else
			   Q3ListView * _view, Q3ListViewItem * _pre,
#endif
			   QObject * _parent, const char * _name) :
  Super(_node, _view, _pre, _parent, _name),
  config_(ConfigFile::instance())
{
  assert(!node().toElement().isNull());
}

void 
ParameterXML::init()
{
}

#ifdef LSB_Q3POPUPMENU
void
ParameterXML::contextMenu(QMenu& _menu)
{
  QAction * pAction = NULL;

  pAction = new QAction(tr("Set Parameters"), this);
  connect(pAction, SIGNAL(triggered), this, SLOT(slotSetParameters()));
  _menu.addAction(pAction);

  _menu.addSeparator();

  pAction = new QAction(tr("Up"), this);
  connect(pAction, SIGNAL(triggered), this, SLOT(up()));
  _menu.addAction(pAction);

  pAction = new QAction(tr("Down"), this);
  connect(pAction, SIGNAL(triggered), this, SLOT(down()));
  _menu.addAction(pAction);

  _menu.addSeparator();

  pAction = new QAction(tr("Delete"), this);
  connect(pAction, SIGNAL(triggered), this, SLOT(slotDelete()));
  _menu.addAction(pAction);
}
#else
void
ParameterXML::contextMenu(Q3PopupMenu& _menu)
{
  _menu.insertItem("Set Parameters", this, SLOT(slotSetParameters()));
  _menu.insertSeparator();
  _menu.insertItem("Up", this, SLOT(up()));
  _menu.insertItem("Down", this, SLOT(down()));
  _menu.insertSeparator();
  _menu.insertItem("Delete", this, SLOT(slotDelete()));
}
#endif

void
ParameterXML::slotSetParameters()
{
  setParameters();
}

void
ParameterXML::setParameters()
{
}
