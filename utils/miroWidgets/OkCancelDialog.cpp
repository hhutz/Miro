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
#define LSB_Q3VBOXLAYOUT

#include "OkCancelDialog.h"
#include "ItemXML.h"

#ifdef LSB_Q3VGROUPBOX
#include <Q3GroupBox>
#else
#include <q3vgroupbox.h>
#endif
#include <q3hgroupbox.h>
#ifdef LSB_Q3FRAME
#include <QFrame>
#else
#include <q3frame.h>
#endif
#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#ifdef LSB_Q3VBOXLAYOUT
#include <QVBoxLayout>
#else
#include <Q3VBoxLayout>
#endif
#include <Q3HBoxLayout>

#include <cassert>

OkCancelDialog::OkCancelDialog(QWidget * _parent, const char * _name, bool _modal) :
  Super(_parent, _name, _modal),
  frame_(NULL),
  modified_(false),
  accept_(0)
{
  QString n = _name;
  n[0] = n[0].upper();
  setCaption(n);

#ifdef LSB_Q3VBOXLAYOUT
  QWidget * const pTopBoxParent = this;
  QVBoxLayout * const topBox = new QVBoxLayout(pTopBoxParent);
  assert(topBox != NULL);
  const int margin = 0;
  topBox->setContentsMargins(margin, margin, margin, margin);
  const int spacing = -1;
  topBox->setSpacing(spacing);
#else
  Q3VBoxLayout * topBox = new Q3VBoxLayout(this, 0, -1, "boxLayout");
#endif

#ifdef LSB_Q3VGROUPBOX
  // Create the Group Box
  const QString groupBoxTitle("");
  QWidget * const pGroupBoxParent = this;
  groupBox_ = new QGroupBox(groupBoxTitle, pGroupBoxParent);
  assert(groupBox_ != NULL);

  // Create the Vertical Box Layout
  QVBoxLayout * const pGroupBoxLayout = new QVBoxLayout;
  assert(pGroupBoxLayout != NULL);

  // Assign the Vertical Box Layout to the Group Box
  groupBox_->setLayout(pGroupBoxLayout);
#else
  groupBox_ = new Q3VGroupBox(this, "groupbox");
#endif

#if defined(LSB_Q3FRAME) && defined(LSB_Q3VGROUPBOX)
  // Create the Frame with no parent
  QWidget * const pFrameParent = NULL;
  frame_ = new QFrame(pFrameParent);
  // Add the Frame to the Group Box's Layout
  pGroupBoxLayout->addWidget(frame_);
#elif defined(LSB_Q3FRAME) && !defined(LSB_Q3VGROUPBOX)
  // Create the Frame with the Group Box as parent
  QWidget * const parent = groupBox_;
  frame_ = new QFrame(parent);
#elif !defined(LSB_Q3FRAME) && defined(LSB_Q3VGROUPBOX)
  // Create the Frame with no parent
  QWidget * const pFrameParent = NULL;
  const QString frameName("parameterframe");
  frame_ = new Q3Frame(pFrameParent, frameName);
  // Add the Frame to the Group Box's Layout
  pGroupBoxLayout->addWidget(frame_);
#else
  // Create the Frame with the Group Box as parent
  frame_ = new Q3Frame(groupBox_, "parameterframe");
#endif

  Q3HBoxLayout * dialogButtonsBox = new Q3HBoxLayout(NULL, 0, -1, "hBoxLayout");
  QSpacerItem * dBSpace = new QSpacerItem(0, 0);
  QPushButton * okButton = new QPushButton("OK", this);
  QPushButton * cancelButton = new QPushButton("Cancel", this);

  topBox->addSpacing(10);
  topBox->addWidget(groupBox_);
  topBox->addSpacing(10);
  topBox->addLayout(dialogButtonsBox);
  topBox->addSpacing(5);

  dialogButtonsBox->addItem(dBSpace);
  dialogButtonsBox->addWidget(okButton);
  dialogButtonsBox->addSpacing(5);
  dialogButtonsBox->addWidget(cancelButton);
  dialogButtonsBox->addSpacing(5);
  
  okButton->setDefault(true);

  // connect the dialogs functionality  
  connect(this,         SIGNAL(okay(bool)), okButton, SLOT(setEnabled(bool)));
  connect(okButton,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
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
