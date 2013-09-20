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
#ifndef XmlParse_h
#define XmlParse_h

#include "Exception.h"
#include "miroXml_Export.h"

#include <qdom.h>

#include <string>
#include <vector>
#include <set>


#define EXCEPTION_SUBTYPE(type, parent) \
  class type : public parent \
  { \
    typedef parent Super; \
  public: \
    /*! Default constructor. */ \
    type() throw() : Super() {} \
    /*! Initializing constructor */ \
    type(const std::string& _what) throw() : Super(_what) {} \
  }

namespace Miro
{
  // forward declarations
  class Angle;
  class Text;

  // XML parsing exceptions

  EXCEPTION_SUBTYPE(XmlException, Exception);
  EXCEPTION_SUBTYPE(InvalidXmlException, XmlException);
  EXCEPTION_SUBTYPE(UnnamedParameterException, XmlException);
  EXCEPTION_SUBTYPE(UnknownParameterException, XmlException);

  miroXml_Export void operator <<= (bool& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (char& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (unsigned char& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (short& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (unsigned short& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (int& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (unsigned int& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (long& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (unsigned long& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (float& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (double& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (Angle& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (std::string& lhs, const QDomNode& node);
  miroXml_Export void operator <<= (Text& lhs, const QDomNode& node);

  template<class T>
  void operator <<= (std::vector<T>& lhs, const QDomNode& node)
  {
    lhs.clear();
    QDomNode n = node.firstChild();
    while (!n.isNull()) {
      if (n.isElement()) {
        T t;
        t <<= n;
        lhs.push_back(t);
      }
      n = n.nextSibling();
    }
  }

  template<class T>
  void operator <<= (std::set<T>& lhs, const QDomNode& node)
  {
    lhs.clear();
    QDomNode n = node.firstChild();
    while (!n.isNull()) {
      if (n.isElement()) {
        T t;
        t <<= n;
        lhs.insert(t);
      }
      n = n.nextSibling();
    }
  }

  miroXml_Export QDomElement operator >>= (const bool& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const char& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const unsigned char& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const short& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const unsigned short& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const int& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const unsigned int& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const long& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const unsigned long& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const float& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const double& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const Angle& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const std::string& lhs, QDomNode& node);
  miroXml_Export QDomElement operator >>= (const Text& lhs, QDomNode& node);

  template<class T>
  QDomElement operator >>= (const std::vector<T>& lhs, QDomNode& node)
  {
    QDomDocument d = node.ownerDocument();
    QDomElement e = d.createElement("parameter");
    typename std::vector<T>::const_iterator f, l = lhs.end();
    for (f = lhs.begin(); f != l; ++f) {
      (*f) >>= e;
    }
    node.appendChild(e);
    return e;
  }
  template<class T>
  QDomElement operator >>= (const std::set<T>& lhs, QDomNode& node)
  {
    QDomDocument d = node.ownerDocument();
    QDomElement e = d.createElement("parameter");
    typename std::set<T>::const_iterator f, l = lhs.end();
    for (f = lhs.begin(); f != l; ++f) {
      (*f) >>= e;
    }
    node.appendChild(e);
    return e;
  }
}


#endif // xmlParse_h
