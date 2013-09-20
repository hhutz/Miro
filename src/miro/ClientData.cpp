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
#include "ClientData.h"

namespace Miro
{
  ACE_Thread_Mutex ClientData::instance_mutex_;
  ClientData * ClientData::instance_ = NULL;

  ClientData *
  ClientData::instance(int& argc, char * argv[])
  {
    ACE_Guard<ACE_Thread_Mutex> guard(instance_mutex_);
    if (NULL == instance_) {
      instance_ = new ClientData(argc, argv);
    }
    instance_->add_ref();
    return instance_;
  }

#ifndef MIRO_NO_CORBA
  ClientData::ClientData(int& argc, char* argv[]) :
      orb(CORBA::ORB_init(argc, argv)),
      ref_count_(0)
  {

  }
#else
  ClientData::ClientData(int&, char**) :
      ref_count_(0)
  {}

#endif // MIRO_NO_CORBA


  void
  ClientData::add_ref()
  {
    ++ref_count_;
  }

  void
  ClientData::remove_ref()
  {
    unsigned int const new_count = --ref_count_;

    if (new_count == 0)
      delete this;
  }

  unsigned int
  ClientData::refcount_value() const
  {
    return ref_count_.value();
  }
}
