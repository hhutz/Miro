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
#ifndef LogReader_h
#define LogReader_h

#include "LogHeader.h"
#include "LogTypeRepository.h"
#include "miro/Parameters.h"

#include "miro_Export.h"

#include <orbsvcs/CosNotificationC.h>
#include <tao/CDR.h>
#include <ace/Mem_Map.h>

#include <string>

namespace Miro
{

  class miro_Export LogReader
  {
  public:
    static int const READER;
    static int const TRUNCATE;

    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Initializating constructor.
    LogReader(std::string const& _fileName, int mode = READER) throw(Miro::Exception);
    ~LogReader();

    TAO_InputCDR * istr() {
      return istr_;
    }
    char const * rdPtr() const throw();
    void rdPtr(char const * _rdPtr) throw();
    bool parseTimeStamp(ACE_Time_Value& _stamp) throw();
    bool parseEventHeader(CosNotification::FixedEventHeader& _header) throw();
    bool parseEventBody(CosNotification::StructuredEvent& _event) throw();
    bool skipEvent() throw();
    bool skipEventBody() throw();

    //! Report the protocol version.
    unsigned short version() const throw();
    //! Report the number of events in the log file.
    ACE_UINT32 events() const throw();
    //! Report the number of events in the log file.
    void events(ACE_UINT32 count) throw(Miro::Exception);
    //! Flag indicating end of file.
    bool eof() const throw();

    unsigned int progress() const throw();

  protected:
    void packTCR(char * dest) throw(Miro::Exception);

    //--------------------------------------------------------------------------
    // protected data
    //--------------------------------------------------------------------------

    //! Mode of operations (READER, TRUNCATE)
    int const mode_;
    //! Memory mapped file, holding the log.
    ACE_Mem_Map memMap_;
    //! File header.
    LogHeader * header_;
    //! CDR stream to read from.
    TAO_InputCDR * istr_;
    //! Instance of the type repository.
    LogTypeRepository * typeRepository_;
    //! Next log entry in stream.
    char const * next_;

    //! Version number of the log file protocol.
    ACE_UINT16 version_;
    //! Slot to write the offset of the type code repository in the log file. */
    char * tcrOffsetSlot_;
    //! Offset of the type code repository in log file (version >= 3).
    ACE_UINT32 tcrOffset_;
    //! Slot to write the offset of the type code repository in the log file. */
    char * eventsSlot_;
    //! Number of events in log (version >= 3).
    ACE_UINT32 events_;

    //! Flag inidcating end of file.
    bool eof_;
  };

  inline
  char const *
  LogReader::rdPtr() const throw()
  {
    return istr_->rd_ptr();
  }
  inline
  void
  LogReader::rdPtr(char const * _rdPtr) throw()
  {
    eof_ = false;

    if (_rdPtr == NULL) {
      eof_ = true;
    }


    if (version() == 2) {
      delete istr_;
      istr_ = new TAO_InputCDR(_rdPtr,
                               ((char *)memMap_.addr() + memMap_.size()) - _rdPtr,
                               (int)header_->byteOrder);
    }
    else {
      ACE_Message_Block const * mblock = istr_->start();
      const_cast<ACE_Message_Block *>(mblock)->rd_ptr(const_cast<char *>(_rdPtr));
    }
  }
  inline
  bool
  LogReader::eof() const throw()
  {
    return eof_;
  }
  inline
  unsigned short
  LogReader::version() const throw()
  {
    return version_;
  }
  inline
  ACE_UINT32
  LogReader::events() const throw()
  {
    return events_;
  }
  inline
  bool
  LogReader::skipEventBody() throw()
  {
    //    CosNotification::StructuredEvent event;
    //    return parseEventBody(event);

    if (eof_)
      return false;

    if (version() > 1) {
      rdPtr(next_);
    }
    else {
      CosNotification::OptionalHeaderFields variable_header;
      CosNotification::FilterableEventBody filterable_data;
      CORBA::Any remainder_of_body;
      if (!((*istr_) >> variable_header) ||
                  !((*istr_) >> filterable_data) ||
                  !((*istr_) >> remainder_of_body)) {
        eof_ = true;
        return false;
      }
    }

    return true;
  }
  inline
  unsigned int
  LogReader::progress() const throw()
  {
    return (unsigned int)((double)((char *) istr_->rd_ptr() -
                                   (char *) memMap_.addr()) * 100. /
                          (double) memMap_.size());
  }
}
#endif
