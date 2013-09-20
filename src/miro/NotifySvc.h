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
#ifndef miro_NotifySvc_h
#define miro_NotifySvc_h

#include "miro_Export.h"

#include <ace/svc_export.h>

#include <ace/Service_Config.h>
#include <ace/Service_Object.h>

class TAO_Notify_Properties;

namespace Miro
{
  // forward declarations
  class Server;

  class miro_Export NotifySvc : public ACE_Service_Object
  {
  public:
    NotifySvc();
    virtual ~NotifySvc();

    virtual int init(int argc, ACE_TCHAR *argv[]);
    virtual int info(ACE_TCHAR **src, size_t len) const;
    virtual int fini();

  private:
    int parseArgs(int& argc, char* argv[]);

    Server * m_server;
    TAO_Notify_Properties * m_properties;

    bool m_factory;
    bool m_verbose;

    // hidden default copy dtor and assignement operator.
    NotifySvc(NotifySvc const&);
    NotifySvc& operator = (NotifySvc const&);
  };
}

// Declare both static and dynamic services.
ACE_STATIC_SVC_DECLARE_EXPORT(miro, Miro_NotifySvc)
ACE_FACTORY_DECLARE(miro, NotifySvc)

#endif // clrty_impl_Camera_Svc_h
