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
#ifndef miro_NotifyLogSvc_h
#define miro_NotifyLogSvc_h

#include "miro_Export.h"

#include <ace/Service_Config.h>
#include <ace/Service_Object.h>

#include <string>

namespace Miro
{
  class LogNotifyConsumer;
}

namespace Miro
{
  // forward declarations
  class Server;

  class miro_Export NotifyLogSvc : public ACE_Service_Object
  {
  public:
    NotifyLogSvc();
    virtual ~NotifyLogSvc();

    virtual int init(int argc, ACE_TCHAR *argv[]);
    virtual int info(ACE_TCHAR **src, size_t len) const;
    virtual int fini();

  private:
    int parse_args(int& argc, char* argv[]);

    Server * _server;
    Miro::LogNotifyConsumer * _consumer;

    std::string _fileName;
    bool        _verbose;

    // hidden default copy dtor and assignement operator.
    NotifyLogSvc(NotifyLogSvc const&);
    NotifyLogSvc& operator = (NotifyLogSvc const&);
  };
}

// Declare both static and dynamic services.
ACE_STATIC_SVC_DECLARE_EXPORT(miro, Miro_NotifyLogSvc)
ACE_FACTORY_DECLARE(miro, NotifyLogSvc)

#endif // clrty_impl_Camera_Svc_h
