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
#ifndef miro_AnyPrinter_h
#define miro_AnyPrinter_h

#include "miro_Export.h"

#include <tao/DynamicAny/DynamicAny.h>

#include <string>
#include <iostream>

namespace CORBA
{
  class Any;
  class TypeCode;

  typedef TypeCode * TypeCode_ptr;
}

namespace Miro
{
  class miro_Export AnyPrinter
  {
  public:
    AnyPrinter(bool printName = true,
               bool printType = true,
               bool printLength = false,
               bool printEnums = false,
               bool printTc = false,
               bool printId = false,
               unsigned int maxSeqLength = 100);

    void print(std::ostream& ostr, CORBA::Any const& _any);

    bool printTc() const {
      return m_printTc;
    }
    bool printId() const {
      return m_printId;
    }
    bool printType() const {
      return m_printType;
    }
    bool printName() const {
      return m_printName;
    }
    bool printEnums() const {
      return m_printEnums;
    }
    bool printLength() const {
      return m_printLength;
    }

    unsigned int maxSeqLength() const {
      return m_maxSeqLength;
    }

    void printTc(bool f) {
      m_printTc = f;
    }
    void printId(bool f) {
      m_printId = f;
    }
    void printType(bool f) {
      m_printType = f;
    }
    void printName(bool f) {
      m_printName = f;
    }
    void printEnums(bool f) {
      m_printEnums = f;
    }
    void printLength(bool f) {
      m_printLength = f;
    }

    void maxSeqLength(unsigned int len) {
      m_maxSeqLength = len;
    }

    std::string separator() const {
      return m_separator;
    }
    std::string tcSeparator() const {
      return m_tcSeparator;
    }
    std::string idSeparator() const {
      return m_idSeparator;
    }
    std::string typeSeparator() const {
      return m_typeSeparator;
    }
    std::string nameSeparator() const {
      return m_nameSeparator;
    }
    std::string compoundSeparator() const {
      return m_compoundSeparator;
    }

    void separator(std::string const& s) {
      m_separator = s;
    }
    void tcSeparator(std::string const& s) {
      m_tcSeparator = s;
    }
    void idSeparator(std::string const& s) {
      m_idSeparator = s;
    }
    void typeSeparator(std::string const& s) {
      m_typeSeparator = s;
    }
    void nameSeparator(std::string const& s) {
      m_nameSeparator = s;
    }
    void compoundSeparator(std::string const& s) {
      m_compoundSeparator = s;
    }

  protected:
    void printTcInfo(std::ostream& ostr, CORBA::TypeCode_ptr tc, char const * name) const ;
    void printTcId(std::ostream& ostr, CORBA::TypeCode_ptr tc, char const * name) const ;

    template<typename T>
    void
    printType(std::ostream& ostr, CORBA::Any const& _any) {
      T value;
      _any >>= value;
      ostr << value;
    }

  private:
    DynamicAny::DynAnyFactory_var m_daf;

    int m_indent;

    bool m_printTc;
    bool m_printId;
    bool m_printType;
    bool m_printName;
    bool m_printEnums;
    bool m_printLength;

    unsigned int m_maxSeqLength;

    std::string m_separator;
    std::string m_tcSeparator;
    std::string m_idSeparator;
    std::string m_typeSeparator;
    std::string m_nameSeparator;
    std::string m_compoundSeparator;
  };
}

#endif // miro_AnyPrinter_h
