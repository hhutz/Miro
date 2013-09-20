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
#include <tao/DynamicInterface/Request.h> // This has to be first with gcc 4.1

#include "CORBA_TypesC.h"

#include "miro/Client.h"
#include "miro/Log.h"
#include "miro/AnyPrinter.h"

#include <ace/Get_Opt.h>

#include <iostream>

using namespace XMP;
using namespace std;

bool verbose;
bool printName = true;
bool printType = true;
bool printLength = false;
bool printEnums = false;
bool printTc = false;
bool printId = false;

int
parseArgs(int& argc, char* argv[])
{
  int rc = 0;
  int c;

  ACE_Get_Opt get_opts (argc, argv, "ntlsdiNTLSDIv?");

  while ((c = get_opts()) != -1) {
    switch (c) {
      case 'n':
        printName = true;
        break;
      case 'N':
        printName = false;
        break;
      case 't':
        printType = true;
        break;
      case 'T':
        printType = false;
        break;
      case 'l':
        printLength = true;
        break;
      case 'L':
        printLength = false;
        break;
      case 's':
        printEnums = true;
        break;
      case 'S':
        printEnums = false;
        break;
      case 'd':
        printTc = true;
        break;
      case 'D':
        printTc = false;
        break;
      case 'i':
        printId = true;
        break;
      case 'I':
        printId = false;
        break;
      case 'v':
        verbose = true;
        break;
      case '?':
      default:
        rc = 1;
    }
  }

  if (rc) {
    cerr << "usage: " << argv[0] << "-e eventName [-c channelName] [-v?]" << endl
    << "  -n|N print field names <true|false> (default is true)" << endl
    << "  -t|T print field names <true|false> (default is true)" << endl
    << "  -l|L print length of arrays and sequences <true|false> (default is false)" << endl
    << "  -s|S print enumeration sets <true|false> (default is false)" << endl
    << "  -d|D print type codes <true|false> (default is false)" << endl
    << "  -i|I print type id of user types <true|false> (default is false)" << endl
    << "  -v verbose mode" << endl
    << "  -? help: emit this text and stop" << endl;
  }

  if (verbose) {
    cerr << "print names: " << printName << endl
    << "print types: " << printType << endl
    << "print length: " << printLength << endl
    << "print enums: " << printEnums << endl
    << "print type codes: " << printTc << endl
    << "print IDs: " << printId << endl;
  }

  return rc;
}

CORBA::Request_ptr createRequest(GenericCmd const& cmd)
{
  CORBA::Request_var req = cmd.reference->_request(cmd.methodName);

  for (CORBA::ULong i = 0; i < cmd.parameters.length(); ++i) {
    switch (cmd.parameters[i].direction) {
      case PARAM_IN: {
        CORBA::Any& a = req->add_in_arg(cmd.parameters[i].name);
        a = cmd.parameters[i].value;
      }
      break;
      case PARAM_OUT: {
        CORBA::Any& a = req->add_out_arg(cmd.parameters[i].name);
        a = cmd.parameters[i].value;
      }
      break;
      case PARAM_INOUT: {
        CORBA::Any& a = req->add_inout_arg(cmd.parameters[i].name);
        a = cmd.parameters[i].value;
      }
      break;
      default:
        MIRO_ASSERT(false);
    }
  }

  req->set_return_type(cmd.returnType);

  CORBA::ExceptionList_ptr e = req->exceptions();
  for (CORBA::ULong i = 0; i < cmd.exceptions.length(); ++i) {
    e->add(cmd.exceptions[i]);
  }

  return req._retn();
}

void
reportResult(CORBA::Request_ptr request)
{
  Miro::Client client;
  Miro::AnyPrinter printer(printName,
                           printType,
                           printLength,
                           printEnums,
                           printTc,
                           printId);

  CORBA::NVList_ptr arguments = request->arguments();
  for (CORBA::ULong i = 0; i < arguments->count(); ++i) {
    cout << "argument name: " << arguments->item(i)->name() << endl;
    CORBA::Any_ptr any = arguments->item(i)->value();

    printer.print(cout, *any);
    cout << endl;
  }

  cout << "return value: " << endl;
  CORBA::Any any = request->return_value();
  printer.print(cout, any);
  cout << endl;
}

int main(int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  Miro::Client client(argc, argv);
  if (parseArgs(argc, argv) == -1)
    return 1;

  GenericCmd cmd;
  cmd.objectName = CORBA::string_dup("Basic");
  cmd.reference = client.resolveName<CORBA::Object>("Basic");

  //
  // call s0
  //
  cmd.methodName = CORBA::string_dup("s0");
  cmd.returnType = CORBA::_tc_void;

  CORBA::Request_var req0 = createRequest(cmd);
  req0->invoke();

  reportResult(req0);

  //
  // call s1
  //
  cmd.methodName = CORBA::string_dup("s1");
  cmd.parameters.length(3);
  cmd.parameters[0].name = CORBA::string_dup("p1");
  cmd.parameters[0].direction = PARAM_IN;
  cmd.parameters[0].value <<= CORBA::Long(2);
  cmd.parameters[1].name = CORBA::string_dup("p1");
  cmd.parameters[1].direction = PARAM_OUT;
  cmd.parameters[1].value <<= CORBA::Long();
  cmd.parameters[2].name = CORBA::string_dup("p1");
  cmd.parameters[2].direction = PARAM_INOUT;
  cmd.parameters[2].value <<= CORBA::Long(4);
  cmd.returnType = CORBA::_tc_long;

  CORBA::Request_var req1 = createRequest(cmd);
  req1->invoke();
  reportResult(req1);

  //
  // call s2
  //
  cmd.methodName = CORBA::string_dup("s2");
  cmd.parameters.length(3);
  cmd.parameters[0].value <<= CORBA::string_dup("hello");
  cmd.parameters[1].value <<= CORBA::string_dup("");
  cmd.parameters[2].value <<= CORBA::string_dup("world");
  cmd.returnType = CORBA::_tc_string;

  CORBA::Request_var req2 = createRequest(cmd);
  req2->invoke();
  reportResult(req2);

  //
  // call e0
  //
  cmd.methodName = CORBA::string_dup("e0");
  cmd.parameters.length(0);
  cmd.exceptions.length(1);
  cmd.exceptions[0] = _tc_EFoo;
  cmd.returnType = CORBA::_tc_void;

  CORBA::Request_var reqE = createRequest(cmd);
  try {
    reqE->invoke();
  }
  catch (XMP::EFoo const& e) {
    cout << "caught exception " << e.what << endl;
  }
  catch (CORBA::Exception const& e) {
    cout << "caught corba exception " << e << endl;
  }
  reportResult(reqE);

  return 0;
}
