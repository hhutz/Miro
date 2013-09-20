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
#include "LogHeader.h"
#include "Exception.h"

#include <ace/CDR_Stream.h>

#include <string>
#include <sstream>

#include <cstring>

namespace Miro
{
  LogHeader::LogHeader(WRITE) :
      id(PROTOCOL_ID),
      version(PROTOCOL_VERSION),
      byteOrder(ACE_CDR_BYTE_ORDER)
  {}

  LogHeader::LogHeader(READ) throw(EFileType, EVersion)
  {
    if (id != PROTOCOL_ID) {
      char idString[5] = "    ";
      memcpy(idString, &id, sizeof(id));
      throw EFileType(std::string("Wrong protocol ID for a Miro Log File (should be MLOG):") + idString);
    }

    if (version > MAX_VERSION) {
      std::ostringstream o;
      o << version;
      throw EVersion("Unsupported protocol version: " + o.str());
    }
  }
}
