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
class QTreeWidget;

/** 
 * Class representing an XML document.
 * This is the root node.
 * It has also an associated QTreeWidgetItem that represents the node
 * in a QTreeWidget.
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

  /**
   * Initializing constructor. Taking a XML document as first parameter.
   * @param[in,out] _document the DOM tree for the document being edited
   * @param[in,out] _treeWidget the QTreeWidget for viewing the document
   * @param[in,out] _parent ?
   * @param[in] _name ignored
   */
  DocumentXML(QDomDocument const& _document,
	      QTreeWidget * _treeWidget,
	      QObject * _parent = NULL,
	      const char * _name = NULL);

  /**
   * Virtual destructor.
   * The destructor has nothing to do.
   */
  virtual ~DocumentXML();

  /**
   * Reset the policy document.
   * Clear the document, set its name to the empty string, create the root
   * element, set the name and type of the root element, and notify that the
   * document is not modified.
   * @param[in] _rootTag the label gien to the root QTreeWidget
   */
  virtual void init(QString const& _rootTag);

  /**
   * Set policy document.
   * Set the content of the DOM tree for the document from the XML document
   * with the given file pathname.
   * @param[in] _xml the file pathname of the XML document from which to
   * populate the DOM tree.
   * @throw Miro::Exception on failure to set the DOM tree's contents from the
   * given file pathname
   */
  virtual void initXML(QString const& _xml);

  /**
   * Parse document and build QTreeWidget tree.
   * This is a pure virtual member function.
   */
  virtual void parse() = 0;

  /**
   * Clean up document.
   * Remove the root element and all its children.
   */
  virtual void clear();
  
  /**
   * Set the name of the document. If the name has changed, notify that the
   * document has been modified.
   * @param[in] _name the new name for the document; may be empty
   */
  void setName(const QString& _name);

  /**
   * Load a new document from a given XML file, clearning any previous contents.
   * Notify that the document has not been modified.
   * @param[in] _name the file pathname of the XML file for the document; this
   * becomes the name of the item
   * @throw Miro::Exception on failure to open the file for reading
   * @throw Miro::Exception on failure to parse the document
   */
  void loadXML(const QString& _name);

  /**
   * Save the document to a given XML file.
   * The file pathname is that from which the DOM tree was populated.
   * Notify that the document has not been modified.
   * @throw Miro::Exception of failure to open the file
   */
  void saveXML();

  /**
   * Accessor method to the xml document.
   * Return a const reference to the DOM tree.
   * @return a const reference to the DOM tree
   */
  QDomDocument const& domDocument() const;

protected:
  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  /**
   * The main document structure.
   * This is a DOM tree populated by parsing an XML file.
   */
  QDomDocument document_;

private:
  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  
  /** Disable the copy constructor. */
  DocumentXML(DocumentXML const&);
};

inline
QDomDocument const& 
DocumentXML::domDocument() const {
  return document_;
}
#endif
