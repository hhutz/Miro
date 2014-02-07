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
#include "FileListDialog.h"

#include <q3groupbox.h>
#include <q3vgroupbox.h>
#include <q3hgroupbox.h>
#include <q3hbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#ifdef LSB_Q3LISTBOX
#include <QListWidget>
#else
#include <q3listbox.h>
#endif
#include <qstringlist.h>
#ifdef LSB_Q3FILEDIALOG
#include <QFileDialog>
#else
#include <q3filedialog.h>
#endif
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <cassert>

FileListDialog::FileListDialog(QWidget* parent,
			       QString const& _dialogTitle,
			       QString const& _listTitle,
			       char const * _filters[]) :
  QDialog(parent, "FileListDialog", TRUE),       // TRUE = modal dialog
  list_(NULL),
  delButton_(NULL),
  fileDialog_(NULL),
  modified_(false)
{
  resize(300, 200);
  setCaption(_dialogTitle);

  Q3VBoxLayout * topBox = new Q3VBoxLayout(this, 0, -1, "boxLayout");

  Q3VGroupBox * fileBox = new Q3VGroupBox(this, "fileBox");
#ifdef LSB_Q3LISTBOX
  list_ = new QListWidget(fileBox);
#else
  list_ = new Q3ListBox(fileBox, "list");
#endif

  Q3HBox * fileButtonsBox = new Q3HBox(fileBox, "fileButtons");
  QPushButton * addButton = new QPushButton("Add...", fileButtonsBox);
  delButton_ = new QPushButton("Remove", fileButtonsBox);

#ifdef LSB_Q3FILEDIALOG
  QWidget * const fdParent = this;
  const QString caption(_dialogTitle);
  const QString directory;
  QString filter;
  if (_filters)
  {
    // A non-NULL filters argument was passed to the constructor
    for (int i = 0; _filters[i] != NULL; ++i)
    {
      if (i != 0)
      {
	filter.append(";;");
      }
      filter.append(_filters[i]);
    }
  }
  else
  {
    // A NULL filters aregument was passed to the constructor
    filter = "all files (*)";
  }
  fileDialog_ = new QFileDialog(fdParent, caption, directory, filter);
#else
  fileDialog_ = new Q3FileDialog(this, "config file dialog", TRUE);
#endif

  topBox->addSpacing(10);
  topBox->addWidget(fileBox);
  fileBox->setTitle(_listTitle);

  topBox->addSpacing(10);
  Q3HBoxLayout * dialogButtonsBox = new Q3HBoxLayout(topBox, -1, "hBoxLayout");
  QSpacerItem * dBSpace = new QSpacerItem(0, 0);
  QPushButton * okButton = new QPushButton("OK", this);
  QPushButton * cancelButton = new QPushButton("Cancel", this);
  
  topBox->addSpacing(5);
  dialogButtonsBox->addItem(dBSpace);
  dialogButtonsBox->addWidget(okButton);
  dialogButtonsBox->addSpacing(5);
  dialogButtonsBox->addWidget(cancelButton);
  dialogButtonsBox->addSpacing(5);
  
  okButton->setDefault(true);

#ifdef LSB_Q3FILEDIALOG
#else
  static const char * filters[3] = { "all files (*)", NULL };

  fileDialog_->setCaption("File open dialog");
  fileDialog_->setFilters((_filters == NULL)? filters : _filters);
#endif

  // connect the dialogs functionality  
  connect(okButton,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
  connect(addButton,    SIGNAL(clicked()), SLOT(add()));
  connect(delButton_,   SIGNAL(clicked()), SLOT(del()));

  selectListItem();
}

FileListDialog::~FileListDialog()
{
}

void 
FileListDialog::init(const QStringList& _list)
{
  // fill list with current parameters
  modified_ = false;
#ifdef LSB_Q3LISTBOX
  list_->clear();
  const int row = 0;
  list_->insertItems(row, _list);
#else
  list_->clear();
  list_->insertStringList(_list);
#endif
 
  selectListItem();
}

void
FileListDialog::selectListItem()
{
#ifdef LSB_Q3LISTBOX
  // Precondition
  assert(list_ != NULL);
  if (list_->count() != 0)
  {
    // Select the current item, or item 0 if there is no current item
    QListWidgetItem * pItem = list_->currentItem();
    if (pItem == NULL)
    {
      // There is no current item; use item 0
      pItem = list_->item(0);
    }
    assert(pItem != NULL);
    // This also selects
    pItem->setSelected(true);
    // Postcondition
    assert(pItem->isSelected());
    assert(list_->selectedItems().size() > 0);
  }
#else
  if (list_->count() != 0) {
    int item = list_->currentItem();
    if (item == -1)
      item = 0;
    list_->setSelected(list_->item(item), true);
  }

  delButton_->setEnabled(list_->currentItem() != -1);
#endif
}

QStringList
FileListDialog::result() 
{
  QStringList list;
#ifdef LSB_Q3LISTBOX
  // Precondition
  assert(list_ != NULL);

  // Cache the size to avoid repeated accesses
  const int count = list_->count();
  for (int row = 0; row < count; ++row)
  {
    const QListWidgetItem * const pItem = list_->item(row);
    assert(pItem != NULL);
    const QString& s = pItem->text();
    list.append(s);
  }

  // Postcondition
  assert(list.size() == list_->count());
#else
  for (unsigned int i = 0; i < list_->count(); ++i)
    list.append(list_->text(i));
#endif

  return list;
}

void 
FileListDialog::add()
{
  if (fileDialog_->exec() == QDialog::Accepted ) {

#ifdef LSB_Q3LISTBOX
    // Precondition
    assert(fileDialog_ != NULL);
    assert(list_ != NULL);

    // The collection of all selected file names
    const QStringList selectedFileNames = fileDialog_->selectedFiles();
    // The number of selected file names
    const int count = selectedFileNames.size();
    assert(count > 0); // If accepted, there must be one
    // Use the first
    const QString& selectedFileName = selectedFileNames.at(0);
    // Search for the file name among the QListWidget items
    // This is case insensitive. Use Qt::MatchCaseSensitive or Qt::MatchExactly?
    Qt::MatchFlags flags = Qt::MatchFixedString; 
    const QList<QListWidgetItem*> found =
      list_->findItems(selectedFileName, flags);
    if (found.empty())
    {
      // The selected file name was not an item in the QListWidget; insert it
      // Insert at the end
      const int row = list_->count();
      list_->insertItem(row, selectedFileName);
      // Notify that the QListWidget has been modified
      modified_ = true;
    }
#else
    if (list_->find_item(fileDialog_-selectedFile()) != NULL) {
      list_->insertItem(fileDialog_->selectedFile());
      modified_ = true;
    }
#endif
    else {
      QMessageBox::warning(this, "Duplicated file", "Selected file is already part of the file list.");
    }
  }

  selectListItem();
}

void 
FileListDialog::del() {
#ifdef LSB_Q3LISTBOX
  // Precondition
  assert(list_ != 0);
  
  QListWidgetItem * const pCurrentItem = list_->currentItem();
  if (pCurrentItem != NULL)
  {
    // There is a current item; remove it from the QListWidget
    /// @todo Does this delete the item?
    /// @todo Should we use QListWidget::take() and delete it ourselves?
    list_->removeItemWidget(pCurrentItem);
    modified_ = true;
  }

#else
  int item; 
  if ((item = list_->currentItem()) != -1) {
    list_->removeItem(item);
    modified_ = true;
  }
#endif
 
  selectListItem();
}
