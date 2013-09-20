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
#include "LogReader.h"
#include "LogHeader.h"
#include "LogTypeRepository.h"
#include "Log.h"
#include "Exception.h"
#include "TimeHelper.h"

#include <orbsvcs/Time_Utilities.h>
#include <orbsvcs/CosNotificationC.h>

#include <tao/Version.h>
#if (TAO_MAJOR_VERSION > 1) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION > 4) ) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) && (TAO_BETA_VERSION > 7) )
#  include <tao/AnyTypeCode/Any_Impl.h>
#  include <tao/AnyTypeCode/Any_Unknown_IDL_Type.h>
#elif ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) )
#  include <tao/Any_Impl.h>
#  include <tao/Any_Unknown_IDL_Type.h>
#else
#  include <tao/Marshal.h>
#  include <tao/Typecode.h>
#  include <tao/Environment.h>
#endif

#include <cstdio>

namespace Miro
{
  using namespace std;

  int const LogReader::READER = 0;
  int const LogReader::TRUNCATE = 1;

  LogReader::LogReader(string const& _fileName, int _mode) throw(Miro::Exception) :
      mode_(_mode),
      memMap_(_fileName.c_str(),  static_cast<size_t>(-1), (mode_ == TRUNCATE) ? O_RDWR : O_RDONLY,
              ACE_DEFAULT_FILE_PERMS, PROT_RDWR,
              (mode_ == TRUNCATE) ? ACE_MAP_SHARED : ACE_MAP_PRIVATE),
      header_(NULL),
      istr_(NULL),
      typeRepository_(NULL),
      next_(NULL),
      version_(1),
      tcrOffsetSlot_(NULL),
      tcrOffset_(sizeof(LogHeader)),
      eventsSlot_(NULL),
      events_(0),
      eof_(false)
  {
    if (memMap_.addr() == MAP_FAILED)
      throw CException(errno, strerror(errno));

    if (memMap_.size() <= sizeof(LogHeader) + 2 * 4)
      throw Exception("LogFile contains no data.");

    MIRO_DBG(MIRO, LL_DEBUG, "Looking for log file type.");
    LogHeader::READ r;

    header_ = new(memMap_.addr()) LogHeader(r);
    version_ = header_->version;

    if (mode_ != READER && version_ < 3) {
      throw Miro::Exception("Log truncation not supported for log file format prior v 3");
    }


    istr_ = new TAO_InputCDR((char*)memMap_.addr() + sizeof(LogHeader),
                             memMap_.size() - sizeof(LogHeader),
                             (int)header_->byteOrder);
    // version 2 log file
    if (version() == 2) {
      istr_ = new TAO_InputCDR((char*)memMap_.addr() + sizeof(LogHeader),
                               memMap_.size() - sizeof(LogHeader),
                               (int)header_->byteOrder);
      typeRepository_ = new LogTypeRepository(*istr_);
    }
    // version 3 log file
    else if (version() == 3 || version() == 4) {
      istr_ = new TAO_InputCDR((char*)memMap_.addr() + sizeof(LogHeader),
                               memMap_.size() - sizeof(LogHeader),
                               (int)header_->byteOrder);

      tcrOffsetSlot_ = istr_->rd_ptr();
      if (!istr_->read_ulong(tcrOffset_))
        throw Exception("Could not read tcrOffset_.");
      if (tcrOffset_ > (memMap_.size() - sizeof(LogHeader) + 2 * 4)) {
        throw Exception("tcrOffset_ outside file boundaries. Logfile corrupted.");
      }

      eventsSlot_ = istr_->rd_ptr();
      if (!istr_->read_ulong(events_))
        throw Exception("Could not read events_.");
      if (events_ == 0) {
        throw Exception("Log file contains zero events data.");
      }

      MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                    "LogReader - TCR Offset: 0x" <<
                    hex << tcrOffset_ << dec << endl <<
                    "LogReader - Number of events: " <<
                    events_ << dec);

      TAO_InputCDR tcrIStream((char*)memMap_.addr() + tcrOffset_,
                              memMap_.size() - tcrOffset_,
                              (int)header_->byteOrder);
      typeRepository_ = new LogTypeRepository(tcrIStream);


      MIRO_DBG_OSTR(MIRO, LL_PRATTLE, "LogReader - Good bit : " << istr_->good_bit());
    }
    else {
      ostringstream s;
      s << "Unsupported log format version: "  << version();
      throw Exception(s.str());
    }
    MIRO_DBG_OSTR(MIRO, LL_DEBUG,  "version : " << version());
  }

  LogReader::~LogReader()
  {
    if (mode_ == TRUNCATE) {
      string filename = memMap_.filename();
      size_t fileSize =  memMap_.size();
      char * base = (char *)memMap_.addr();

      //--------------------------------------------------------------------------
      // place type codes at the end of the event stream
      //--------------------------------------------------------------------------

      char * dest = istr_->rd_ptr();
      // 8 byte alignement
      dest = ACE_ptr_align_binary(dest, 8);

      // add the type repository to the length
      size_t size = dest - base;
      size_t totalSize = size + typeRepository_->totalLength();

      MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                    "LogReader - TRUNCATE base:       " << hex << (void*) base << dec << endl <<
                    "LogReader - TRUNCATE size:       " << size << endl <<
                    "LogReader - TRUNCATE tcr offset: " << hex << (void*)dest << dec << endl <<
                    "LogReader - TRUNCATE tcr size:   " << typeRepository_->totalLength() << endl <<
                    "LogReader - TRUNCATE total size: " << totalSize << endl);

      packTCR(dest);

      //--------------------------------------------------------------------------
      // log file size cleanup
      //--------------------------------------------------------------------------


      // close the memory mapped file
      memMap_.close();

      if (fileSize > totalSize) {
        MIRO_LOG_OSTR(LL_WARNING, "LogReader - TRUNCATE discarded bytes: " << fileSize - totalSize);

        if (ACE_OS::truncate((filename).c_str(), totalSize) == -1) {
          // We shouldn't throw in a destructor...
          MIRO_LOG_OSTR(LL_WARNING,
                        "LogReader - Error " << errno <<
                        " truncating log file " << filename  << endl
                        << strerror(errno));
        }
      }
    }
  }

  bool
  LogReader::parseTimeStamp(ACE_Time_Value& _stamp) throw()
  {
    if (eof_ ||
                istr_->length() == 0 ||
                istr_->rd_ptr() == 0)
      return false;

    // version >= 2 parsing
    if (version() >= 2) {

      // get the time stamp
      TimeBase::TimeT t;
      if (!(istr_->read_ulonglong(t))) {
        MIRO_DBG(MIRO, LL_DEBUG, "eof 1");
        eof_ = true;
        return false;
      }

      if (next_ != NULL &&
                  next_ != istr_->rd_ptr() - sizeof(TimeBase::TimeT)) {
        MIRO_LOG_OSTR(LL_ERROR,
                      "mismatch: " << (void*)next_ <<
                      "\t " << (void *)(istr_->rd_ptr() - sizeof(TimeBase::TimeT)));
      }

      if (version() >= 4) {
        ORBSVCS_Time::Absolute_TimeT_to_Time_Value(_stamp, t);
      }
      else {
        ORBSVCS_Time::TimeT_to_Time_Value(_stamp, t);
      }
    }
    // version 1 parsing
    else {
      MIRO_LOG(LL_ERROR, "version 1 log files not supported");
#if 0
      TimeIDL timeStamp;
      if (!((*istr_) >> timeStamp)) {
        MIRO_DBG(MIRO, LL_DEBUG, "eof 2");
        eof_ = true;
        return false;
      }

      timeC2A(timeStamp, _stamp);
#endif
    }

    if (_stamp == ACE_Time_Value::zero) {
      MIRO_DBG(MIRO, LL_DEBUG, "eof 3");
      eof_ = true;
      return false;
    }
    return true;
  }

  bool
  LogReader::skipEvent() throw()
  {
    if (eof_)
      return false;

    // version >= 2 parsing
    if (version() >= 2) {
      // get address of next event
      char const * here = istr_->start()->rd_ptr();
      ACE_UINT32 len;
      if (!istr_->read_ulong(len)) {
        MIRO_DBG(MIRO, LL_DEBUG, "eof h1");
        eof_ = true;
        return false;
      }

      if (len == 0) {
        MIRO_DBG(MIRO, LL_DEBUG, "eof h2");
        eof_ = true;
        return false;
      }

      next_ = here + len;

      rdPtr(next_);
    }
    // version 1 parsing
    else {
      CosNotification::FixedEventHeader header;
      CosNotification::OptionalHeaderFields variable_header;
      CosNotification::FilterableEventBody filterable_data;
      CORBA::Any remainder_of_body;
      if (!((*istr_) >> header) ||
                  !((*istr_) >> variable_header) ||
                  !((*istr_) >> filterable_data) ||
                  !((*istr_) >> remainder_of_body)) {
        eof_ = true;
        return false;
      }
    }
    return true;
  }

  bool
  LogReader::parseEventHeader(CosNotification::FixedEventHeader& _header) throw()
  {
    if (eof_)
      return false;

    // version >= 2 parsing
    if (version() >= 2) {
      // get address of next event
      char const * here = istr_->start()->rd_ptr();
      ACE_UINT32 len;
      if (!istr_->read_ulong(len)) {
        MIRO_DBG(MIRO, LL_DEBUG, "eof h1");
        eof_ = true;
        return false;
      }
      if (len == 0)
        eof_ = true;

      next_ = here + len;

      // get header
      if (!((*istr_) >> _header)) {
        eof_ = true;
        return false;
      }
    }
    // version 1 parsing
    else {
      if (!((*istr_) >> _header)) {
        eof_ = true;
        return false;
      }
    }
    return true;
  }

  bool
  LogReader::parseEventBody(CosNotification::StructuredEvent& _event) throw()
  {
    if (eof_) {
      MIRO_DBG(MIRO, LL_DEBUG, "eof e1");
      return false;
    }

    // get the rest of the header (variable header)
    // get the filterable data
    if (!((*istr_) >> _event.header.variable_header) ||
                !((*istr_) >> _event.filterable_data)) {
      MIRO_DBG(MIRO, LL_DEBUG, "eof e2");
      eof_ = true;
      return false;
    }

    // parse the any
    if (version() >= 2) {

      // get type code from repository
      ACE_INT32 id;
      if (!istr_->read_long(id)) {
        eof_ = true;
        return false;
      }

      CORBA::TypeCode_ptr tc = typeRepository_->typeCode(id);

      if (tc == CORBA::_tc_null) {
        _event.remainder_of_body = CORBA::Any();
        return true;

      }

      // direct copies from TAO sources
      // read the any payload

#if (TAO_MAJOR_VERSION > 1) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION > 4) ) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) && (TAO_BETA_VERSION > 3) ) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) && (TAO_OCI_REV == a) && (TAO_OCI_PATCHLEVEL >= 11) )

      try {

        TAO::Unknown_IDL_Type *impl = new TAO::Unknown_IDL_Type(tc);
        _event.remainder_of_body.replace(impl);
        impl->_tao_decode(*istr_);
      }
      catch (CORBA::Exception const&) {
        eof_ = true;
        return false;
      }

#elif \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 4) ) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 3) && (TAO_BETA_VERSION > 4) )

      try {

        TAO::Unknown_IDL_Type *impl = new TAO::Unknown_IDL_Type(tc,
            0,
            istr_->byte_order());
        _event.remainder_of_body.replace(impl);
        impl->_tao_decode(*istr_);
      }
      catch (CORBA::Exception const&) {
        eof_ = true;
        return false;
      }
#else
      ACE_TRY_NEW_ENV {

        // This will be the start of a new message block.
        char *begin = istr_->rd_ptr();

        // Skip over the next aregument.
#if \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 3) ) || \
  ( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 2) && (TAO_BETA_VERSION > 1) )
        CORBA::TypeCode::traverse_status status =
          TAO_Marshal_Object::perform_skip(tc,
                                           istr_
                                           ACE_ENV_ARG_PARAMETER);
#else // TAO_MINOR_VERSION <= 1
        CORBA::TypeCode::traverse_status status =
          TAO_Marshal_Object::perform_skip(tc, istr_, ACE_TRY_ENV);
#endif
        ACE_TRY_CHECK;

        if (status != CORBA::TypeCode::TRAVERSE_CONTINUE) {
          eof_ = true;
          return false;
        }

        // This will be the end of the new message block.
        char *end = istr_->rd_ptr();

        // The ACE_CDR::mb_align() call can shift the rd_ptr by up to
        // ACE_CDR::MAX_ALIGNMENT-1 bytes. Similarly, the offset adjustment
        // can move the rd_ptr by up to the same amount. We accommodate
        // this by including 2 * ACE_CDR::MAX_ALIGNMENT bytes of additional
        // space in the message block.
        size_t size = end - begin;
        ACE_Message_Block mb(size + 2 * ACE_CDR::MAX_ALIGNMENT);
        ACE_CDR::mb_align(&mb);
        ptr_arith_t offset = ptr_arith_t (begin) % ACE_CDR::MAX_ALIGNMENT;
        mb.rd_ptr(offset);
        mb.wr_ptr(offset + size);
        ACE_OS::memcpy(mb.rd_ptr(), begin, size);

        // Stick it into the Any. It gets duplicated there.
        _event.remainder_of_body._tao_replace(tc,
                                              istr_->byte_order(),
                                              &mb);
      }
      ACE_CATCH(CORBA_Exception, ex) {
        eof_ = true;
        return false;
      }
      ACE_ENDTRY;

#endif
    }
    // version 1 parsing
    else {
      if (!((*istr_) >> _event.remainder_of_body)) {
        eof_ = true;
        return false;
      }
    }

    return true;
  }


  void
  LogReader::events(ACE_UINT32 count) throw(Miro::Exception)
  {
    if (mode_ != READER && version_ < 3) {
      throw Miro::Exception("Log truncation not supported for log file format prior v 3");
    }

    if (events_ != count) {
      // TODO: honor byte swapping if necessary
      *reinterpret_cast<ACE_UINT32 *>(eventsSlot_) = count;
      events_ = count;
    }
  }

  void
  LogReader::packTCR(char * dest) throw(Miro::Exception)
  {
    char const * const source = (char *)memMap_.addr() + tcrOffset_;
    size_t size =  typeRepository_->totalLength();
    size_t offset = dest - (char *)memMap_.addr();

    if (dest != source) {
      memmove(dest, source, size);

      // note new location of tcr
      TAO_OutputCDR o(tcrOffsetSlot_, 4);
      if (!o.write_ulong(offset))
        throw Miro::Exception("Failed to write tcr offset count");
    }
    MIRO_DBG_OSTR(MIRO, LL_DEBUG,
                  "LogWriter - TCR Offset: 0x" << hex << offset << dec << endl <<
                  "LogWriter - TCR number of events: " << events_ << endl <<
                  "LogWriter - TCR length: " << size);
  }
}
