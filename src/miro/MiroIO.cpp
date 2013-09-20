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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "MiroIO.h"

#include <ace/INET_Addr.h>
#include <ace/Sched_Params.h>

#include <sstream>

std::ostream &
operator<<(std::ostream &ostr, ACE_Sched_Params const &rhs)
{
  std::ostringstream p;
  switch (rhs.policy()) {
    case ACE_SCHED_OTHER:
      p <<  "ACE_SCHED_OTHER";
      break;
    case ACE_SCHED_FIFO:
      p << "ACE_SCHED_FIFO";
      break;
    case ACE_SCHED_RR:
      p << "ACE_SCHED_RR";
      break;
    default:
      p << rhs.policy();
  }

  ostr << "policy = " << p.str() << std::endl
  << "priority = " << rhs.priority() << std::endl
  << "quantum = " << rhs.quantum() << " sec";

  return ostr;
}

std::ostream &
operator<<(std::ostream &ostr, ACE_INET_Addr const &rhs)
{
  ostr << rhs.get_host_addr() << ":" << rhs.get_port_number();
  return ostr;
}
