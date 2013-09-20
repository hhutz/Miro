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
#include "AnyPrinter.h"

#include "Client.h"

#include <tao/ORB.h>
#include <tao/AnyTypeCode/TypeCode.h>
#include <tao/Typecode_typesC.h>
#include <tao/DynamicAny/DynamicAny.h>

#include <iostream>

namespace
{
  char const * tkNames[] = {
    "tk_null",
    "tk_void",
    "tk_short",
    "tk_long",
    "tk_ushort",
    "tk_ulong",
    "tk_float",
    "tk_double",
    "tk_boolean",
    "tk_char",
    "tk_octet",
    "tk_any",
    "tk_TypeCode",
    "tk_Principal",
    "tk_objref",
    "tk_struct",
    "tk_union",
    "tk_enum",
    "tk_string",
    "tk_sequence",
    "tk_array",
    "tk_alias",
    "tk_except",
    "tk_longlong",
    "tk_ulonglong",
    "tk_longdouble",
    "tk_wchar",
    "tk_wstring",
    "tk_fixed",
    "tk_value",
    "tk_value_box",
    "tk_native",
    "tk_abstract_interface",
    "tk_local_interface",
    "tk_component",
    "tk_home",
    "tk_event"
  };
  int TK_LEN = sizeof(tkNames) / sizeof(char const *);
}

namespace Miro
{
  using namespace std;

  AnyPrinter::AnyPrinter(bool printName,
                         bool printType,
                         bool printLength,
                         bool printEnums,
                         bool printTc,
                         bool printId,
                         unsigned int maxSeqLength) :
      m_indent(0),
      m_printTc(printTc),
      m_printId(printId),
      m_printType(printType),
      m_printName(printName),
      m_printEnums(printEnums),
      m_printLength(printLength),
      m_maxSeqLength(maxSeqLength),
      m_separator(", "),
      m_tcSeparator(": "),
      m_idSeparator(" - "),
      m_typeSeparator(" "),
      m_nameSeparator("="),
      m_compoundSeparator("")
  {
    Miro::Client client;
    m_daf = client.resolveInit<DynamicAny::DynAnyFactory>("DynAnyFactory");
  }

  void
  AnyPrinter::print(std::ostream& ostr, CORBA::Any const& _any)
  {
    CORBA::TypeCode_var tc = _any.type();

    switch (tc->kind()) {
      case CORBA::tk_null:
        // empty any
        if (m_printTc)
          ostr << "<tk null>";
        ostr << "NIL";
        break;

      case CORBA::tk_void:
        if (m_printTc)
          ostr << "<tk void>";
        ostr << "VOID";
        break;

      case CORBA::tk_short:
        if (m_printTc)
          ostr << "<tk short>";
        printType<CORBA::Short>(ostr, _any);
        break;

      case CORBA::tk_ushort:
        if (m_printTc)
          ostr << "<tk ushort>";
        printType<CORBA::UShort>(ostr, _any);
        break;

      case CORBA::tk_long:
        if (m_printTc)
          ostr << "<tk long>";
        printType<CORBA::Long>(ostr, _any);
        break;

      case CORBA::tk_ulong:
        ostr << "<tk ulong>";
        printType<CORBA::ULong>(ostr, _any);
        break;

      case CORBA::tk_float:
        if (m_printTc)
          ostr << "<tk float>";
        printType<CORBA::Float>(ostr, _any);
        break;

      case CORBA::tk_double:
        if (m_printTc)
          ostr << "<tk double>";
        printType<CORBA::Double>(ostr, _any);
        break;

      case CORBA::tk_boolean: {
        CORBA::Boolean value;
        _any >>= CORBA::Any::to_boolean(value);
        if (m_printTc)
          ostr << "<tk boolean>";
        ostr << value << m_separator;
      }
      break;
      case CORBA::tk_char: {
        CORBA::Char value;
        _any >>= CORBA::Any::to_char(value);
        if (m_printTc)
          ostr << "<tk char>";
        ostr << value << m_separator;
      }
      break;
      case CORBA::tk_octet: {
        CORBA::Octet value;
        _any >>= CORBA::Any::to_octet(value);
        if (m_printTc)
          ostr << "<tk octet>";
        if (isprint(value)) {
          ostr << value << m_separator;
        }
        else {
          ostr << "0x" << hex << (int)value << dec << m_separator;
        }
      }
      break;

      case CORBA::tk_string:
        if (m_printTc)
          ostr << "<tk string>";
        printType<char const *>(ostr, _any);
        break;

      case CORBA::tk_longlong:
        if (m_printTc)
          ostr << "<tk longlong>";
        printType<CORBA::LongLong>(ostr, _any);
        break;

      case CORBA::tk_ulonglong:
        if (m_printTc)
          ostr << "<tk ulonglong>";
        printType<CORBA::ULongLong>(ostr, _any);
        break;

      case CORBA::tk_longdouble: {
        if (m_printTc)
          ostr << "<tk longdouble>";
        ostr << "(LONG DOUBLE)";
      }
      break;

      case CORBA::tk_except: {
        printTcInfo(ostr, tc, "tk except");

        DynamicAny::DynAny_var da = m_daf->create_dyn_any(_any);
        DynamicAny::DynStruct_var ds = DynamicAny::DynStruct::_narrow(da);
        DynamicAny::NameValuePairSeq_var members = ds->get_members();

        if (m_printLength)
          ostr << "(" << members->length() << "):";
        ostr << "{" << m_compoundSeparator;
        ++m_indent;

        for (CORBA::ULong i = 0; i < members->length(); ++i) {
          if (m_printName)
            ostr << members[i].id << m_nameSeparator;
          print(ostr, members[i].value);
          if (i != members->length() - 1)
            ostr << m_separator;
        }

        da->destroy();

        ostr << m_compoundSeparator << "}";
        --m_indent;
      }
      break;

      case CORBA::tk_struct: {
        printTcInfo(ostr, tc, "tk_struct");

        DynamicAny::DynAny_var da = m_daf->create_dyn_any(_any);
        DynamicAny::DynStruct_var ds = DynamicAny::DynStruct::_narrow(da);
        DynamicAny::NameValuePairSeq_var members = ds->get_members();

        if (m_printLength)
          ostr << "(" << members->length() << "):";
        ostr << "{" << m_compoundSeparator;
        ++m_indent;

        for (CORBA::ULong i = 0; i < members->length(); ++i) {
          if (m_printName)
            ostr << members[i].id << m_nameSeparator;
          print(ostr, members[i].value);
          if (i != members->length() - 1)
            ostr << m_separator;
        }

        da->destroy();

        ostr << m_compoundSeparator << "}";
        --m_indent;
      }
      break;

      case CORBA::tk_enum: {
        printTcInfo(ostr, tc, "tk enum");

        if (m_printEnums) {
          ostr << "[";
          for (CORBA::ULong i = 0; i < tc->member_count(); ++ i) {
            CORBA::String_var member = tc->member_name(i);
            ostr << member;
            if (i != tc->member_count() - 1)
              ostr << ", ";
          }
          ostr << "]:";
        }

        DynamicAny::DynAny_var da = m_daf->create_dyn_any(_any);
        DynamicAny::DynEnum_var de = DynamicAny::DynEnum::_narrow(da);
        CORBA::String_var val(de->get_as_string());
        ostr << val;

        da->destroy();
      }
      break;

      case CORBA::tk_sequence: {
        if (m_printTc) {
          ostr << "<tk sequence>";
        }

        DynamicAny::DynAny_var da = m_daf->create_dyn_any(_any);
        DynamicAny::DynSequence_var ds = DynamicAny::DynSequence::_narrow(da);
        DynamicAny::AnySeq_var members = ds->get_elements();

        if (m_printLength ||
                    members->length() >= m_maxSeqLength)
          ostr << "(" << members->length() << "):";
        ostr << "[" << m_compoundSeparator;
        ++m_indent;

        if (members->length() < m_maxSeqLength) {
          for (CORBA::ULong i = 0; i < members->length(); ++i) {
            print(ostr, members[i]);
            if (i != members->length() - 1)
              ostr << m_separator;
          }
        }
        else {
          ostr << "...";
        }

        da->destroy();

        ostr << m_compoundSeparator << "]";
        --m_indent;
      }
      break;

      case CORBA::tk_array: {
        if (m_printTc) {
          ostr << "<tk array>";
        }

        DynamicAny::DynAny_var da = m_daf->create_dyn_any(_any);
        DynamicAny::DynArray_var ds = DynamicAny::DynArray::_narrow(da);
        DynamicAny::AnySeq_var members = ds->get_elements();

        if (m_printLength)
          ostr << "(" << members->length() << "):";
        ostr << "[" << m_compoundSeparator;
        ++m_indent;

        for (CORBA::ULong i = 0; i < members->length(); ++i) {
          print(ostr, members[i]);
          if (i != members->length() - 1)
            ostr << m_separator;
        }

        da->destroy();

        ostr << m_compoundSeparator << "]";
        --m_indent;
      }
      break;

      case CORBA::tk_any: {
        if (m_printTc)
          ostr << "<tk any>";
        ostr << "{" << m_compoundSeparator;
        ++m_indent;
        CORBA::Any const * any;
        _any >>= any;
        this->print(ostr, *any);
        ostr << m_compoundSeparator << "}";
        --m_indent;
      }
      break;

      case CORBA::tk_alias: {
        printTcInfo(ostr, tc, "tk alias");

        if (m_printTc || m_printId || m_printName)
          ostr << "==>" << flush;

        CORBA::TypeCode_var t = tc->content_type();
        CORBA::Any any = _any;
        any.type(t);
        print(ostr, any);
      }
      break;

      case CORBA::tk_objref: {
        printTcId(ostr, tc, "tk objref");

        CORBA::Object * obj;
        _any >>= CORBA::Any::to_object(obj);
        CORBA::String_var str_ref = "NULL";
        if (!CORBA::is_nil(obj)) {
          Miro::Client client;
          CORBA::ORB_var orb = client.orb();
          str_ref = orb->object_to_string(obj);
        }
        ostr << str_ref;
      }
      break;

      case CORBA::tk_TypeCode: {
        if (m_printTc) {
          ostr << "<tk TypeCode>" << flush;
        }
        CORBA::TypeCode_ptr typeCode;
        _any >>= typeCode;

        switch (typeCode->kind()) {
          case CORBA::tk_objref:
          case CORBA::tk_struct:
          case CORBA::tk_union:
          case CORBA::tk_enum:
          case CORBA::tk_alias:
          case CORBA::tk_except: {
            CORBA::String_var name = typeCode->name();
            ostr << name;
          }
          break;
          default:
            if (typeCode->kind() < TK_LEN)
              ostr << tkNames[typeCode->kind()];
        }
      }
      break;
      case CORBA::tk_wchar:
      case CORBA::tk_wstring:
      case CORBA::tk_fixed:
      case CORBA::tk_Principal:
      case CORBA::tk_union:
        ostr << "<" << tkNames[tc->kind()] << ">";
        break;

      default:
        ostr << "unknown kind";
    }

  }

  void
  AnyPrinter::printTcInfo(ostream& ostr, CORBA::TypeCode_ptr tc, char const * name) const
  {
    if (m_printTc || m_printId || m_printType) {
      ostr << "<";

      if (m_printTc) {
        ostr << name;
        if (m_printId || m_printType)
          ostr << m_tcSeparator;
      }


      if (m_printId) {
        CORBA::String_var id = tc->id();
        ostr << id;
        if (m_printType)
          ostr << m_idSeparator;
      }

      if (m_printType) {
        CORBA::String_var name = tc->name();
        ostr << name;
      }

      ostr << ">";
    }
  }

  void
  AnyPrinter::printTcId(ostream& ostr, CORBA::TypeCode_ptr tc, char const * name) const
  {
    if (m_printTc || m_printId) {
      ostr << "<";

      if (m_printTc) {
        ostr << name;
        if (m_printId)
          ostr << m_tcSeparator;
      }

      if (m_printId) {
        CORBA::String_var id = tc->id();
        ostr << id;
      }

      ostr << ">";
    }
  }
}
