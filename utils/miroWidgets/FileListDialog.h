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

// Enable migration from Qt v3 to Qt v4

#ifndef FileListDialog_h
#define FileListDialog_h

#include <qdialog.h>

#include "miroWidgets_Export.h"

// forward declarations
class QListWidget;
class QFileDialog;
class QPushButton;
class QString;
class QStringList;
class QVBoxLayout;

/**
 * This class shows a dialog for the behaviour parameters.
 */
class miroWidgets_Export FileListDialog : public QDialog
{
  Q_OBJECT

public:
  FileListDialog(QWidget* parent,
		 QString const& _dialogTitle = QString("File list dialog"),
		 QString const& _listTitle = QString("Files"),
		 char const * _filters[] = NULL);
  ~FileListDialog();

  bool modified();

  void init(const QStringList& _list);
  QStringList result();

public slots:
  void add();
  void del();

protected:
  void selectListItem();

  QListWidget * list_;
  QPushButton * delButton_;  
  QFileDialog * fileDialog_;

  bool modified_;

 private:
  QVBoxLayout * createLayout();
  void createDialogBox(const QString& title, const char * filters[]);
  void createFileButtonsBox(QVBoxLayout * const pFileBoxlayout);
  void createDialogButtonsBox(QVBoxLayout * const pFileListDialogLayout);
};

inline
bool
FileListDialog::modified() {
  return modified_;
}

#endif
