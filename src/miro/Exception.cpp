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
#include "Exception.h"

#include <iostream>

namespace Miro
{
  /**
   * This operator pipes a Miro::Exception to a specified ostream.
   *
   * @return Reference to the ostream.
   */
  std::ostream& operator << (std::ostream& _ostr, const Exception& x)
  {
    x.printToStream(_ostr);
    return _ostr;
  }

  /** Noop implementation. */
  Exception::Exception() throw() :
      Super(),
      what_()
  {}

  /**
   * You can pass this constructor a string, which describes the
   * cause of the exception.
   */
  Exception::Exception(const std::string& _what) throw() :
      Super(),
      what_(_what)
  {}

  /**
   * You can pass this constructor a string, which describes the
   * cause of the exception.
   */
  Exception::Exception(const char* _what) throw() :
      Super(),
      what_(_what)
  {}

  /** Noop implementation. */
  Exception::~Exception() throw()
  {}

  /**
   * @return A pointer to the string describing the exception
   * reason.  This string is set when the exception is thrown.
   */
  char const *
  Exception::what() const throw()
  {
    return what_.c_str();
  }

  /**
   * This method pipes a Miro::Exception to a specified ostream.
   *
   * @return Reference to the ostream.
   */
  void
  Exception::printToStream(std::ostream& _ostr) const
  {
    _ostr << what();
  }

  /**
   * Pass the value of errno and a string describing the cause for this exception
   * to be thrown to this constructor.
   */
  CException::CException(int _error, const std::string& _what) throw() :
      Super(_what),
      error_(_error)
  {}

  /** Noop implementation. */
  CException::~CException() throw()
  {}

  /**
   * Returns a number describing the exception reason.
   * This number is set when the exception is thrown.
   */
  int CException::error_num() const throw()
  {
    return error_;
  }

  /**
   * This method pipes a Miro::CException to a specified ostream.
   *
   * @return Reference to the ostream.
   */
  void
  CException::printToStream(std::ostream& _ostr) const
  {
    _ostr << what() << " - "
    << error_num() << ": " << ::strerror(error_num());
  }
}
