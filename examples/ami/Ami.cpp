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
#include "Ami.h"

#ifdef MIRO_NO_CORBA
#  include "miro/ServerWorker.h"

namespace 
{
  class AMI_sendc_Ami_s0 : public ACE_Event_Handler
  {
  public:
    AMI_sendc_Ami_s0(POA_XMP::Ami * obj,
		     XMP::AMI_AmiHandler_ptr ami_handler, 
		     CORBA::Long p1) :
      _obj(obj),
      _ami_handler(ami_handler),
      _p1(p1) 
    {}

    virtual int handle_timeout(const ACE_Time_Value &,
			       const void * = 0)
    {
      _obj->s0(_p1);
      if (_ami_handler != NULL)
	_ami_handler->s0();

      return -1;
    }

    virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask)
    {
      delete this;
      return 0;
    }

  private:
    XMP::Ami * _obj;
    XMP::AMI_AmiHandler_var _ami_handler;
    CORBA::Long _p1;
  };

  class AMI_sendc_Ami_s1 : public ACE_Event_Handler
  {
  public:
    AMI_sendc_Ami_s1(POA_XMP::Ami * obj,
		     XMP::AMI_AmiHandler_ptr ami_handler) :
      _obj(obj),
      _ami_handler(ami_handler)
    {}

    virtual int handle_timeout(const ACE_Time_Value &,
			       const void * = 0)
    {
      CORBA::Long  rc = _obj->s1();
      if (_ami_handler != NULL)
	_ami_handler->s1(rc);

      return -1;
    }

    virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask)
    {
      delete this;
      return 0;
    }

  private:
    XMP::Ami * _obj;
    XMP::AMI_AmiHandler_var _ami_handler;
  };

  class AMI_sendc_Ami_s2 : public ACE_Event_Handler
  {
  public:
    AMI_sendc_Ami_s2(POA_XMP::Ami * obj,
		     XMP::AMI_AmiHandler_ptr ami_handler) :
      _obj(obj),
      _ami_handler(ami_handler)
    {}

    virtual int handle_timeout(const ACE_Time_Value &,
			       const void * = 0)
    {
      try {
	_obj->s2();
	if (_ami_handler != NULL)
	  _ami_handler->s2();
      }
      catch (XMP::EAmi const& e) {
	Messaging::ExceptionHolder * eh = 
	  new POA_Messaging::ExceptionHolder_T<XMP::EAmi>(e);
	if (_ami_handler != NULL)
	  _ami_handler->s2_excep(eh);

      }
      return -1;
    }

    virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask)
    {
      delete this;
      return 0;
    }

  private:
    XMP::Ami * _obj;
    XMP::AMI_AmiHandler_var _ami_handler;
  }; 
}

namespace POA_XMP
{
  void 
  Ami::sendc_s0(XMP::AMI_AmiHandler_ptr ami_handler, CORBA::Long p1) throw()
  {
    ACE_Event_Handler * eh = new AMI_sendc_Ami_s0(this, ami_handler, p1);
    //    ServerWorker::instance()->reactor()->schedule_timer(eh, NULL,  0);
  }

  void
  Ami::sendc_s1(XMP::AMI_AmiHandler_ptr ami_handler) throw()
  {
    ACE_Event_Handler * eh = new AMI_sendc_Ami_s1(this, ami_handler);
    //    ServerWorker::instance()->reactor()->schedule_timer(eh, NULL, 0);
  }

  void 
  Ami::sendc_s2(XMP::AMI_AmiHandler_ptr ami_handler) throw()
  {
    ACE_Event_Handler * eh = new AMI_sendc_Ami_s2(this, ami_handler);
    //    ServerWorker::instance()->reactor()->schedule_timer(eh, NULL, 0);
  }
}

#endif // MIRO_NO_CORBA
