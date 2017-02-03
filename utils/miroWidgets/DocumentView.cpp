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

#include "DocumentView.h"
#include "ParameterXML.h"
#include "ItemXML.h"

#include <QFileDialog>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <QMenu>
#include <QCloseEvent>

#include <cassert>

namespace 
{
  char const * const filters[3] = { 
    "parameter files (*.xml)", 
    "all files (*)", 
    NULL 
  };
}

DocumentView::DocumentView(QWidget * _parent, char const * _name, Qt::WFlags _f) :
  Super(_parent),
  titleBar_(NULL),
  statusBar_(NULL),
  document_(NULL),
  rootTag_()
{
  //----------------------------------------------------------------------------
  // init list view
  const int columnCount = 3;
  setColumnCount(columnCount);
  QStringList headerLabels;
  headerLabels << "Item Name" << "Value" << "Type";
  setHeaderLabels(headerLabels);
  setContextMenuPolicy(Qt::CustomContextMenu);
  setRootIsDecorated(true);
  // Disable sorting
  setSortingEnabled(false);
  /// @todo How do we do this in Qt4?
  // setResizeMode(Q3ListView::AllColumns);

  // When the user right licks, present the context menu
  connect(this, 
	  SIGNAL(customContextMenuRequested(const QPoint&)),
	  this,
	  SLOT(slotContextMenu(const QPoint&)));
  // When the user double clicks, display an item editor
  connect(this,
	  SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
	  this,
	  SLOT(slotDoubleClick(QTreeWidgetItem *, int)));

  setColumnWidth(0, 400);
  setColumnWidth(1, 150);
  setColumnWidth(2, 200);
}

DocumentView::~DocumentView()
{
}

void
DocumentView::setDocument(DocumentXML * _document, QString const& _rootTag)
{
  document_ = _document;
  rootTag_ = _rootTag;

  //----------------------------------------------------------------------------
  // init document //
  newDocument();
}			 

bool
DocumentView::saveIfModified()
{
  bool success = true;

  assert(document_ != NULL);

  if (document_->modified()) {
    
    int rc = QMessageBox::warning(this, 
				  "Save if Modified", 
				  QString("The document ") + document_->name() + " has been modified\n" + 
				  "Do you want to save it?",
				  "&Save...", "&Dont't Save", "&Cancel", 0, 2);
    
    switch(rc) {
    case 0: 
      success = saveDocument(); 
      break;
    case 1:
      success = true;
      break; 
    case 2:
      success = false;
    }
  }  
  return success;
}

void 
DocumentView::newDocument()
{
  assert(document_ != NULL);

  document_->init(rootTag_);
  
  setTitle("New document.");
  QString message("New document opened.");
  setMessage(message, 3000);
  update();
}

void 
DocumentView::openDocument(const QString& _name)
{
  assert(document_ != NULL);

  if (_name.isEmpty())
    return;

  try {
    // load selected XML file
    document_->loadXML(_name);
    document_->parse();
    
    // set new caption and status bar
    setTitle(document_->name());
    QString message(_name + " opened.");
    setMessage(message, 3000);
    update();
  }
  catch (QString const& e) {
    QMessageBox::warning(this, 
			 "Error loading file", e,
			     QMessageBox::Ok, Qt::NoButton);
  }
}

void 
DocumentView::openDocumentXML(QString const& _title, QString const& _xml)
{
  assert(document_ != NULL);

  if (_xml.isEmpty())
    return;

  try {
    // load selected XML file
    document_->initXML(_xml);
    document_->parse();
    
    // set new caption and status bar
    setTitle(_title);
    QString message(_title + " opened.");
    setMessage(message, 3000);
    update();
  }
  catch (QString const& e) {
    QMessageBox::warning(this, 
			 "Error opening file", e,
			     QMessageBox::Ok, QMessageBox::NoButton);
  }
}

bool
DocumentView::saveDocument() 
{
  assert(document_ != NULL);

  bool procede = true;
  if (QString(document_->name()).isEmpty()) {
    procede = saveDocumentAs();
  }
  if (procede) {
    document_->saveXML();
    QString message(QString(document_->name()) + " saved.");
    setMessage(message, 3000);
  }
  return procede;
}

bool
DocumentView::saveDocumentAs()
{
  bool rc = true;
  bool selected = false;
  QString filename;

  while (!selected) {
    // show file dialog
    QWidget * const parent = this;
    const QString caption("Save File");
    const QString dir;
    const QString filter("Parameter Description (*.xml)");
    QString * const selectedFilter = 0;
    const QFileDialog::Options options = 0;
    filename = QFileDialog::getSaveFileName(parent, caption, dir, filter,
					    selectedFilter, options);

    if (filename.isEmpty()) {
      rc = false;
      break;
    }

    // append file extension if none is given
    QFileInfo file(filename);
    if (file.extension(false).isEmpty()) {
      filename.append(".xml");
      file.setFile(filename);
    }

    // warn if file exists
    if (!file.exists()) {
      selected = true;
    } 
    else {
      int button = 
	QMessageBox::warning(this, 
			     "DocumentView", 
			     "The file " + filename + " alread exists!\n" +
			     "Do you want to overwrite the existing file?",
			     "&Yes", "&No", QString::null, 1);
      
      if (button == 0)
	selected = true;
    }
  }

  // set new caption and document name
  setTitle(filename);
  document_->setName(filename);
  return rc;
}

void 
DocumentView::slotNew()
{
  if (saveIfModified()) {
    newDocument();
  }
}


void 
DocumentView::slotLoad()
{
  if (saveIfModified()) {
    QWidget * const parent = this;
    const QString caption("Open File");
    const QString dir;
    const QString filter("Policies (*.xml);;All Data (*)");
    QString * const selectedFilter = 0;
    const QFileDialog::Options options = 0;
    const QString filename =
      QFileDialog::getOpenFileName(parent, caption, dir, filter, 
				   selectedFilter, options);

    if (filename.isNull())
      return;

    openDocument(filename);
  }
}

void 
DocumentView::slotSave()
{
  saveDocument();
}
  

void
DocumentView::slotSaveAs()
{
  if (saveDocumentAs()) {
    saveDocument();
  }
}

void 
DocumentView::slotContextMenu(const QPoint & _pos)
{
  // Find the QTreeWidgetItem at the position passed in from the Signal
  QTreeWidgetItem * const _item = itemAt(_pos);

  if (_item == NULL)
    return;

  ItemXML::ItemMap::const_iterator item = ItemXML::itemMap().find(_item);
  assert(item != ItemXML::itemMap().end());

  QMenu menu(NULL);
  const std::pair<QTreeWidgetItem*, Item*>& p = *item;
  Item * const pItem = p.second;
  assert(pItem != 0);
  // Populate this context menu based on the Item
  pItem->contextMenu(menu);
  if (!menu.isEmpty())
  {
    const QTreeWidget * const pTreeWidget = _item->treeWidget();
    assert(pTreeWidget != NULL);
    const QPoint pos = pTreeWidget->mapToGlobal(_pos);
    menu.exec(pos);
  }
}

void
DocumentView::slotDoubleClick(QTreeWidgetItem * _item, int column)
{
  if (_item->child(0) == NULL) {
    ItemXML::ItemMap::const_iterator item = ItemXML::itemMap().find(_item);
    assert(item != ItemXML::itemMap().end());
    
    ParameterXML * param =  dynamic_cast<ParameterXML *>(item->second);
    if (param != NULL) {
      try {
	param->setParameters();
      }
      catch (QString const& e) {
	QMessageBox::warning(this, 
			     "Error editing Parameters", e,
			     QMessageBox::Ok, QMessageBox::NoButton);
      }
    }
  }
}

void 
DocumentView::tryClose(QCloseEvent *e)
{
  if (saveIfModified()) {
    e->accept();
  }
  else {
    e->ignore(); 
  }
}

void 
DocumentView::setMessage(QString const& _message, int _ms)
{
  if (statusBar_)
    statusBar_->message(_message, _ms);
}

void 
DocumentView::setTitle(QString const& _title)
{
  if (titleBar_)
    titleBar_->setCaption(_title);
}
