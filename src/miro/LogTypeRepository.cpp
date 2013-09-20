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
#include "LogTypeRepository.h"
#include "Log.h"
#include "Exception.h"

#include <tao/CDR.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace Miro
{
  LogTypeRepository::LogTypeRepository(TAO_OutputCDR * _ostr,
                                       size_t _maxLength) throw(CException):
      ostr_(_ostr),
      maxLength_(_maxLength),
      totalLength_(ostr_->total_length()),
      full_(false)
  {
    types_.reserve(64);

    pNumTypes_ = ostr_->current()->wr_ptr();
    ostr_->write_ulong(0x00000000);
  }

  LogTypeRepository::LogTypeRepository(TAO_InputCDR& _istr) throw(Exception) :
      ostr_(NULL),
      maxLength_(),
      totalLength_(),
      full_(true)
  {
    //--------------------------------------------------------------------------
    // parse log repository
    //--------------------------------------------------------------------------

    // get number of types in cdr stream
    ACE_UINT32 numTypes;
    _istr.read_ulong(numTypes);
    try {
      types_.reserve(numTypes);
    }
    catch (std::length_error const& /*e*/) {
      MIRO_LOG_OSTR(LL_CRITICAL, "LogTypeRepository - std exception on types_.reseve(" << numTypes << ").");
      throw Miro::Exception("Length error on reserving log type vector. Probable log-file corruption.");
    }

    MIRO_DBG_OSTR(MIRO, LL_DEBUG, "LogTypeRepository - number of types: " << numTypes);

    // read each type from cdr stream
    for (ACE_UINT32 i = 0; i < numTypes; ++i) {

      // read type code from the mmapped file
      CORBA::TypeCode_ptr type;
      if (!(_istr >> type))
        throw Exception("Error reading log type repository. LogFile corrupted.");

      // search for type code. fail, if already known
      MIRO_ASSERT(std::find(types_.begin(), types_.end(), type) == types_.end());

      // add type to our repository
      types_.push_back(CORBA::TypeCode::_duplicate(type));
    }

    ACE_Message_Block const * block = _istr.start();
    totalLength_ = (block->rd_ptr() - block->base());

    MIRO_DBG_OSTR(MIRO, LL_DEBUG, "LogTypeRepository - total length: " << totalLength_);
  }

  LogTypeRepository::~LogTypeRepository() throw()
  {
    // free type codes
    TypeCodeVector::const_iterator first, last = types_.end();
    for (first = types_.begin(); first != last; ++first) {
      CORBA::release(*first);
    }
  }

  CORBA::Long
  LogTypeRepository::addType(CORBA::TypeCode_ptr _type)
  {
    // add type code to the mmapped file

    TAO_OutputCDR ostr;
    ostr << _type;

    // if memory is used up, fail.
    if (ostr_->total_length() + ostr.total_length() > maxLength_) {
      MIRO_LOG(LL_ERROR, "Event log type code repository full.");
      full_ = true;
      return -2;
    }

    // flatten message block queue
    ACE_Message_Block * buffer = const_cast<ACE_Message_Block *>(ostr_->current());
    for (ACE_Message_Block const * block = ostr.begin(); block != NULL; block = block->cont()) {
      buffer->copy(block->rd_ptr(), block->length());
    }

    // add type to our repository
    types_.push_back(CORBA::TypeCode::_duplicate(_type));

    // increase type code counter
    TAO_OutputCDR len(pNumTypes_, 32);
    len.write_ulong(types_.size());

    // remember file length (for cleanup)
    totalLength_ = ostr_->total_length();

    return types_.size() - 1;
  }
}
