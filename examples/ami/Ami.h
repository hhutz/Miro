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
#ifndef XMP_Ami_h
#define XMP_Ami_h

#include "miro/DummyCORBA.h"

namespace POA_XMP
{ 
  class Ami;
  class AMI_AmiHandler;
}

namespace XMP
{

  using POA_XMP::Ami;
  typedef Ami * Ami_ptr;
  typedef Ami * Ami_var;
  
  using POA_XMP::AMI_AmiHandler;
  typedef AMI_AmiHandler * AMI_AmiHandler_ptr; 
  typedef AMI_AmiHandler * AMI_AmiHandler_var;

  class  EAmi : public ::CORBA::UserException
  {
  public:
    char * what;
    EAmi () : what(NULL) {}
    //    EAmi (const EAmi &);
    //    ~EAmi ();
    //    EAmi &operator= (const EAmi &);
    
    EAmi (char * _tao_what) : what(_tao_what) {}
  };
} 

namespace POA_XMP
{ 
  class  Ami : public virtual PortableServer::ServantBase
  {
  public:
    virtual void s0(CORBA::Long p1) throw() = 0;
    virtual void sendc_s0(XMP::AMI_AmiHandler_ptr ami_handler, CORBA::Long p1) throw();
    virtual CORBA::Long s1() throw() = 0;    
    virtual void sendc_s1 (XMP::AMI_AmiHandler_ptr ami_handler) throw();     
    virtual void s2 () throw (XMP::EAmi) = 0;
    virtual void sendc_s2 (XMP::AMI_AmiHandler_ptr ami_handler) throw();
  };

  class  AMI_AmiHandler : public virtual POA_Messaging::ReplyHandler
  {
  public:
    typedef XMP::AMI_AmiHandler_ptr _ptr_type;
    typedef XMP::AMI_AmiHandler_var _var_type;
    typedef XMP::AMI_AmiHandler_ptr _stub_ptr_type;
    typedef XMP::AMI_AmiHandler_var _stub_var_type;

    AMI_AmiHandler * _this() { return this; }

    static XMP::AMI_AmiHandler_ptr _nil () { return static_cast<XMP::AMI_AmiHandler_ptr> (0); }
    
    virtual void s0 () throw() = 0;
    virtual void s0_excep (Messaging::ExceptionHolder * excep_holder) throw() = 0;
    virtual void s1 (CORBA::Long ami_return_val) throw() = 0;
    virtual void s1_excep (Messaging::ExceptionHolder * excep_holder) throw() = 0;
    virtual void s2 () throw (XMP::EAmi) = 0;
    virtual void s2_excep (Messaging::ExceptionHolder * excep_holder) throw () = 0;
  };
} // namespace POA_XMP

#endif // XMP_Ami_h


