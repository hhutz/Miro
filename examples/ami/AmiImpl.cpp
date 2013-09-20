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
#include "miro/Client.h"

#include <iostream>

namespace XMP
{
  void AmiImpl::s ()
    throw ()
  {
    std::cout << "  ...entering s, sleeping for " << 1 << " second." << std::endl;
    ACE_OS::sleep(1);
    std::cout << "  ...waking up in s, returning." << std::endl;
  }

  void AmiImpl::s0 (::CORBA::Long s)
    throw ()
  {
    std::cout << "  ...entering s0, sleeping for " << s << " seconds." << std::endl;
    ACE_OS::sleep(s);
    std::cout << "  ...waking up in s0, returning." << std::endl;
  }

  ::CORBA::Long AmiImpl::s1 ()
    throw ()
  {
    std::cout << "  ...entering s1, sleeping for 2 seconds." << std::endl;
    ACE_OS::sleep(2);
    std::cout << "  ...waking up in s1, returning a value (2)." << std::endl;
    return 2;
  }

  /** Server side mapping for strings.
   * We do not use wide strings.
   */
  void AmiImpl::s2 ()
    throw (::XMP::EAmi)
  {
    std::cout << "  ...entering s2, sleeping for 3 seconds." << std::endl;
    ACE_OS::sleep(3);
    std::cout << "  ...waking up in s2, throwing an exception." << std::endl;
    throw EAmi("This is an expected exception!");
  }
}

