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
#ifndef ConfigDocumentXML_h
#define ConfigDocumentXML_h

#include "miroWidgets/DocumentXML.h"
//Added by qt3to4:
#include <Q3PopupMenu>

// forward declarations
class Q3ListView;

class ConfigDocumentXML : public DocumentXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------  
  // private types
  //----------------------------------------------------------------------------  
  
  typedef DocumentXML Super;

public:
  //----------------------------------------------------------------------------  
  // public methods
  //----------------------------------------------------------------------------  

  ConfigDocumentXML(QDomDocument const& _document,
		    Q3ListView * _listView, 
		    QObject * _parent = NULL, const char * _name = NULL);
  virtual ~ConfigDocumentXML();

  //----------------------------------------------------------------------------  
  // inherited public methods
  //----------------------------------------------------------------------------  

  virtual void parse();
  virtual void contextMenu(Q3PopupMenu& _menu);

  //----------------------------------------------------------------------------  
  // public constants
  //----------------------------------------------------------------------------  

  static const QString XML_DOCTYPE;
  static const QString XML_TAG;

public slots:
  //----------------------------------------------------------------------------  
  // public slots
  //----------------------------------------------------------------------------  

  void onAddSection(int);

protected:
  //----------------------------------------------------------------------------  
  // protected members
  //----------------------------------------------------------------------------  

  //! The section menu.
  Q3PopupMenu * menuAddSection_;

  //----------------------------------------------------------------------------  
  // hidden methods
  //----------------------------------------------------------------------------  

  ConfigDocumentXML(ConfigDocumentXML const&);
};

#endif
