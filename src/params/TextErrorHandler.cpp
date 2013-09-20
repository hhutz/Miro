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
#include "TextErrorHandler.h"

#include <sstream>

#include "qt_compatibility.h"

namespace Miro
{
  namespace CFG {
    TextErrorHandler::~TextErrorHandler()
    {
    }

    bool
    TextErrorHandler::warning(const QXmlParseException & exception)
    {
      createErrorMessage("Warning: ", exception);
      return false;
    }

    bool
    TextErrorHandler::error(const QXmlParseException & exception)
    {
      createErrorMessage("Error: ", exception);
      return false;
    }

    bool
    TextErrorHandler::fatalError(const QXmlParseException & exception)
    {
      createErrorMessage("Fatal error: ", exception);
      return false;
    }

    QString
    TextErrorHandler::errorString()
    {
      return error_;
    }

    QString
    TextErrorHandler::errorString() const
    {
      return error_;
    }

    void
    TextErrorHandler::createErrorMessage(const QString& type, const QXmlParseException & exception)
    {
      std::stringstream ostr;
      ostr << type
      << " in line " << exception.lineNumber() << " "
      << ", column " << exception.columnNumber() << std::endl
      //       << "public id: " << exception.publicId() << endl
      //       << "system id: " << exception.systemId() << endl
      << exception.message() << std::endl;

      error_ = ostr.str().c_str();
    }
  }
}
