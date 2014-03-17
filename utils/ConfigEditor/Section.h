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

#ifndef Section_h
#define Section_h

#include "miroWidgets/ItemXML.h"

#include <qstring.h>

// forward declarations
class QTreeWidgetItem;

class Section : public ItemXML
{
  Q_OBJECT
  
  typedef ItemXML Super;
public:
  Section(QDomNode const& _node,
	  QTreeWidgetItem * _parentItem, QTreeWidgetItem * _pre,
	  QObject * _parent, const char * _name);

  //----------------------------------------------------------------------------  
  // inherited public methods
  //----------------------------------------------------------------------------  
  virtual void contextMenu(QMenu& _menu);

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  static QString const XML_TAG;

protected slots:
  //! Add a named instance of a parameter type.
  void onAddInstance(int _n);
  //! Add a default instance of a parameter.
  void onAddParameter(int _n);

protected:
  //! Build the subtree from the XML document.
  void buildSubtree();

  QMenu * menuAddParameter_;
  QMenu * menuAddInstance_;
};

#endif
