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
#include "EventC.h"

#include "miro/Server.h"
#include "miro/StructuredPushConsumer.h"
#include "miro/Log.h"
#include "miro/TimeHelper.h"

#include <orbsvcs/Time_Utilities.h>

#include <tao/ORB.h>
#include <tao/AnyTypeCode/TypeCode.h>
#include <tao/Typecode_typesC.h>
#include <tao/DynamicAny/DynamicAny.h>


#include <ace/Get_Opt.h>

#include <iostream>
#include <cmath>

using namespace std;

string channel_name = "NotifyEventChannel";
bool verbose = false;

bool typeName = false;

struct Indent { 
  int depth;
  int stepSize;
  char fill;

  Indent() : 
    depth(0),
    stepSize(2),
    fill(' ') 
  {}

  Indent& operator ++() { ++depth; return *this; }
  Indent& operator --() { --depth; return *this; }
};

Indent indent;

std::ostream& operator << (std::ostream& ostr, Indent const& rhs) {
  for (int i = 0; i < rhs.depth * rhs.stepSize; ++i) {
    ostr << rhs.fill;
  }
  return ostr;
}

int 
parse_args(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "c:tv?");
  
  while ((c = get_opts()) != -1) {
    switch (c) {
    case 'c':
      channel_name = get_opts.optarg;
      break;
    case 't':
      typeName = true;
      break;
    case 'v':
      verbose = true;
      break;
    case '?':
    default:
      rc = 1;
    }
  }

  if (rc == 1) {
      cerr << "usage: " << argv[0] << "[-c channel_name] [-v?]" << endl
	   << "  -c <channel name> name of the event channel (default: NotifyEventChannel)" << endl
	   << "  -t print type names" << endl
	   << "  -v verbose mode" << endl
	   << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    cout<< "channel name: " << channel_name << endl;
  }

  return rc;
}

struct ParseAny
{
  static void parseTC(CORBA::TypeCode_ptr _tc, DynamicAny::DynAny_ptr _dynAny);
  static void parseDynAny(DynamicAny::DynAny_ptr _dynAny);
};

void 
ParseAny::parseTC(CORBA::TypeCode_ptr _tc, DynamicAny::DynAny_ptr _dynAny)
{
  switch (_tc->kind())
  {
  case CORBA::tk_null:
    // empty any
    if (typeName)
      cout <<"tk null" << endl;
    break;
  case CORBA::tk_void:
    // unknown, yet
    if (typeName)
      cout << "tk void" << endl;
    break;
  case CORBA::tk_short:
    if (typeName)
      cout << "tk short: ";
    cout << _dynAny->get_short() << endl;
    break;
  case CORBA::tk_long:
    if (typeName)
      cout << "tk long: ";
    cout << _dynAny->get_long() << endl;
    break;
  case CORBA::tk_ushort:
    if (typeName)
      cout << "tk ushort: ";
    cout << _dynAny->get_ushort() << endl;
    break;
  case CORBA::tk_ulong:
    if (typeName)
      cout << "tk ulong: ";
    cout << _dynAny->get_ulong() << endl;
    break;
  case CORBA::tk_float:
    if (typeName)
      cout << "tk float: ";
    cout  << _dynAny->get_float() << endl;
    break;
  case CORBA::tk_double:
    if (typeName)
      cout << "tk double: ";
    cout << _dynAny->get_double() << endl;
    break;
  case CORBA::tk_boolean:
    if (typeName)
      cout << "tk boolean: ";
    cout << _dynAny->get_boolean() << endl;
    break;
  case CORBA::tk_char:
    if (typeName)
      cout << "tk char: ";
    cout << _dynAny->get_char() << endl;
    break;
  case CORBA::tk_octet:
    if (typeName)
      cout << "tk octet: ";
    cout << (int)_dynAny->get_octet() << endl;
    break;
  case CORBA::tk_string:
    {
      CORBA::String_var s = _dynAny->get_string();
      if (typeName)
	cout << "tk string: ";
      cout << s << endl;
      break;
    }
  case CORBA::tk_longlong:
    {
      long long int i =  _dynAny->get_longlong();
      if (typeName)
	cout << "tk longlong: ";
      cout << i << endl;
    }
    break;
  case CORBA::tk_ulonglong:
    {
      unsigned long long int i =  _dynAny->get_ulonglong();
      if (typeName)
	cout << "tk ulonglong: ";
      cout << i << endl;
    }
    break;
  case CORBA::tk_longdouble:
    {
      //      long double d = _dynAny->get_longdouble();
      if (typeName)
	cout << "tk longdouble" << endl;
    }
    break;
  case CORBA::tk_except:
    {
      CORBA::String_var name = _tc->name();
      CORBA::String_var id = _tc->id();
      if (typeName)
	cout << "tk except" << endl
	     << " id: " << id << endl;

      cout << "[exception " << name << "] " << endl;

      DynamicAny::DynStruct_var ds =
	DynamicAny::DynStruct::_narrow(_dynAny);

      DynamicAny::NameDynAnyPairSeq_var members =
	ds->get_members_as_dyn_any();
      ++indent;
      for (CORBA::ULong i = 0; i < ds->component_count(); ++i) {
	DynamicAny::DynAny_var member = ds->current_component();
	CORBA::String_var name = ds->current_member_name();
	cout << indent << name << endl;
	parseDynAny(member);
	ds->next();
      }
      --indent;
    }
    break;
  case CORBA::tk_struct:
    {
      CORBA::String_var name = _tc->name();
      CORBA::String_var id = _tc->id();
      cout << "[struct " << name << "] " << endl;

      if (typeName)
	cout << "tk struct" << endl
	     << " id: " << id << endl;
      

      DynamicAny::DynStruct_var ds =
	DynamicAny::DynStruct::_narrow(_dynAny);

      ++indent;
      for (CORBA::ULong i = 0; i < ds->component_count(); ++i) {
	DynamicAny::DynAny_var member = ds->current_component();
	CORBA::String_var name = ds->current_member_name();
	
	cout << indent << name << " ";
	parseDynAny(member);
	ds->next();
      }
      --indent;
    }
    break;
  case CORBA::tk_enum:
    {
      CORBA::String_var name = _tc->name();
      CORBA::String_var id = _tc->id();
      cout << "[enum " << name << "] ";

      if (typeName)
	cout << "tk enum" << endl
	     << " name: " << name << endl
	     << " id: " << id << endl;

      if (typeName) {
	cout << " values: ";
	for (CORBA::ULong i = 0; i < _tc->member_count(); ++ i) {
	  CORBA::String_var member = _tc->member_name(i);
	  cout << member << " ";
	}
	cout << endl;
      }

      DynamicAny::DynEnum_var de =
	DynamicAny::DynEnum::_narrow(_dynAny);
      CORBA::String_var val(de->get_as_string());
      cout << val << endl;
    }
    break;
  case CORBA::tk_sequence:
    {
      cout << "[sequence length " << _dynAny->component_count() << "] " << endl;

      if (typeName)
	cout << "tk sequence" << endl;
   
      DynamicAny::DynSequence_var ds =
	DynamicAny::DynSequence::_narrow(_dynAny);
      DynamicAny::DynAnySeq_var members =
	ds->get_elements_as_dyn_any();

      ++indent;
      for (CORBA::ULong i = 0; i < members->length(); ++i) {
	parseDynAny(members[i]);
      }
      --indent;
    }
    break;
  case CORBA::tk_array:
    {
      cout << "[array length " << _tc->length() << "] " << endl;

      if (typeName)
	cout << "tk array" << endl;
   
      for (CORBA::ULong i = 0; i < _dynAny->component_count(); ++i) {
	DynamicAny::DynAny_var member =
	  _dynAny->current_component();
	cout << "  [" << i << "]";
	parseDynAny(member);
	_dynAny->next();
      }
    }
    break;
  case CORBA::tk_any:
    {
      if (typeName)
	cout << "tk any" << endl;

      Miro::Client client;
      CORBA::Any_var any = _dynAny->get_any();
      DynamicAny::DynAnyFactory_var daf =
	client.resolveInit<DynamicAny::DynAnyFactory>("DynAnyFactory");
      DynamicAny::DynAny_var da = daf->create_dyn_any(*any);
      ++indent;
      parseDynAny(da);
      ++indent;
      da->destroy();
    }
    break;
  case CORBA::tk_alias:
    {
      CORBA::String_var name = _tc->name();
      CORBA::String_var id = _tc->id();
      if (typeName)
	cout << "tk alias" << endl
	     << " name: " << name << endl
	     << " id: " << id << endl;

      CORBA::TypeCode_var tc = _tc->content_type();
      parseTC(tc, _dynAny);
    }
    break;
  case CORBA::tk_objref:
    {
      Miro::Client client;
      CORBA::ORB_var orb = client.orb();
      CORBA::String_var id = _tc->id();
      cout << "Object reference (" << id << ")" << endl;
      CORBA::Object_var obj = _dynAny->get_reference();
      CORBA::String_var str_ref = "<NULL>"; 
      if (!CORBA::is_nil(obj))
	str_ref = orb->object_to_string(obj);
      cout << str_ref << endl;
    }
    break;
  case CORBA::tk_wchar:
  case CORBA::tk_wstring:
  case CORBA::tk_fixed:
  case CORBA::tk_TypeCode:
  case CORBA::tk_Principal:
  case CORBA::tk_union:
  default:
    cout << "other kind" << endl;
  }
}

void
ParseAny::parseDynAny(DynamicAny::DynAny_ptr _dynAny)
{
  CORBA::TypeCode_var tc = _dynAny->type();

  parseTC(tc, _dynAny);
}

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  try {
    Miro::Client client(argc, argv);

    if (parse_args(argc, argv) != 0) {
      return 1;
    }

    CosNotifyChannelAdmin::EventChannel_var ec =
      client.resolveName<CosNotifyChannelAdmin::EventChannel>(channel_name);

    CosNotifyChannelAdmin::AdminIDSeq_var admins =
      ec->get_all_consumeradmins();

    cout << "Consumer admins: " << admins->length() << endl;
    for (CORBA::ULong i = 0; i < admins->length(); ++i) {

      cout << "Consumer admin id: " << admins[i] << endl;

      CosNotifyChannelAdmin::ConsumerAdmin_var admin =
	ec->get_consumeradmin(admins[i]);
      CosNotification::QoSProperties_var properties =
	admin->get_qos();

      cout << "Admin QoS properties: " << properties->length() << endl;
      ++indent;
      DynamicAny::DynAnyFactory_var daf =
	client.resolveInit<DynamicAny::DynAnyFactory>("DynAnyFactory");
      for (CORBA::ULong j = 0; j < properties->length(); ++j) {
	cout << indent << "Admin QoS property name: " << properties[j].name
	     << " value: " << " ";
	
	DynamicAny::DynAny_var da = daf->create_dyn_any(properties[j].value);

	ParseAny::parseDynAny(da);
	da->destroy();
      }
      --indent;
    }
  }
  catch (CORBA::Exception const& e) {
    cerr << "CORBA Exception: " << e << endl;
  }
  return 0;
}
