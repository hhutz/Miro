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
#include "XmlParse.h"
#include "Angle.h"
#include "Text.h"
#include "Robot.h"
#include "ConfigParameters.h"

#include <qstring.h>

#include <algorithm>
#include <climits>
#include <limits>

#include "qt_compatibility.h"

#define XML_PARSE_QSTRING_IMPL(type, qstringmethod)			\
  void operator <<= (type & _lhs, const QDomNode& _node)		\
  {									\
    bool valid;								\
    QString value = getAttribute(_node, QString("value"));		\
    Miro::Robot::substitute(value);                                     \
    _lhs = value.  qstringmethod  (&valid);				\
    if (!valid) {							\
      throwException(_node, value, #type);					\
    }									\
  }									\
  QDomElement operator >>= (const type & _lhs, QDomNode& _node)		\
  {									\
    QString value;							\
    QDomDocument document = _node.ownerDocument();			\
    QDomElement e = document. createElement("parameter");		\
    value.setNum(_lhs);							\
    e.setAttribute("value", value);					\
    _node.appendChild(e);						\
    return e;								\
  }

namespace
{
  QString getAttribute(QDomNode _node, const QString& _attribute)
  {
    QString value("");

    if (!_node.isNull()) {
      QDomElement e = _node.toElement(); // try to convert the node to an element.
      if (!e.isNull()) {             // the node was really an element.
        QDomAttr attribute = e.attributeNode(_attribute);
        if (!attribute.isNull()) {
          value = attribute.value();
        }
      }
    }
    return value;
  }


  void throwException(const QDomNode& _node, QString const& value, char const * type)
  {
    QString e("Parse exception - expected type=");
    e += type;
    e += " ";

#if QT_VERSION >= 0x040100
    if (_node.lineNumber() != -1) {
      QString lineNumber;
      lineNumber.setNum(_node.lineNumber());
      e.append("line number=");
      e.append(lineNumber);
      e.append(" ");
    }
#endif

    QString name = getAttribute(_node, QString("name"));
    if (!name.isEmpty()) {
      e.append("name=");
      e.append(name);
      e.append(" ");
    }

    e.append("value=");
    e.append(value);

    throw Miro::Exception(qPrintable(e));
  }
};

namespace Miro
{
  using namespace std;

  void operator <<= (bool& lhs, const QDomNode& node)
  {
    QString value = getAttribute(node, QString("value"));
    Miro::Robot::substitute(value);
    if (value != "true" && value != "false")
      throwException(node, value, "bool");

    lhs = (value == "true");
  }

  QDomElement operator >>= (const bool& lhs, QDomNode& _node)
  {
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    e.setAttribute("value", QString((lhs) ? "true" : "false"));
    _node.appendChild(e);
    return e;
  }

  void operator <<= (char& lhs, const QDomNode& node)
  {
    QString value = getAttribute(node, QString("value"));
    Miro::Robot::substitute(value);
    if (value.length() != 1)
      throwException(node, value, "char");

#if QT_VERSION >= 0x040000
    lhs = value[0].toAscii();
#else
    lhs = value[0].latin1();
#endif
  }

  QDomElement operator >>= (const char& lhs, QDomNode& _node)
  {
    QChar c(lhs);
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    e.setAttribute("value", QString(c));
    _node.appendChild(e);
    return e;
  }

  void operator <<= (signed char& lhs, const QDomNode& node)
  {
    bool valid = false;
    QString value = getAttribute(node, QString("value"));
    Miro::Robot::substitute(value);
    signed short v = value.toUShort(&valid);
    if (!valid || v < SCHAR_MIN || v > SCHAR_MAX)
      throwException(node, value, "signed char");
    lhs = v;
  }

  QDomElement operator >>= (const signed char& lhs, QDomNode& _node)
  {
    QString value;
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    value.setNum(lhs);
    e.setAttribute("value", value);
    _node.appendChild(e);
    return e;
  }

  void operator <<= (unsigned char& lhs, const QDomNode& node)
  {
    bool valid = false;
    QString value = getAttribute(node, QString("value"));
    Miro::Robot::substitute(value);
    unsigned short v = value.toUShort(&valid);
    if (!valid || v > UCHAR_MAX)
      throwException(node, value, "unsigned char");
    lhs = v;
  }

  QDomElement operator >>= (const unsigned char& lhs, QDomNode& _node)
  {
    QString value;
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    value.setNum(lhs);
    e.setAttribute("value", value);
    _node.appendChild(e);
    return e;
  }

  XML_PARSE_QSTRING_IMPL(short, toShort);
  XML_PARSE_QSTRING_IMPL(unsigned short, toUShort);
  XML_PARSE_QSTRING_IMPL(int, toInt);
  XML_PARSE_QSTRING_IMPL(unsigned int, toUInt);
  XML_PARSE_QSTRING_IMPL(long, toLong);
  XML_PARSE_QSTRING_IMPL(unsigned long, toULong);
  XML_PARSE_QSTRING_IMPL(float, toFloat);
  XML_PARSE_QSTRING_IMPL(double, toDouble);

  void operator <<= (Angle& lhs, const QDomNode& node)
  {
    bool valid;
    QString value = getAttribute(node, QString("value"));
    Miro::Robot::substitute(value);
    lhs = deg2Rad(value.toDouble(&valid));
    if (!valid)
      throwException(node, value, "Angle");
  }

  QDomElement operator >>= (const Angle& lhs, QDomNode& _node)
  {
    QString value;
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    value.setNum(rad2Deg(lhs));
    e.setAttribute("value", value);
    _node.appendChild(e);
    return e;
  }

  void operator <<= (std::string& lhs, const QDomNode& node)
  {
    QString value = getAttribute(node, QString("value"));
    Miro::Robot::substitute(value);

    lhs = qPrintable(value);
  }

  QDomElement operator >>= (const std::string& lhs, QDomNode& _node)
  {
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    e.setAttribute("value", QString(lhs.c_str()));
    _node.appendChild(e);
    return e;
  }

  void operator <<= (Text& lhs, const QDomNode& node)
  {
    QDomNode n(node);
    if (!n.isNull()) {
      QDomElement const e = n.toElement(); // try to convert the node to an element.
      if (!e.isNull()) {             // the node was really an element.
	QString value = e.text();
	Miro::Robot::substitute(value);

        lhs = string(value.toAscii());
      }
    }
  }

  QDomElement operator >>= (const Text& lhs, QDomNode& _node)
  {
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document.createElement("parameter");
    QDomText t;
    t.setNodeValue(lhs.c_str());
    e.appendChild(t);
    _node.appendChild(e);
    return e;
  }
}
