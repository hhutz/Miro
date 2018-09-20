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
#include "ParameterXML.h"
// This application
#include "ConfigFile.h"
// The Qt library
#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>
// The C++ Standard Library
#include <cassert>

QString const ParameterXML::XML_TAG = "parameter";

ParameterXML::ParameterXML(QDomNode const& _node,
			   QTreeWidgetItem * _parentItem,
			   QTreeWidgetItem * _pre,
			   QObject * _parent,
			   QString const& _name) :
  Super(_node, _parentItem, _pre, _parent, _name),
  config_(ConfigFile::instance())
{
  assert(!node().toElement().isNull());
}

ParameterXML::ParameterXML(QDomNode const& _node,
			   QTreeWidget * _view,
			   QTreeWidgetItem * _pre,
			   QObject * _parent,
			   QString const& _name) :
  Super(_node, _view, _pre, _parent, _name),
  config_(ConfigFile::instance())
{
  assert(!node().toElement().isNull());
}

void 
ParameterXML::init()
{
}

void
ParameterXML::contextMenu(QMenu& _menu)
{
  // Add 4 QActions to the context menu: "Set Parameters", "Up", "Down" and
  // "Delete"
  QAction * pAction = NULL;

  pAction = new QAction(tr("Set Parameters"), &_menu);
  connect(pAction, SIGNAL(triggered()), this, SLOT(slotSetParameters()));
  _menu.addAction(pAction);

  _menu.addSeparator();

  pAction = new QAction(tr("Up"), &_menu);
  connect(pAction, SIGNAL(triggered()), this, SLOT(up()));
  _menu.addAction(pAction);

  pAction = new QAction(tr("Down"), &_menu);
  connect(pAction, SIGNAL(triggered()), this, SLOT(down()));
  _menu.addAction(pAction);

  _menu.addSeparator();

  pAction = new QAction(tr("Delete"), &_menu);
  connect(pAction, SIGNAL(triggered()), this, SLOT(slotDelete()));
  _menu.addAction(pAction);
}

void
ParameterXML::slotSetParameters()
{
  setParameters();
}

void
ParameterXML::setParameters()
{
}
