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

#include <QGroupBox>
#include <QWidget>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#ifdef LSB_Q3LISTBOX
#include <QListWidget>
#else
#include <q3listbox.h>
#endif
#include <qstringlist.h>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
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

  // Create and configure the layout for the FileListDialog
  QVBoxLayout * const pFileListDialogLayout = createLayout();

  // Create the upper pane, contining the ListBox of file pathnames and the
  // "Add" and "Remove" buttons
  const QString fileBoxTitle("");
  QGroupBox * const pFileBox = new QGroupBox(fileBoxTitle);
  assert(pFileBox != NULL);

  // The ListBox is above the pane that contains the buttons
  QVBoxLayout * const pFileBoxLayout = new QVBoxLayout;
  assert(pFileBoxLayout != NULL);
  pFileBox->setLayout(pFileBoxLayout);

#if defined(LSB_Q3LISTBOX) 
  // Create the widget that displays the list of file pathnames
  QWidget * const pListWidgetParent = pFileBox;
  list_ = new QListWidget(pListWidgetParent);
  // Add the List Widget to the Group Box's Layout
  pGroupBoxLayout->addWidget(list_);
#else
  // Create the List Box with the Group Box as parent
  list_ = new Q3ListBox(pFileBox, "list");
  pFileBoxLayout->addWidget(list_);
#endif

  createFileButtonsBox(pFileBoxLayout);

  // Create the dialog for soliciting the pathname
  createDialogBox(_dialogTitle, _filters);

  pFileListDialogLayout->addSpacing(10);
  // The ListBox goes on top
  pFileListDialogLayout->addWidget(pFileBox);
  // The title for the ListBox
  pFileBox->setTitle(_listTitle);

  pFileListDialogLayout->addSpacing(10);
  createDialogButtonsBox(pFileListDialogLayout);

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
    if (list_->findItem(fileDialog_->selectedFile()) != NULL) {
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

void
FileListDialog::createDialogBox(const QString& title, const char * filters[])
{ 
  QWidget * const fdParent = this;
  const QString caption(title);
  const QString directory;
  QString filter;
  if (filters)
  {
    // A non-NULL filters argument was passed to the constructor
    for (int i = 0; filters[i] != NULL; ++i)
    {
      if (i != 0)
      {
	filter.append(";;");
      }
      filter.append(filters[i]);
    }
  }
  else
  {
    // A NULL filters aregument was passed to the constructor
    filter = "all files (*)";
  }
  fileDialog_ = new QFileDialog(fdParent, caption, directory, filter);
}

void
FileListDialog::createFileButtonsBox(QVBoxLayout * const pFileBoxLayout)
{
  // Create the widget to hold the Add and Remove buttons, arrayed horizontally
  QWidget * const pFileButtonsBox = new QWidget;
  assert(pFileButtonsBox != NULL);

  // Create and set its Layout
  QHBoxLayout * const pFileButtonsBoxLayout = new QHBoxLayout;
  assert(pFileButtonsBoxLayout != NULL);
  pFileButtonsBox->setLayout(pFileButtonsBoxLayout);

  // Create the Add button and add it to the layout
  const QString addButtonText("Add...");
  QPushButton * const addButton = new QPushButton(addButtonText);
  assert(addButton != NULL);
  pFileButtonsBoxLayout->addWidget(addButton);

  // Create the Remove button and add it to the layout
  const QString delButtonText("Remove");
  delButton_ = new QPushButton(delButtonText);
  assert(delButton_ != NULL);
  pFileButtonsBoxLayout->addWidget(delButton_);

  pFileBoxLayout->addWidget(pFileButtonsBox);

  connect(addButton,    SIGNAL(clicked()), SLOT(add()));
  connect(delButton_,   SIGNAL(clicked()), SLOT(del()));
}

void
FileListDialog::createDialogButtonsBox(QVBoxLayout * const pFileListDialogLayout)
{
  // Create the Widget
  QWidget * const pDialogButtonsBox = new QWidget();
  assert(pDialogButtonsBox != NULL);

  // Create the Layout
  QHBoxLayout * const pDialogButtonsBoxLayout = new QHBoxLayout;
  assert(pDialogButtonsBoxLayout != NULL);

  pDialogButtonsBox->setLayout(pDialogButtonsBoxLayout);
  // Use the value of the margin for the spacing
  pDialogButtonsBoxLayout->setSpacing(-1);

  QSpacerItem * dBSpace = new QSpacerItem(0, 0);
  QPushButton * okButton = new QPushButton("OK", this);
  QPushButton * cancelButton = new QPushButton("Cancel", this);
  
  pFileListDialogLayout->addSpacing(5);
  pDialogButtonsBoxLayout->addItem(dBSpace);
  pDialogButtonsBoxLayout->addWidget(okButton);
  pDialogButtonsBoxLayout->addSpacing(5);
  pDialogButtonsBoxLayout->addWidget(cancelButton);
  pDialogButtonsBoxLayout->addSpacing(5);
  okButton->setDefault(true);

  // connect the dialogs functionality  
  connect(okButton,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));

  // Add the Widget to its Layout
  pFileListDialogLayout->addWidget(pDialogButtonsBox);
}

QVBoxLayout *
FileListDialog::createLayout()
{
  QVBoxLayout * const pFileListDialogLayout = new QVBoxLayout;
  assert(pFileListDialogLayout != NULL);
  const int margin = 0;
  pFileListDialogLayout->setContentsMargins(margin, margin, margin, margin);
  // The spacing is the same as the margin
  const int spacing = margin;
  pFileListDialogLayout->setSpacing(spacing);
  this->setLayout(pFileListDialogLayout);

  return pFileListDialogLayout;
}
