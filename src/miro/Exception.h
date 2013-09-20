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
#ifndef miro_Exception_h
#define miro_Exception_h

#include "miroBase_Export.h"

#include <exception>
#include <string>
#include <iosfwd>

#define MIRO_EXCEPTION_TYPE(N) \
  class N : public ::Miro::Exception \
  { \
    typedef ::Miro::Exception Super; \
  public: \
    N() throw(): Super() {}  \
    explicit N(char const * _what) throw() : Super(_what) {} \
    explicit N(std::string const& _what) throw() : Super(_what) {} \
    virtual ~N() throw() {} \
  }

//! The namespace of the Miro project.
/**
 * To not pollute the global namespace of users of Miro, all classes, structs
 * and helper functions are encapsulated into the namespace Miro.
 */
namespace Miro
{
  // forward declarations
  class Exception;

  //! Debug output stream operator.
  miroBase_Export std::ostream& operator << (std::ostream& ostr, const Exception& x);


  //!The root of the server side exception hierarchy.
  /**
   * This is the root class of the exceptions used within Miro Servers.
   * Exceptions that raise on the client side are CORBA exception, which are
   * defined in Exception.idl. As a pure client programmer you can
   * safely ignore this class and its derivates.
   */
  class miroBase_Export Exception : public std::exception
  {
    typedef exception Super;
  public:
    //! Default constructor.
    Exception() throw();
    //! Initializing constructor
    explicit Exception(std::string const& _what) throw();
    //! Initializing constructor for const char* (to resolve ambiguity)
    explicit Exception(const char* _what) throw();
    //! Virtual dtor.
    virtual ~Exception() throw();

    //! Standart information hook.
    virtual char const * what() const throw();

  protected:
    //! Stream output method.
    void printToStream(std::ostream& _ostr) const;

  private:
    std::string const what_;

    friend miroBase_Export std::ostream& operator << (std::ostream& ostr, const Exception& x);
  };

  //! Class to throw C errors as exceptions.
  class miroBase_Export CException : public Exception
  {
    typedef Exception Super;
  public:
    //! Initializing constructor.
    CException(int _error, const std::string& _what) throw();
    virtual ~CException() throw();
    //! C error number.
    virtual int error_num() const throw();

  protected:
    //! Stream output method.
    void printToStream(std::ostream& _ostr) const;

  private:
    int error_;
  };
}
#endif // miro_Exception_h
