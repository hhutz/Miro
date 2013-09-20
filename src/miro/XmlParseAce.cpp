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
#include "XmlParseAce.h"

#include <ace/INET_Addr.h>
#include <ace/Sched_Params.h>

#include <qstring.h>

#include "qt_compatibility.h"

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
};

namespace Miro
{
  using namespace std;

  void operator <<= (ACE_Time_Value& lhs, const QDomNode& node)
  {
    bool valid;
    QString value = getAttribute(node, QString("value"));
#if QT_VERSION >= 0x040000
    int dot = value.indexOf('.');
#else
    int dot = value.find('.');
#endif

    QString sec = value;
    QString usec = "0";
    if (dot > 0) {
      sec = value.left(dot);
      if (dot != ((int)value.length()) - 1)
        usec = value.mid(dot + 1);
    }

    // bring usec to 6 digits
    usec += QString("000000");
    usec = usec.left(6);

    lhs.sec(sec.toULong(&valid));
    if (!valid)
      throw Exception("Parse exception (ACE_Time_Value)");
    lhs.usec(usec.toULong(&valid));
    if (!valid)
      throw Exception("Parse exception (ACE_Time_Value)");
  }

  QDomElement operator >>= (const ACE_Time_Value& lhs, QDomNode& _node)
  {
    static const QString zero("000000");

    QString s;
    QString us;
    s.setNum(lhs.sec());
    us.setNum(lhs.usec());

    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");
    e.setAttribute("value", s + "." + zero.left(6 - us.length()) + us);
    _node.appendChild(e);
    return e;
  }

  void operator <<= (ACE_INET_Addr& lhs, const QDomNode& node)
  {
    lhs.set(qPrintable(getAttribute(node, QString("value"))));
  }

  QDomElement operator >>= (const ACE_INET_Addr& lhs, QDomNode& _node)
  {
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document.createElement("parameter");
    e.setAttribute("value", QString(lhs.get_host_addr()));
    _node.appendChild(e);
    return e;
  }

  void operator <<= (ACE_Sched_Params& _lhs, const QDomNode& _node)
  {
    if (!_node.isNull()) {
      QDomNode n = _node.firstChild();
      while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
          QDomAttr a = e.attributeNode("name");
          if (!a.isNull()) {
            QString i = a.value();
            if (i == "Policy") {
              int p;
              p <<= n;
              _lhs.policy(p);
            }
            else if (i == "Priority") {
              int p;
              p <<= n;
              _lhs.priority(p);
            }
            else if (i == "Quantum") {
              ACE_Time_Value t;
              t <<= n;
              _lhs.quantum(t);
            }
          }
        }
        n = n.nextSibling();
      }
    }
  }

  QDomElement operator >>= (const ACE_Sched_Params& lhs, QDomNode& _node)
  {
    QDomDocument document = _node.ownerDocument();
    QDomElement e = document. createElement("parameter");

    QDomElement f;
    f = (lhs.policy() >>= e);
    f.setAttribute("name", "Policy");
    f = (lhs.priority() >>= e);
    f.setAttribute("name", "Priority");
    f = (lhs.quantum() >>= e);
    f.setAttribute("name", "Quantum");

    _node.appendChild(e);
    return e;
  }
}
