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
// #define LSB_Q3LISTVIEW
// #define LSB_Q3LISTVIEWITEM

#ifndef ConfigDocumentXML_h
#define ConfigDocumentXML_h

#include "miroWidgets/DocumentXML.h"

// forward declarations
#ifdef LSB_Q3LISTVIEW
class QTreeWidget;
#else
class Q3ListView;
#endif

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
#ifdef LSB_Q3LISTVIEW
		    QTreeWidget * _treeWidget,
#else
		    Q3ListView * _listView,
#endif
		    QObject * _parent = NULL, const char * _name = NULL);
  virtual ~ConfigDocumentXML();

  //----------------------------------------------------------------------------  
  // inherited public methods
  //----------------------------------------------------------------------------  

  virtual void parse();
  virtual void contextMenu(QMenu& _menu);

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
  QMenu * menuAddSection_;

  //----------------------------------------------------------------------------  
  // hidden methods
  //----------------------------------------------------------------------------  

  ConfigDocumentXML(ConfigDocumentXML const&);
};

#endif
