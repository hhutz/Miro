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
#ifndef DocumentView_h
#define DocumentView_h

#include "DocumentXML.h"

#include "miroWidgets_Export.h"

#include <q3listview.h>
//Added by qt3to4:
#include <QCloseEvent>

// forward declarattions
class DocumentFile;
class QWidget;
class QStatusBar;

//! Main class of the DocumentView application 
/**
 * A specialized QListView, that holds an XML document visualization.
 */
class miroWidgets_Export DocumentView : public Q3ListView
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! The super class.
  typedef Q3ListView Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  DocumentView(QWidget * _parent = 0, char const * _name = 0, Qt::WFlags _f = 0);
  //! Virtual destructor.
  virtual ~DocumentView();

  //! Initialize new document.
  void newDocument();
  //! Open an XML document into the view.
  void openDocument(QString const& _name);
  //! Open an XML document from a string into the view.
  void openDocumentXML(QString const& _title, QString const& _xml);
  //! Save the XML document form the view.
  bool saveDocument();
  //! Save the XML document via QFileDialog selection.
  bool saveDocumentAs();

  //! Associate the XML document with the view.
  void setDocument(DocumentXML * _document, QString const& _rootTag);
  //! Associate a title bar with the view.
  /** The document name is displayed there. */
  void setTitleBar(QWidget * _titleBar);
  //! Associate a status bar with the view.
  /** For display of the usual status information. */
  void setStatusBar(QStatusBar * _statusBar);
  //! Try to close the document.
  /** 
   * If the document is modified, the user can cancel the
   * operation. 
   */
  void tryClose(QCloseEvent *e);

protected slots:

  //! File Menu.
  void slotNew();
  //! File Menu.
  void slotLoad();
  //! File Menu.
  void slotSave();
  //! File Menu.
  void slotSaveAs();

  //! Context Menu request.
  void slotContextMenu(Q3ListViewItem * _item, const QPoint & _pos, int);
  //! Double Click event.
  void slotDoubleClick(Q3ListViewItem * _item);

protected:
  //----------------------------------------------------------------------------
  // protected methods data
  //----------------------------------------------------------------------------

  //! Ask user if the documents changes shall be saved.
  bool saveIfModified();
  //! Display a message in the status bar, if there is one.
  void setMessage(QString const& _message, int _ms);
  //! Set the title in the title bar, if there is one.
  void setTitle(QString const& _title);

  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! The main window.
  QWidget * titleBar_;
  //! The status bar.
  QStatusBar * statusBar_;

  //! Document file.
  DocumentXML * document_;
  //! The root tag of the document.
  QString rootTag_;

private:
  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  DocumentView(DocumentView const&);
};

inline
void
DocumentView::setTitleBar(QWidget * _titleBar) {
  titleBar_ = _titleBar;
}

inline
void
DocumentView::setStatusBar(QStatusBar * _statusBar) {
  statusBar_ = _statusBar;
}


#endif

