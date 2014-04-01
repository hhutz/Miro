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

// This module
#include "OkCancelDialog.h"
// This application
#include "ItemXML.h"
// The Qt library
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <qlayout.h>
#include <qpushbutton.h>
#include <QVBoxLayout>
// The C++ Standard Library
#include <cassert>

OkCancelDialog::OkCancelDialog(QWidget * _parent, const char * _name, bool _modal) :
  Super(_parent, _name, _modal),
  frame_(NULL),
  modified_(false),
  accept_(0)
{
  // Capitalize the _name member variable and use this as the caption
  QString n = _name;
  n[0] = n[0].upper();
  setCaption(n);

  QVBoxLayout * const pTopBoxLayout = createLayout();
  createFrame();
  createGroupBox();

  // Create the Horizontal Box Layout
  QWidget * const pDialogButtonsBox = new QGroupBox;
  QHBoxLayout * const pDialogButtonsBoxLayout = new QHBoxLayout;
  assert(pDialogButtonsBoxLayout != NULL);
  {
    const int margin = 0;
    pDialogButtonsBoxLayout->setContentsMargins(margin, margin, margin, margin);
    const int spacing = -1;
    pDialogButtonsBoxLayout->setSpacing(spacing);
  }
  pDialogButtonsBox->setLayout(pDialogButtonsBoxLayout);

  pTopBoxLayout->addSpacing(10);
  // Add the GroupBox to the OkCancelDialog's Layout
  pTopBoxLayout->addWidget(groupBox_);
  pTopBoxLayout->addSpacing(10);

  QSpacerItem * const dBSpace = new QSpacerItem(0, 0);
  pDialogButtonsBoxLayout->addItem(dBSpace);
  QPushButton * const okButton = new QPushButton("OK");
  pDialogButtonsBoxLayout->addWidget(okButton);
  pDialogButtonsBoxLayout->addSpacing(5);
  QPushButton * const cancelButton = new QPushButton("Cancel");
  pDialogButtonsBoxLayout->addWidget(cancelButton);
  pDialogButtonsBoxLayout->addSpacing(5);
  
  okButton->setDefault(true);

  pTopBoxLayout->addWidget(pDialogButtonsBox);
  pTopBoxLayout->addSpacing(5);

  // connect the dialogs functionality  
  connect(this,         SIGNAL(okay(bool)), okButton, SLOT(setEnabled(bool)));
  connect(okButton,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

// Create, configure and assign the Layout for the OkCancelDialog
QVBoxLayout* OkCancelDialog::createLayout()
{
  QVBoxLayout * const pTopBoxLayout = new QVBoxLayout;
  assert(pTopBoxLayout != NULL);
  {
    const int margin = 0;
    pTopBoxLayout->setContentsMargins(margin, margin, margin, margin);
    const int spacing = margin;
    pTopBoxLayout->setSpacing(spacing);
  }
  this->setLayout(pTopBoxLayout);
  return pTopBoxLayout;
}

void OkCancelDialog::createFrame()
{
  // Create the Frame with no parent
  QWidget * const pFrameParent = NULL;
  frame_ = new QFrame(pFrameParent);
}

void OkCancelDialog::createGroupBox()
{
  // Create the Group Box to hold the main content of the dialog
  groupBox_ = new QGroupBox;
  assert(groupBox_ != NULL);

  // Create, configure and assign the Layout for the groupBox_
  QVBoxLayout * const pGroupBoxLayout = new QVBoxLayout;
  assert(pGroupBoxLayout != NULL);
  groupBox_->setLayout(pGroupBoxLayout);
  // Add  the Frame to the Group Box's Layout
  assert(frame_ != NULL);
  pGroupBoxLayout->addWidget(frame_);
}

void
OkCancelDialog::accept(bool _accept)
{
  accept_ += (_accept)? -1 : 1;

  assert(accept_ >= 0);

  bool a = accept_ == 0;
  emit okay(a);
}

void
OkCancelDialog::accept()
{
  Super::accept();
}
