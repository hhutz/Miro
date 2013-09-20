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
#ifndef DocumentXML_h
#define DocumentXML_h

#include "ItemXML.h"

#include "miroWidgets_Export.h"

// forward declarations
class Q3ListView;


//! Class representing an XML document.
/** 
 * It has also an associated QListViewItem, that represents the node
 * in a QListView.
 */
class miroWidgets_Export DocumentXML : public ItemXML
{
  Q_OBJECT

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef ItemXML Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor. Taking a XML document as first parameter.
  DocumentXML(QDomDocument const& _document,
	      Q3ListView * _listView, 
	      QObject * _parent = NULL, const char * _name = NULL);
  //! Virtual destructor.
  virtual ~DocumentXML();

  //! Reset policy document.
  virtual void init(QString const& _rootTag);
  //! Set policy document.
  virtual void initXML(QString const& _xml);
  //! Parse document and build list view tree
  virtual void parse() = 0;
  //! Clean up document.
  virtual void clear();
  
  //! Set the name of the document.
  void setName(const QString& _name);
  //! Load a new document from a given XML file.
  void loadXML(const QString& _name);
  //! Save the document to a given XML file.
  void saveXML();

  //! Accessor method to the xml document.
  QDomDocument const& domDocument() const;

protected:
  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! The main document structure.
  QDomDocument document_;

private:
  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  DocumentXML(DocumentXML const&);
};

inline
QDomDocument const& 
DocumentXML::domDocument() const {
  return document_;
}
#endif
