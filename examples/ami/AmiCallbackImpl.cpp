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
#include "AmiCallbackImpl.h"
#include "miro/Client.h"

#include <iostream>

namespace XMP
{
  void 
  AMI_Ami_Default::s () throw() 
  {}
  void
  AMI_Ami_Default::s_excep (::Messaging::ExceptionHolder *) throw()
  {}
  void 
  AMI_Ami_Default::s0 () throw() 
  {}
  void
  AMI_Ami_Default::s0_excep (::Messaging::ExceptionHolder *) throw()
  {}
  void
  AMI_Ami_Default::s1 (::CORBA::Long) throw()
  {}
  void
  AMI_Ami_Default::s1_excep (::Messaging::ExceptionHolder *) throw()
  {}
  void
  AMI_Ami_Default::s2 () throw(::XMP::EAmi)
  {}
  void
  AMI_Ami_Default::s2_excep (::Messaging::ExceptionHolder *) throw()
  {}

  // generic implemenation
  void AMI_Ami_s::s () throw()
  {
    finish();
  }

  // generic implemenation
  void AMI_Ami_s::s_excep (::Messaging::ExceptionHolder * excep_holder) throw()
  {
    finishByException(excep_holder);
  }

  // generic implemenation
  void AMI_Ami_s0::s0 () throw()
  {
    finish();
  }

  // generic implemenation
  void AMI_Ami_s0::s0_excep (::Messaging::ExceptionHolder * excep_holder) throw()
  {
    finishByException(excep_holder);
  }

  // helper macro based implementations

  GENERIC_TYPED_CALLBACK_IMPL(AMI_Ami_s1, s1, int, ());
  GENERIC_EXCEPTION_CALLBACK_IMPL(AMI_Ami_s1, s1);

  GENERIC_VOID_CALLBACK_IMPL(AMI_Ami_s2, s2, (XMP::EAmi));
  GENERIC_EXCEPTION_CALLBACK_IMPL(AMI_Ami_s2, s2);
}

