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
#include "AmiImpl.h"
#include "AmiCallbackImpl.h"

#include "miro/Server.h"
#include "miro/Log.h"
#include "miro/AmiHelper.h"

#include <ace/Synch.h>

#include <iostream>

int main (int argc, char * argv[])
{
  Miro::Log::init(argc, argv);
  Miro::Server server(argc, argv);

  // server part
  try {
    std::string name = "AMI";
    XMP::AmiImpl * ami = new XMP::AmiImpl();

    server.activateNamedObject(name, ami);
    server.detach(5);
  }
  catch (CORBA::Exception const& e) {
    MIRO_LOG_OSTR(LL_CRITICAL, "CORBA exception:\n" << e);
  }

  // client part
  try {
    // anything that has a callback is actually a server...
    Miro::Server server(argc, argv);
    XMP::Ami_var ami = server.resolveName<XMP::Ami>("AMI");

    // we just run the ORB in a background thread
    server.detach(3);

    //--------------------------------------------------------------------------
    // calling a method without a return arguement
    {
      //------------------------------------------------------------------------
      // synchronous method call

      std::cout << "synchronous call to s0." << std::endl;
      ami->s0(5);
      std::cout << "s0 returned." << std::endl;
      
      //------------------------------------------------------------------------
      // asynchronous upcall, not caring about the return value
      std::cout << "asynchronous (oneway) call to s." << std::endl;
      ami->sendc_s(NULL);
      std::cout << "s returned." << std::endl;

      //------------------------------------------------------------------------
      // asynchronous call with reply handler

      // the generic AMI helper class
      // instanciated with our generic callback implemenation
      Miro::AmiHelper<XMP::AMI_Ami_s> h;
    
      std::cout << "asynchronous call to s." << std::endl;
      ami->sendc_s(h.handler());
      std::cout << "s returned." << std::endl;
      
      std::cout << "wait for s to finish." << std::endl;
      h.wait();
      // checking return value, in case there was an exception:
      h.rc();
      std::cout << "s finished." << std::endl;


      //------------------------------------------------------------------------
      // asynchronous upcall, not caring about the return value
#ifdef ASDF
      std::cout << "asynchronous (oneway) call to s0." << std::endl;
      ami->sendc_s0(NULL, 4);
      std::cout << "s0 returned." << std::endl;

      //------------------------------------------------------------------------
      // asynchronous call with reply handler

      // the generic AMI helper class
      // instanciated with our generic callback implemenation
      Miro::AmiHelper<XMP::AMI_Ami_s0> h0;
    
      std::cout << "asynchronous call to s0." << std::endl;
      ami->sendc_s0(h0.handler(), 4);
      std::cout << "s0 returned." << std::endl;
      
      std::cout << "wait for s0 to finish." << std::endl;
      h0.wait();
      // checking return value, in case there was an exception:
      h0.rc();
      std::cout << "s0 finished." << std::endl;
#endif
    }
    //--------------------------------------------------------------------------
    // calling a method with one single return arguement
    {
      //------------------------------------------------------------------------
      // synchronous method call

      std::cout << "synchronous call to s1." << std::endl;
      int rc = ami->s1();
      std::cout << "s1 returned:" << rc << std::endl;

      //------------------------------------------------------------------------
      // asynchronous call with reply handler

      // the generic AMI helper class
      Miro::AmiHelper<XMP::AMI_Ami_s1> h1;

      std::cout << "asynchronous call to s1." << std::endl;
      ami->sendc_s1(h1.handler());
      std::cout << "s1 returned." << std::endl;

      std::cout << "wait for s1 finish!" << std::endl;
      h1.wait();
      std::cout << "s1 finished - rc:" << h1.rc() << std::endl;
    
      //------------------------------------------------------------------------
      // asynchronous call with dangling reply handler
   
      Miro::AmiHelper<XMP::AMI_Ami_s1> h1b;
      std::cout << "asynchronous call to s1." << std::endl;
      ami->sendc_s1(h1b.handler());
      std::cout << "s1 returned." << std::endl;
      std::cout << "ooops we forgot towait for completion." << std::endl;
      std::cout << "how nice, that this results no memory leak." << std::endl;
    }

    //--------------------------------------------------------------------------
    // calling a method which throws an exception
    try {
      Miro::AmiHelper<XMP::AMI_Ami_s2> h2;

      std::cout << "asynchronous call to s2." << std::endl;
      ami->sendc_s2(h2.handler());
      std::cout << "s2 returned." << std::endl;
      
      std::cout << "wait for s2 finish!" << std::endl;
      h2.wait();
      std::cout << "s2 finished." << std::endl;
      
      h2.rc();
    }
    catch (XMP::EAmi const& e) {
      std::cout << "EAmi exception: " << e.what << std::endl;
    }

    std::cout << "shuting down server." << std::endl;
    server.shutdown();
    server.wait();
    std::cout << "server down." << std::endl;
  }
  catch (CORBA::Exception const& e) {
    MIRO_LOG_OSTR(LL_CRITICAL, "CORBA exception:\n" << e);
    server.shutdown();
    std::cout << "what's next?" << std::endl;
  }
  std::cout << "cleanup" << std::endl;
  return 0;
}
