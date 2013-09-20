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
#ifndef ItemXML_h
#define ItemXML_h

#include "Item.h"

#include <qstring.h>
#include <qdom.h>
#include <Q3PopupMenu>

#include <vector>

#include "miroWidgets_Export.h"

typedef std::vector<QString> QStringVector;

//! This class represents an item in the XML tree.
/**
 * It encapsulates the xml processing of the item.
 */
class miroWidgets_Export ItemXML : public Item
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  //! Base class type.
  typedef Item Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  //! Initializing constructor.
  /**
   * @param node Reference to the node in the xml document
   * representing the item.
   */
  ItemXML(QDomNode const& _node,
	  Q3ListViewItem * _parentItem, Q3ListViewItem * _pre = NULL,
	  QObject * _parent = NULL, const char * _name = NULL);
  //! Initializing constructor.
  /**
   * @param node Reference to the node in the xml document
   * representing the item.
   */
  ItemXML(QDomNode const& _node,
	  Q3ListView * _view, Q3ListViewItem * _pre = NULL,
	  QObject * _parent = NULL, const char * _name = NULL);
  //! Virtual destructor.
  virtual ~ItemXML();


  //! Rename item within the document.
  virtual void rename(const QString& _name);
  //! Move item within the document.
  virtual void moveTo(QDomNode& _parent, QDomNode& _predecessor);
  //! Test if item is deletable.
  virtual bool deletable();
  //! Delete item from the document.
  virtual void deleteItem();

  //! The node associated with the item.
  QDomNode node() const;

  //! Report if the node or one of its childs was modified.
  bool modified() const;

  //! Associate a widget with an ItemXML.
  /** This is usually used to edit the underlying QDomNode. */
  void setWidget(QWidget * _widget);
  //! Accessor for the associated widget.
  /** Note that the return value can be NULL. */
  QWidget * widget();
  //! Redisplay widget if existing.
  void updateWidget();
  void updateWidgetGeometry();

  //----------------------------------------------------------------------------
  // inherited public methods
  //----------------------------------------------------------------------------

  //! Provide the context menu.
  virtual void contextMenu(Q3PopupMenu& _menu);
  //! Move item up in the list view and in the document.
  virtual void moveUp();
  //! Move item down  in the list view and in the document.
  virtual void moveDown();

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  //! The xml attribute for key values.
  static QString const XML_ATTRIBUTE_KEY;

public slots:
  //! Delete element from the document.
  void slotDelete();

protected slots:
  //! Set the name attribute of the element.
  void slotRename();

protected:
  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  //! Set the modified bit
  void setModified(bool _modified = true, bool _recurse = false);

  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Reference to the node in the xml document representing the item.
  QDomNode node_;
  //! an associated widget that needs to be updated due to changes here
  QWidget * widget_;

private:
  //----------------------------------------------------------------------------
  // private data
  //----------------------------------------------------------------------------

  //! Modification indicator.
  bool modified_;

  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------
  ItemXML(ItemXML const&);
};

inline
QDomNode
ItemXML::node() const {
  return node_;
}
inline
void
ItemXML::setWidget(QWidget * _widget) {
  widget_ = _widget;
}
inline
QWidget *
ItemXML::widget() {
  return widget_;
}
#endif
