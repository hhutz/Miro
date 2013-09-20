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
#include "LogWriter.h"
#include "LogTypeRepository.h"
#include "Log.h"
#include "Exception.h"

#include <orbsvcs/Time_Utilities.h>

#include <tao/Version.h>
#if (TAO_MAJOR_VERSION > 1) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION > 4) ) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) && (TAO_BETA_VERSION > 7) )
#  include <tao/AnyTypeCode/Any_Impl.h>
#elif ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) )
#  include <tao/Any_Impl.h>
#else
#include <tao/Marshal.h>
#endif

#include <ace/FILE_Connector.h>
#include <ace/OS_Memory.h>

#include <cstdio>

namespace Miro
{
  using namespace std;

  LogWriter::LogWriter(std::string const& _fileName,
                       LogNotifyParameters const& _parameters) :
      parameters_(_parameters),
      fileName_(_fileName),
      memMap_((fileName_).c_str(), parameters_.maxFileSize,
              O_RDWR | O_CREAT | O_TRUNC, ACE_DEFAULT_FILE_PERMS, PROT_RDWR,
              ACE_MAP_SHARED),
      header_(new(memMap_.addr()) LogHeader(w_)),
      ostr_((char*)memMap_.addr() + sizeof(LogHeader),
            memMap_.size() - sizeof(LogHeader) - parameters_.tCRFileSize),
      tcrOstr_((char*)memMap_.addr() + memMap_.size() - parameters_.tCRFileSize,
               parameters_.tCRFileSize),
      typeRepository_(&tcrOstr_, parameters_.tCRFileSize),
      tcrOffsetSlot_(NULL),
      numEventsSlot_(NULL),
      numEvents_(0UL),
      totalLength_(0),
      full_(false)
  {
    MIRO_LOG_CTOR("Miro::LogWriter");

    if (memMap_.addr() == MAP_FAILED)
      throw CException(errno, "Opening " + _fileName + ": " + strerror(errno));

    // The alignement is okay as we wrote 8 bytes of LogHeader
    tcrOffsetSlot_ = ostr_.current()->wr_ptr();
    ostr_.write_ulong(memMap_.size() - parameters_.tCRFileSize);

    // The allignement is okay as we write now a ulong.
    numEventsSlot_ = ostr_.current()->wr_ptr();
    ostr_.write_ulong(0);
  }

  LogWriter::~LogWriter()
  {
    //--------------------------------------------------------------------------
    // place type codes at the end of the event stream
    //--------------------------------------------------------------------------
    packTCR();

    //--------------------------------------------------------------------------
    // log file size cleanup
    //--------------------------------------------------------------------------

    // close the memory mapped file
    memMap_.close();

    if (ACE_OS::truncate((fileName_).c_str(), totalLength_) == -1) {
      // We shouldn't throw in a destructor...
      MIRO_LOG_OSTR(LL_WARNING,
                    "LogWriter - Error " << errno <<
                    " truncating log file " << fileName_  << std::endl
                    << strerror(errno));
    }
  }

  bool
  LogWriter::logEvent(ACE_Time_Value const& _stamp,
                      CosNotification::StructuredEvent const& _event)
  {
    // if there is place in the log file
    if (!full_) { // not full

      // set the time stamp
      TimeBase::TimeT t;
      ORBSVCS_Time::Absolute_Time_Value_to_TimeT(t, _stamp);

      if (ostr_.write_ulonglong(t)) { // write time stamp

        /*Slot to write the length of the serialized structured event.
         * This is used for skipped parsing of the file. */
        char * lengthSlot =
          // The alignement is okay as we wrote an ulonglong before
          ostr_.current()->wr_ptr();

        // write the length slot
        if (ostr_.write_ulong(0) &&
                    // write the header
                    ostr_ << _event.header &&
                    // write the filterable data
                    ostr_ << _event.filterable_data) { // and now the data


          // serialize remainder_of_body

          // obtain type code id
          CORBA::Long typeId = -1;
          CORBA::TypeCode_var tc = _event.remainder_of_body.type();
          if (tc.in() != CORBA::_tc_null) {
            typeId = typeRepository_.typeID(tc.in());
          }

#if (TAO_MAJOR_VERSION > 1) || \
  ((TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION >= 4)) || \
  ((TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 3 && TAO_BETA_VERSION >= 5))

          //----------------------------------------------------------------
          // we have Any_Impl
          //----------------------------------------------------------------

          // if not type code repository full
          if (typeId != -2 &&
                      // write type code id
                      ostr_.write_long(typeId) &&
                      // write any value if existent
                      (_event.remainder_of_body.impl() == NULL ||
                       _event.remainder_of_body.impl()->marshal_value(ostr_)) &&
                      // not max file size reached
                      (ostr_.total_length() <=
                       (parameters_.maxFileSize - parameters_.tCRFileSize - 100000))) {

            ostr_.consolidate();

            MIRO_ASSERT(ostr_.current() == ostr_.begin());

            // calculate length
            char * here = ACE_ptr_align_binary(ostr_.current()->wr_ptr(), ACE_CDR::LONGLONG_SIZE);
            CORBA::ULong length = here - lengthSlot;

            // write the length of the event
            // direct writing is allowed,
            // as the alignement is correct and we write in host byte order
            *reinterpret_cast<ACE_INT32 *>(lengthSlot) = length;

            // write number of events
            // direct writing is allowed,
            // as the alignement is correct and we write in host byte order
            *reinterpret_cast<ACE_INT32 *>(numEventsSlot_) = ++numEvents_;

            totalLength_ = ostr_.total_length();
            return true;
          }
#else
#error Only TAO >= Version 1.3.5 is supported
#endif
        }
      }
      full_ = true;
    }

    MIRO_LOG_OSTR(LL_ERROR,
                  "Event log data - max file size reached:" <<
                  totalLength_ <<
                  " - Event logging stopped.");
    return false;
  }

  void
  LogWriter::packTCR() throw(CException)
  {
    char * dest = (char *)memMap_.addr();
    ACE_UINT32 offset = sizeof(LogHeader) + totalLength_;

    // 8 byte alignement
    offset += (0x08 - (totalLength_ & 0x07)) & 0x07;

    dest += offset;
    memmove(dest, tcrOstr_.current()->base(), typeRepository_.totalLength());

    // note new location of tcr
    TAO_OutputCDR o_(tcrOffsetSlot_, 8);
    o_.write_ulong(offset);

    MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                  "LogWriter - TCR Offset: 0x" << std::hex << offset << std::dec << std::endl <<
                  "LogWriter - Number of events: " << numEvents_);

    // get total length of the file
    totalLength_ = offset + typeRepository_.totalLength();
    MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                  "LogWriter - TCR length: " <<  typeRepository_.totalLength());
  }
}
