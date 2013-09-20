// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2005
// Department of Neuroinformatics, University of Ulm, Germany
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
// $Id$
//
#ifndef Miro_CFG_Parser_h
#define Miro_CFG_Parser_h

#include "Type.h"

#include <qxml.h>

#include <memory>

// forward declarations
class QString;

namespace Miro
{
  namespace CFG {
    // forward declarations
    class Generator;


    class Parser : public QXmlDefaultHandler
    {
    public:
      Parser(Generator& _generator);

      bool startDocument();
      bool startElement(QString const&, QString const&, QString const& ,
                        QXmlAttributes const&);
      bool endElement(QString const&, QString const&, QString const&);

      QString errorString() const;
      bool characters(QString const& ch);

#if JSONCPP_FOUND
      void setUseJson(bool _useJson) { useJson_ = _useJson; }
      bool getUseJson(void) { return useJson_; }
#endif

    private:
      void reset();

      Type type_;
      Generator& generator_;

      QString group_;

      bool parsing_;
      bool parameterParsing_;
      bool docuParsing_;
      bool ctorParsing_;

      bool staticConst_;
      bool instance_;
      bool unmanaged_;
      bool userSingleton_;
      QString userSingletonName_;
      bool string_;
      bool enumeration_;
      bool enumerationMultiple_;
      bool vector_;
      bool set_;
      bool angle_;
      bool timeValue_;
      bool inetAddr_;
      bool serialParams_;
      bool scanDescription_;
      bool schedParams_;
      bool text_;

      QString docu_;
      QString ctor_;
      QString error_;

#if JSONCPP_FOUND
      bool useJson_;
#endif
    };
  }
}
#endif // Miro_CFG_Parser_h
