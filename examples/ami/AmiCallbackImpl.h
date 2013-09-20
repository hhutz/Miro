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
#ifndef XMP_AmiCallbackImpl_h
#define XMP_AmiCallbackImpl_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef MIRO_NO_CORBA
#  include "AmiS.h"
#else
#  include "Ami.h"
#endif
#include "miro/AmiHandler.h"

namespace XMP
{
  //! Dummy callback impl.
  /** It jsut instanciates the pure virtual methods, to allow partial
   * implemenations, as done through the helper macros.
   */
  class AMI_Ami_Default : public virtual POA_XMP::AMI_AmiHandler
  {
  public:
    // inherited interface
    virtual void s () throw();
    virtual void s_excep (::Messaging::ExceptionHolder *) throw();
    virtual void s0 () throw();
    virtual void s0_excep (::Messaging::ExceptionHolder *) throw();
    virtual void s1 (::CORBA::Long) throw();
    virtual void s1_excep (::Messaging::ExceptionHolder *) throw();
    virtual void s2 () throw(::XMP::EAmi);
    virtual void s2_excep (::Messaging::ExceptionHolder *) throw();
  };

  //! Generic implementation of callback fo s0.
  /** This is exactly the code produced by the helper macros.  But
   * it's much easier to understand it that way...
   */
  class AMI_Ami_s : public AMI_Ami_Default,
		     public virtual Miro::VoidAmiHandler
  {
    virtual void s() throw();
    virtual void s_excep (::Messaging::ExceptionHolder * excep_holder) throw();
  };

  //! Generic implementation of callback fo s0.
  /** This is exactly the code produced by the helper macros.  But
   * it's much easier to understand it that way...
   */
  class AMI_Ami_s0 : public AMI_Ami_Default,
		     public virtual Miro::VoidAmiHandler
  {
    virtual void s0() throw();
    virtual void s0_excep (::Messaging::ExceptionHolder * excep_holder) throw();
  };

  //! Helper macro based implementation of generic callback for s1.
  class AMI_Ami_s1 : public AMI_Ami_Default,
		     public virtual Miro::TypedAmiHandler<int>
  {
    GENERIC_TYPED_CALLBACK_DECL(s1, int, ());
    GENERIC_EXCEPTION_CALLBACK_DECL(s1);
  };

  //! Helper macro based implementation of generic callback for s2.
  class AMI_Ami_s2 : public AMI_Ami_Default,
		     public virtual Miro::VoidAmiHandler
  {
    GENERIC_VOID_CALLBACK_DECL(s2, (XMP::EAmi));
    GENERIC_EXCEPTION_CALLBACK_DECL(s2);
  };
}
#endif // XMP_AmiCallbackImpl_h
