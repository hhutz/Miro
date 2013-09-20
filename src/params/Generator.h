// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2005
// Department of Neuroinformatics, University of Ulm, Germany
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
// $Id$
//
#ifndef Miro_CFG_Generator_h
#define Miro_CFG_Generator_h

#include "Type.h"

#include <vector>
#include <map>
#include <set>

namespace Miro
{
  namespace CFG {
    typedef std::multimap<QString, Type> GroupMap;
    typedef std::vector<QString> QStringVector;

    class Generator;
    std::ostream& operator << (std::ostream& _ostr, Generator const& _rhs);

    class Generator
    {
    public:

      Generator();

      void addinclude(const QString& _include);
      void addInclude(const QString& _Include);
      void addSrcInclude(const QString& _Include);

      void addForwardDeclaration(const QString& _decl);
      void addLocalForwardDeclaration(const QString& _decl);

      void setBaseName(const QString& _baseName);
      void setHeaderExtension(const QString& _extensionName);
#if JSONCPP_FOUND
      void setUseJson(bool _useJson) { useJson_ = _useJson; }
#endif
      void addNamespace(const QString& _namespace);
      void clearNamespace();

      void addType(const QString& _group, const Type& _class);

      void generateHeader(std::ostream& ostr, const QString& exportDirective = QString::null) const;
      void generateSource(std::ostream& ostr) const;

      QStringVector groups() const;
      void getGroupedTypes(const QString& _group,
                           GroupMap::const_iterator& _first,
                           GroupMap::const_iterator& _last) const;
      const Type * getType(const QString& _name) const;


      ParameterVector getFullParameterSet(Type const& _type) const;
      ParameterVector getFullStaticConstParameterSet(Type const& _type) const;

      bool isDerivedType(Type const& _type, Type const& _ancestor) const;
      QStringVector getDescendants(Type const& _type) const;

    protected:
      typedef std::set<QString> QStringSet;
      typedef std::vector<Type> TypeVector;

      QStringSet include_;
      QStringSet Include_;
      QStringSet srcInclude_;

      QStringSet forwardDeclaration_;
      QStringSet localForwardDeclaration_;

      QString fileName_;
      QString extensionName_;
      QStringVector namespace_;
#if JSONCPP_FOUND
      bool useJson_;
#endif

      TypeVector type_;
      GroupMap groups_;

      // static data
      static const unsigned long STEP = 2;

      friend std::ostream& operator << (std::ostream& _ostr, Generator const& _rhs);
    };
  }
}
#endif // Miro_CFG_Generator_h
