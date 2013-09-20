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
#ifndef DialogXML_h
#define DialogXML_h

#include "OkCancelDialog.h"

#include <qdom.h>

#include <iostream>

class ItemXML;

//! Dialog base class, for editing XML nodes of the parameter framework.
class DialogXML : public OkCancelDialog
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef OkCancelDialog Super;

public:

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  DialogXML(QDomNode const& _parentNode, QDomNode const& _node,
	    ItemXML * _parentItem, ItemXML * _item,
	    QWidget * _parent, const char * _name, bool _modal = true);

  virtual void setXML() = 0;

protected:
  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Parent node in the XML tree.
  /** Needed if there is no child node for the parameter itself yet. */
  QDomNode parentNode_;
  //! XML node of the parameter.
  QDomNode node_;

  //! Parent Item in the QListView.
  /** Needed if there is no item for the parameter itself yet. */
  ItemXML * parentItem_;
  //! Parameter item.
  ItemXML * item_;
};

#endif
