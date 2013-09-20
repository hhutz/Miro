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
#ifndef OkCancelDialog_h
#define OkCancelDialog_h

#include <qdialog.h>
#include <qdom.h>
//Added by qt3to4:
#include <Q3Frame>

#include <iostream>

#include "miroWidgets_Export.h"

// forward declarations
class Q3Frame;
class Q3GroupBox;
class ItemXML;

//! Dialog base class, for editing XML nodes of the parameter framework.
class miroWidgets_Export OkCancelDialog : public QDialog
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef QDialog Super;

public:

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  OkCancelDialog(QWidget * _parent, const char * _name, bool _modal = true);

  bool modified() const;
  
signals:
  //----------------------------------------------------------------------------
  // signals
  //----------------------------------------------------------------------------

  //! Emitted, if the input of the dialog is acceptable.
  void okay(bool);

protected slots:
  //----------------------------------------------------------------------------
  // protected slots
  //----------------------------------------------------------------------------

  //! Inherited method.
  /** Adds no functionality. Duplicated only, to avoid warning about
   *  hidden virtual.
   */ 
  virtual void accept();
  //! Tell the dialog to accept the input or not.
  /**
   * This is used for a voting method on all child edit fields.
   * The input is only accepted if noone objects.
   */
  void accept(bool _accept);

protected:
  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  //! Set modified flag.
  void setModified(bool _modified);

  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Layout element needed.
  Q3GroupBox * groupBox_;
  //! Layout element.
  Q3Frame * frame_;

private:
  //----------------------------------------------------------------------------
  // private data
  //----------------------------------------------------------------------------

  //! Flag indicating the dialog was modified.
  bool modified_;
  //! Acceptance counter.
  /** accept(false) increments, accept(true) decrements.
   * accept_ == 0 => okay button active
   */
  int accept_;
};

inline
bool
OkCancelDialog::modified() const {
  return modified_;
}

inline
void
OkCancelDialog::setModified(bool _modified) {
  modified_ = _modified;
}
#endif
