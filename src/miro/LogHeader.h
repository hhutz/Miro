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
#ifndef miro_LogHeader_h
#define miro_LogHeader_h

#include "miro_Export.h"

#include "Exception.h"

#include <ace/Basic_Types.h>

namespace Miro
{
  struct miro_Export LogHeader {
    //--------------------------------------------------------------------------
    // public types
    //--------------------------------------------------------------------------
    MIRO_EXCEPTION_TYPE(EVersion);
    MIRO_EXCEPTION_TYPE(EFileType);

    struct READ {};
    struct WRITE {};

    //--------------------------------------------------------------------------
    // public constants
    //--------------------------------------------------------------------------

    static ACE_UINT32 const PROTOCOL_ID = 0x474f4c4d;      // "MLOG";
    static ACE_UINT16 const PROTOCOL_VERSION = 0x0004;
    static ACE_UINT16 const MAX_VERSION = 0x0004;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    LogHeader(WRITE);
    LogHeader(READ) throw(EFileType, EVersion);

    //--------------------------------------------------------------------------
    // public data
    //--------------------------------------------------------------------------

    ACE_UINT32 id;
    ACE_UINT16 version;
    ACE_UINT16 byteOrder;
  };
}
#endif // miro_LogHeader_h
