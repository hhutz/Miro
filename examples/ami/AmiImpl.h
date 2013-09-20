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
#ifndef XMP_AmiImpl_h
#define XMP_AmiImpl_h

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
  class  AmiImpl : public virtual POA_XMP::Ami
  {
  public:
    virtual void s ()
      throw();

    virtual void s0 (::CORBA::Long s)
      throw();
  
    virtual ::CORBA::Long s1 () 
      throw();
    
    virtual void s2 ()
      throw(XMP::EAmi);
  };
}
#endif // XMP_AmiImpl_h
