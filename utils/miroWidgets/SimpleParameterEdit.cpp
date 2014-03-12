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
#include "SimpleParameterEdit.h"
#include "ParameterDialog.h"
#include "Validators.h"
#include "ConfigFile.h"

#include "params/Parameter.h"
#include "params/Generator.h"

#include <qstring.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qtooltip.h>
#ifdef LSB_Q3LISTVIEW
#include <QTreeWidget>
#else
#include <q3listview.h>
#endif
#include <qcombobox.h>
#ifdef LSB_Q3LISTBOX
#include <QListWidget>
#else
#include <q3listbox.h>
#endif

#include <climits>
#include <cassert>

using std::vector;
using std::string;

#include "qt_compatibility.h"

#ifdef LSB_Q3LISTBOX
/**
 * Function object for inserting an item into a QListWidget by string name.
 * @author Lee Brownston
 * @date 07 Feb 2014
 */
class InsertListBoxItem 
{
public:
  InsertListBoxItem(QListWidget& lw) : m_lw(lw) { }
  void operator()(const std::string& s) {
    const int row = m_lw.count(); // insert at the end
    m_lw.insertItem(row, s.c_str());
  }
private:
  QListWidget& m_lw;
};
#endif

SimpleParameterEdit::SimpleParameterEdit(SimpleParameter::Type _type,
					 Miro::CFG::Parameter const& _parameter,
					 QDomNode& _parentNode, 
					 QDomNode& _node,
					 ItemXML * _parentItem, 
					 ItemXML * _item,
					 QWidget * _parent, 
					 const char * _name) :
  Super(_parameter, _parentNode, _node, 
	_parentItem, _item,
	_parent, _name),
  config_(ConfigFile::instance()),
  type_(_type),
  lineEdit_(NULL),
  textEdit_(NULL),
  typeBox_(NULL),
  listBox_(NULL),
  typeBoxModified_(false),
  listBoxModified_(false)
{
  if (parameter_.type_ == "Miro::Enumeration" || 
      parameter_.type_ == "Enumeration") {
    typeBox_ = new QComboBox(_parent, "type_box");
    editWidget_ = typeBox_;
  } else if (parameter_.type_ == "Miro::EnumerationMultiple" || 
	     parameter_.type_ == "EnumerationMultiple") {
#ifdef LSB_Q3LISTBOX
    if (_parent->layout() != NULL)
      {
	// Create the ListWidget without parent and add to the _parent's layout
	listBox_ = new QListWidget;
	_parent->layout()->addWidget(listBox_);
      }
    else
      {
	// Create the ListWidget as a child of _parent
	listBox_ = new QListWidget(_parent);
      }
#else
    listBox_ = new Q3ListBox(_parent, "list_box");
#endif
    editWidget_ = listBox_;
  } else if (parameter_.type_ == "Miro::Text" ||
	     parameter_.type_ == "Text") {
    textEdit_ = new QTextEdit(_parent, "text_edit");
    textEdit_->setAcceptRichText(false);
    textEdit_->setAutoFormatting(QTextEdit::AutoNone);
    editWidget_ = textEdit_;
  } else {
    lineEdit_ = new QLineEdit(_parent, "line_edit");
    editWidget_ = lineEdit_;
  }

  assert(!parentNode_.isNull());

  if (lineEdit_ != NULL) {
    // customize lineEdit for typesafe editing
    QValidator *  v = NULL;
    switch (_type) {
    case SimpleParameter::BOOL:
      v = new MyBoolValidator(this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::CHAR:
      lineEdit_->setMaxLength(1);
      break;
    case SimpleParameter::SHORT:
      v = new MyIntValidator(SHRT_MIN, SHRT_MAX, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::USHORT:
      v = new MyIntValidator(0, USHRT_MAX, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::INT:
      v = new MyIntValidator(INT_MIN + 1, INT_MAX - 1, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::UINT:
      v = new MyIntValidator(0, INT_MAX - 1, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::LONG:
      v = new MyIntValidator(INT_MIN + 1, INT_MAX - 1, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::ULONG:
      v = new MyIntValidator(0, INT_MAX - 1, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::FLOAT:
      v = new MyFloatValidator(this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::DOUBLE:
      v = new MyDoubleValidator(this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::ANGLE:
      v = new MyDoubleValidator(this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::MIRO_ANGLE:
      v = new MyDoubleValidator(-180., 180., 6, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::STRING:
      break;
    case SimpleParameter::ACE_TIME_VALUE:
      v = new MyDoubleValidator(0, ULONG_MAX, 6, this);
      lineEdit_->setValidator(v);
      break;
    case SimpleParameter::ACE_INET_ADDR:
      break;
    default:
      break;
    }

    // connect validator, if we have one
    if (v) {
      QObject * p = _parent;
      while (p != NULL) {
	if (dynamic_cast<ParameterDialog *>(p) != NULL) {
	  connect(v, SIGNAL(acceptable(bool)), p, SLOT(accept(bool)));
	  break;
	}
	p = p->parent();
      }
    }

    // add default as tooltip
    if (!parameter_.default_.isEmpty()) {
      QToolTip::add(lineEdit_, QString("default: ") + parameter_.default_);
    }

    // set current value
    if (!node_.isNull()) {
      QDomElement e = node_.toElement();
      if (!e.isNull() && e.hasAttribute("value")) {
	lineEdit_->setText(e.attribute("value"));
      }
    }

    // set lineEdit to unedited
    lineEdit_->setEdited(false);
  }
  else if (textEdit_ != NULL) {
    QDomElement e = node_.toElement();
    if (!e.isNull()) {
      QDomNodeList l = e.childNodes();
      unsigned int i;
      for (i = 0; i < l.length(); ++i) {
	if (l.item(i).isText()) {
	  QDomText t = l.item(i).toText();
	  textEditText_ = t.data();
	  textEdit_->setPlainText(textEditText_);
	  break;
	}
      }
    }
  }
  else if (typeBox_ != NULL) {
    vector<string> stringvec;
    switch (_type) {

    case SimpleParameter::ENUMERATION:
      // init combo box
      typeBox_->setEditable(FALSE);
      stringvec = fullDef2StringVector(parameter_.fullDefault_);
      for (vector<string>::const_iterator i= stringvec.begin(); i!=stringvec.end(); ++i)
	typeBox_->insertItem(i->c_str());

      // set current value
      if (!node_.isNull()) {
	QDomElement e = node_.toElement();
	if (!e.isNull() && e.hasAttribute("value")) {
	  typeBox_->setCurrentText(e.attribute("value"));
	}
      } else {
	typeBox_->setCurrentText(parameter_.default_);
      }

      // connect to function, so we recognize, if value is changed
      connect(typeBox_, SIGNAL(activated(const QString&)), this, SLOT(typeBoxModified()));
      break;

    case SimpleParameter::ENUMERATIONMULTIPLE:
      // init list box
      // For each selected element of 
#ifdef LSB_Q3LISTBOX
      // Precondition
      assert(listBox_ != NULL);
      listBox_->setSelectionMode(QAbstractItemView::MultiSelection);
      // Parse the parameter_ member variable into std::vector<std::string>
      stringvec = fullDef2StringVector(parameter_.fullDefault_);
      for_each(stringvec.begin(), stringvec.end(),
	       InsertListBoxItem(*listBox_));
#else
      listBox_->setSelectionMode(Q3ListBox::Multi);
      stringvec = fullDef2StringVector(parameter_.fullDefault_);
      for (vector<string>::const_iterator i= stringvec.begin(); i!=stringvec.end(); ++i)
	listBox_->insertItem(i->c_str());
#endif

      // set current value
#ifdef LSB_Q3LISTBOX
      /// @todo Does this still work? I can't find it in the documentation
      listBox_->clearSelection();
#else
      listBox_->clearSelection();
#endif
      if (!node_.isNull()) {
	QDomElement e = node_.toElement();
	if (!e.isNull() && e.hasAttribute("value")) {
	  QString tmp1 = e.attribute("value");
	  std::vector<std::string> tmp2 = tokenizer(std::string(tmp1.latin1()));
	  for (std::vector<std::string>::const_iterator i=tmp2.begin(); i!=tmp2.end(); ++i) {
#ifdef LSB_Q3LISTBOX
	    /// @todo Unify repeated code
	    // Cache the string to avoid repeated lookups and construction
	    const QString qs(i->c_str());
	    const int rowCount = listBox_->count();
	    for (int row = 0; row < rowCount; ++row)
	    {
	      // The item at this row
	      QListWidgetItem * const pItem = listBox_->item(row);
	      assert(pItem != 0);
	      const QString& text = pItem->text();
	      if (text == qs)
	      {
		pItem->setSelected(true);
	      }
	    } // end for
#else
	    for (unsigned int j=0; j!=listBox_->count(); ++j) {
	      if (listBox_->text(j) == QString(i->c_str()))
		listBox_->setSelected(j, TRUE);
	    }
#endif
	  }
	}
      } else {
	std::vector<std::string> tmp2 = tokenizer(std::string(parameter_.default_.latin1()));
	for (std::vector<std::string>::const_iterator i=tmp2.begin(); i!=tmp2.end(); ++i) {
#ifdef LSB_Q3LISTBOX
	  /// @todo Unify repeated code
	  // Cache the string to avoid repeated lookups and construction
	  const QString qs(i->c_str());
	  const int rowCount = listBox_->count();
	  for (int row = 0; row < rowCount; ++row)
	  {
	    // The item at this row
	    QListWidgetItem * const pItem = listBox_->item(row);
	    assert(pItem != 0);
	    const QString& text = pItem->text();
	    if (text == qs)
	    {
      	      pItem->setSelected(true);
	    }
	  } // end for
#else
	  for (unsigned int j=0; j!=listBox_->count(); ++j) {
	    if (listBox_->text(j) == QString(i->c_str()))
	      listBox_->setSelected(j, TRUE);
	  }
#endif
	}
      }

      // connect to function, so we recognize, if value is changed
      connect(listBox_, SIGNAL(selectionChanged()), this, SLOT(listBoxModified()));
      break;

    default:
      break;
    }
  }
}

void
SimpleParameterEdit::setXML()
{
  // no edit -> nothing to be done
  if (lineEdit_ && !lineEdit_->edited())
    return;

  // delete entry if edit field is empty
  if (!parentNode_.isNull() && 
      ((lineEdit_ && lineEdit_->text().isEmpty()) || 
       (textEdit_ && textEdit_->toPlainText().isEmpty()) ||
       (typeBox_ && !typeBoxModified_) || 
       (listBox_ && !listBoxModified_))) {
    if (!node_.isNull()) {
      // if we know about the associated list view,
      // we delete this one too
      if (item_ != NULL) {
	delete item_;
      }
      parentNode_.removeChild(node_);
    }
    else {
      if (lineEdit_)
	lineEdit_->setEdited(false);
      else if (textEdit_)
	textEditText_ = textEdit_->toPlainText();
      else if (typeBox_)
	typeBoxModified_ = false;
      else if (listBox_)
	listBoxModified_ = false;
    }
    return;
  }

  // crete a node if necessary
  if (!parentNode_.isNull() &&
      node_.isNull()) {

    assert(!parentNode_.ownerDocument().isNull());
    QDomElement e = parentNode_.ownerDocument().createElement(XML_TAG_PARAMETER);

    e.setAttribute(XML_ATTRIBUTE_KEY, name());
    node_ = parentNode_.appendChild(e);

    assert(!node_.isNull());
    
    // if we know about the associated list view,
    // we add the edit field, too
    if (parentItem_ != NULL) {
      ParameterXML * newParam =
	new SimpleParameter(parameter_,
			    node_,
			    parentItem_->treeWidgetItem(), NULL,
			    parentItem_, name());
      newParam->init();
      item_ = newParam;
    }
  }

  QDomElement e = node_.toElement();
  assert(!e.isNull());

  if (lineEdit_) {
    if (e.attribute(XML_ATTRIBUTE_VALUE) != lineEdit_->text()) {
      e.setAttribute(XML_ATTRIBUTE_VALUE, lineEdit_->text());
      // if we know about the associated list view,
      // we set the new value there, too
      if (item_ != NULL)
	item_->treeWidgetItem()->setText(1, e.attribute(XML_ATTRIBUTE_VALUE));
    }
    else {
      lineEdit_->setEdited(false);
    }
  }
  else if (textEdit_) {

    // if already existing, replace text
    QDomNodeList l = e.childNodes();
    unsigned int i;
    for (i = 0; i < l.length(); ++i) {
      if (l.item(i).isText()) {
	QDomText t = l.item(i).toText();
	t.setNodeValue(textEdit_->toPlainText());
	break;
      }
    }
    // otherwise create text node and add text
    if (i == l.length()) {
      std::cout << "ping" << std::endl;
      QDomText t = parentNode_.ownerDocument().createTextNode(textEdit_->toPlainText());
      e.appendChild(t);
      std::cout << t.data() <<std::endl;
    }
  }
  else if (typeBox_) {
    if (e.attribute(XML_ATTRIBUTE_VALUE) != typeBox_->currentText()) {
      e.setAttribute(XML_ATTRIBUTE_VALUE, typeBox_->currentText());
      // if we know about the associated list view,
      // we set the new value there, too
      if (item_ != NULL)
	item_->treeWidgetItem()->setText(1, e.attribute(XML_ATTRIBUTE_VALUE));
    }
    else {
      typeBoxModified_ = false;
    }
  }
  else if (listBox_) {
    // create again a space separated string for all the selections
    QString selectString;
    int j=0; 
#ifdef LSB_Q3LISTBOX
    // Cache the value to avoid repeated lookups
    const int rowCount = listBox_->count();
    for (int row = 0; row < rowCount; ++row)
    {
      const QListWidgetItem * const pItem = listBox_->item(row);
      assert(pItem != 0);
      if (pItem->isSelected())
      {
	selectString = pItem->text();
	j = row;
	break;
      }
    }
    // selectString consists of the text of the first selected item and
    // j is the index of the first selected item
    // Now concatenate the text of all the other selected items
    for (int row = j + 1; row < rowCount; ++row)
    {
      const QListWidgetItem * const pItem = listBox_->item(row);
      assert(pItem != 0);
      if (pItem->isSelected())
      {
	selectString += " " + pItem->text();
      }
    }
#else
    for (unsigned int i=0; i<listBox_->count(); ++i)
      if (listBox_->isSelected(i)) {
	selectString = listBox_->text(i);
	j = i;
	break;
      }
    for (unsigned int i=j+1; i<listBox_->count(); ++i)
      if (listBox_->isSelected(i))
	selectString += " " + listBox_->text(i);
#endif
    //
    if (e.attribute(XML_ATTRIBUTE_VALUE) != selectString) {
      e.setAttribute(XML_ATTRIBUTE_VALUE, selectString);
      // if we know about the associated list view,
      // we set the new value there, too
      if (item_ != NULL)
	item_->treeWidgetItem()->setText(1, e.attribute(XML_ATTRIBUTE_VALUE));
    }
    else {
      listBoxModified_ = false;
    }
  }
}

bool
SimpleParameterEdit::modified() const 
{
  if (lineEdit_) 
    return lineEdit_->edited();
  else if (textEdit_) 
    return textEdit_->toPlainText() != textEditText_;
  else if (typeBox_)
    return typeBoxModified_;
  else if (listBox_)
    return listBoxModified_;
  return false;
}


vector<string>
SimpleParameterEdit::fullDef2StringVector(string const& _string)
{
  // it's the full default value, so we first have to look for the second part
  string tmp = _string.substr(_string.find(",")+1);
  tmp = tmp.substr(tmp.find("\"")+1);
  tmp = tmp.substr(0, tmp.rfind("\""));

  return tokenizer(tmp);
}

vector<string>
SimpleParameterEdit::fullDef2StringVector(QString const& _qstring)
{
  // it's the full default value, so we first have to look for the second part
  string _string(QSTRCAST(_qstring));
  string tmp = _string.substr(_string.find(",")+1);
  tmp = tmp.substr(tmp.find("\"")+1);
  tmp = tmp.substr(0, tmp.rfind("\""));

  return tokenizer(tmp);
}


std::vector<std::string>
SimpleParameterEdit::tokenizer(std::string _values)
{
  int pos = 0;
  std::vector<std::string> values;
  while (_values.find(" ", pos) < _values.size()) {
    string tmp = _values.substr(pos, _values.find(" ", pos)-pos);
    if ((tmp != " ") && (tmp != ""))
      values.push_back(tmp);
    pos = _values.find(" ", pos)+1;
  }
  string tmp = _values.substr(pos, _values.size());
  if ((tmp != " ") && (tmp != ""))
    values.push_back(tmp);

  return values;
}


void
SimpleParameterEdit::typeBoxModified()
{
  typeBoxModified_ = true;
}


void
SimpleParameterEdit::listBoxModified()
{
  listBoxModified_ = true;
}

