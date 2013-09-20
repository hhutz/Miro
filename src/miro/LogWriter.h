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
#ifndef LogWriter_h
#define LogWriter_h

#include "LogHeader.h"
#include "LogTypeRepository.h"
#include "StructuredPushConsumer.h"
#include "miro/Parameters.h"

#include "miro_Export.h"

#include <ace/Mem_Map.h>
#include <ace/High_Res_Timer.h>
#include <tao/CDR.h>

#include <string>

namespace Miro
{
  class miro_Export LogWriter
  {
  public:
    //--------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------

    //! Initializating constructor.
    LogWriter(std::string const& _fileName,
              LogNotifyParameters const& _parameters = *LogNotifyParameters::instance());
    //! Disconnect from the supplier.
    ~LogWriter();

    //! Inherited IDL interface: StructuredPushSupplier method
    bool logEvent(ACE_Time_Value const& _stamp,
                  CosNotification::StructuredEvent const& _event);
    //! Report the protocol version.
    ACE_UINT16 version() const;

  protected:
    //--------------------------------------------------------------------------
    // protected methods
    //--------------------------------------------------------------------------

    void packTCR() throw(CException);

    //--------------------------------------------------------------------------
    // protected data
    //--------------------------------------------------------------------------

    //! Reference to the parameters.
    LogNotifyParameters const& parameters_;
    //! The name of the log file.
    std::string const fileName_;
    //! Memory mapped file, holding the log.
    ACE_Mem_Map memMap_;
    //! Writer flag for log header.
    /** Has to be allocated separately, due to compiler problems. */
    LogHeader::WRITE w_;
    //! Header block of the log file.
    LogHeader * header_;
    //! CDR stream to log to.
    TAO_OutputCDR ostr_;
    //! CDR stream to log type codes to.
    TAO_OutputCDR tcrOstr_;
    //! Instance of the type repository.
    LogTypeRepository typeRepository_;
    //! Slot to write the offset of the type code repository in the log file. */
    char * tcrOffsetSlot_;
    //! Slot to write the number of events in the log file. */
    char * numEventsSlot_;
    //! Variable holding the number of events in the log file.
    ACE_INT32 numEvents_;
    //! The length of the cdr stream
    size_t totalLength_;
    //! Flag indicating that the file is full.
    bool full_;
  };

  inline
  ACE_UINT16
  LogWriter::version() const
  {
    return LogHeader::PROTOCOL_VERSION;
  }
}
#endif
