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

// This application
#include "Item.h"
// The Qt library
#include <qstring.h>
#include <qdom.h>
// The C++ Standard Library
#include <vector>

#include "miroWidgets_Export.h"

// Forward declarations
class QTreeWidget;
class QTreeWidgetItem;

typedef std::vector<QString> QStringVector;

/**
 * This class represents an item in the XML tree.
 * It encapsulates the xml processing of the item.
 * This is the base class for representing all elements.
 */
class miroWidgets_Export ItemXML : public Item
{
  Q_OBJECT;

  //----------------------------------------------------------------------------
  // private types
  //----------------------------------------------------------------------------

  /**
   * Base class type.
   * This is the immediate base class.
   */
  typedef Item Super;

public:
  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidgetItem.
   * @param[in] _node reference to the node in the DOM tree
   * @param[in] _parentItem pointer to the QTreeWidgetItem for the parent of
   * this item
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   */
  ItemXML(QDomNode const& _node,
	  QTreeWidgetItem * _parentItem,
	  QTreeWidgetItem * _pre = NULL,
	  QObject * _parent = NULL,
	  const char * _name = NULL);

  /**
   * Initializing constructor used when the parent of the QTreeWidgetItem is
   * a QTreeWidget.
   * @param _node reference to the node in the DOM tree
   * @param _view the QTreeWidget which contains the QTreeWidgetItem for this
   * ItemXML
   * representing the item.
   * @param[in] _pre pointer to the QTreeWidgetItem for the predecessor of this
   * item
   * @param[in] _parent the QObject that is the parent of this ItemXML
   * @param[in] _name the name of this ItemXML; may be NULl or empty
   */
  ItemXML(QDomNode const& _node,
	  QTreeWidget * _view,
	  QTreeWidgetItem * _pre = NULL,
	  QObject * _parent = NULL,
	  const char * _name = NULL);

  /**
   * Virtual destructor.
   * Delete the associated QWidget, if any.
   */
  virtual ~ItemXML();

  /**
   * Rename item within the document.
   * If the name in the DOM tree elemen is not _name, and _name is not empty,
   * set the DOM tree's element's name to _name and set the QTreeWidgetItem's
   * text to _name. Notify that the ItemXML has been modified.
   * @param[in] _name the new name
   */
  virtual void rename(const QString& _name);

  /**
   * Move item within the document.
   * @param[in] _parent the parent QDomNode of the QDomNode for this ItemXML
   * @param[in] _predecessor the predecessor QDomNode of the QDomNode for this
   * ItemXML
   */
  virtual void moveTo(QDomNode& _parent, QDomNode& _predecessor);

  /**
   * Test if item is deletable.
   * If the ItemXML has children, show a QMessageBox confirmer dialog to
   * solicit permission to delete the ItemXML.
   */
  virtual bool deletable();

  /**
   * Delete item from the document.
   * Schedule the ItemXML for later deletion. Remove the corresponding QDomNode
   * from the DOM tree. Notify that the parent ItemXML, if any, has been
   * modified.
   */
  virtual void deleteItem();

  /**
   * The node associated with the item.
   * @return the QDomNode for this ItemXML
   */
  QDomNode node() const;

  /**
   * Report if the node or any of its children was modified.
   * @retval true if this ItemXML or any of its children has been modified.
   * @retval false if this ItemXML or all of its children have not been
   * modified.
   */
  bool modified() const;

  /**
   * Associate a widget with an ItemXML.
   * This is usually used to edit the underlying QDomNode.
   * @param[in,out] _widget a pointer to a QWidget; may be NULL
   */
  void setWidget(QWidget * _widget);

  /**
   * Accessor for the associated widget.
   * Note that the return value can be NULL.
   * @return the QWidget associated with this ItemXML; may be NULL
   */
  QWidget * widget();

  /**
   * Redisplay widget if existing.
   * If the associated QWidget is not NULL, call QWidget::update() on it.
   */
  void updateWidget();

  /**
   * If the associated QWidget is not NULL, call QWidget::updateGeometry() on
   * it.
   */
  void updateWidgetGeometry();

  //----------------------------------------------------------------------------
  // inherited public methods
  //----------------------------------------------------------------------------

  /**
   * Provide the context menu.
   * Partially populate the context menu for the ItemXML.
   * There is one menu item, "Set Name".
   * The slot is to invoke slotRename().
   * @param[in,out] _menu the context menu
   */
  virtual void contextMenu(QMenu& _menu);

  /**
   * Move item up in the QTreeWidget and in the document DOM tree.
   */
  virtual void moveUp();

  /**
   * Move item down in the QTreeWidget and in the document DOM tree.
   */
  virtual void moveDown();

  //----------------------------------------------------------------------------
  // public constants
  //----------------------------------------------------------------------------

  /**
   * The xml attribute for key values.
   * The element's attribute is "name".
   */
  static QString const XML_ATTRIBUTE_KEY;

public slots:

  /**
   * Delete element from the document if it is deletable.
   */
  void slotDelete();

protected slots:

  /**
   * Set the name attribute of the element.
   * Show a QInputDialog to solicit the new name. On success, if the new name
   * is not empty and different from the current name, re-name the item.
   */
  void slotRename();

protected:

  //----------------------------------------------------------------------------
  // protected methods
  //----------------------------------------------------------------------------

  /**
   * Set the modified bit.
   * @param[in] _modified whether the ItemXML has been modified; defaults to
   * true
   * @param[in] _recurse whether to notify that all children are also modified;
   * defaults to false.
   */
  void setModified(bool _modified = true, bool _recurse = false);

  //----------------------------------------------------------------------------
  // protected data
  //----------------------------------------------------------------------------

  //! Reference to the node in the xml document representing the item.
  QDomNode node_;

  /**
   * An associated widget that needs to be updated due to changes here.
   * It is set after construction. By default it is NULL.
   */
  QWidget * widget_;

private:

  //----------------------------------------------------------------------------
  // private data
  //----------------------------------------------------------------------------

  /**
   * Modification indicator.
   * A dirty bit; true iff the ItemXML has been notified as modified.
   */
  bool modified_;

  //----------------------------------------------------------------------------
  // hidden methods
  //----------------------------------------------------------------------------

  /** Disable the copy constructor. */
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
