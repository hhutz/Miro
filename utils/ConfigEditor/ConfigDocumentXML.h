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

// forward declarations
class QTreeWidget;

/** The document Item for a ConfigEditor document. */
class ConfigDocumentXML : public DocumentXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------  
  // private types
  //----------------------------------------------------------------------------  
  /** The immediate base class. */
  typedef DocumentXML Super;

public:
  //----------------------------------------------------------------------------  
  // public methods
  //----------------------------------------------------------------------------  
  /**   
   * @param[in,out] _document the DOM tree for the document being edited
   * @param[in,out] _treeWidget the QTreeWidget for viewing the document
   * @param[in,out] _parent ?
   * @param[in] _name passed on to the immediate base class constructor
   */
  ConfigDocumentXML(QDomDocument const& _document,
		    QTreeWidget * _treeWidget,
		    QObject * _parent = NULL,
		    const char * _name = NULL);

  /** The destructor has nothing to do. */
  virtual ~ConfigDocumentXML();

  //----------------------------------------------------------------------------  
  // inherited public methods
  //----------------------------------------------------------------------------  
  /** Create the Confguration Editor ocument's Sections. */
  virtual void parse();

  /**
   * Create the context menu for the root element.
   * There is one menu item, "Add Section".
   * The "Add Section" menu has a sub-menu of all Sections in the document
   * that have not already been chosen.
   * @param[in,out] _menu the parent menu of the context menu
   */
  virtual void contextMenu(QMenu& _menu);

  //----------------------------------------------------------------------------  
  // public constants
  //----------------------------------------------------------------------------  
  /** The string that goes in column 2, for the root QTreeWidget's type. */
  static const QString XML_DOCTYPE;
  /** The string that goes in column 0, for the root QTreeWidget's name. */
  static const QString XML_TAG;

public slots:
  //----------------------------------------------------------------------------  
  // public slots
  //----------------------------------------------------------------------------  
  /**
   * The slot to invoke when the user has chosen a Section from the context
   * menu.
   * Create the Section and insert it before the clicked Section, if any.
   */
  void onAddSection(int);

protected:
  //----------------------------------------------------------------------------  
  // protected members
  //----------------------------------------------------------------------------  
  /**
   * The section menu.
   * The context menu for adding a new Section.
   */
  QMenu * menuAddSection_;

  //----------------------------------------------------------------------------  
  // hidden methods
  //----------------------------------------------------------------------------  
  /** Disable the copy constructor. */
  ConfigDocumentXML(ConfigDocumentXML const&);
};

#endif
