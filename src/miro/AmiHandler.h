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
#ifndef miro_AmiHandler_h
#define miro_AmiHandler_h

#include "Exception.h"

#include "miro_Export.h"

#include <ace/Synch.h>

#include <tao/Messaging/MessagingS.h>

#define GENERIC_EXCEPTION_CALLBACK_DECL(METHOD_NAME) \
  virtual void METHOD_NAME ## _excep (::Messaging::ExceptionHolder * excep_holder) throw ()
#define GENERIC_EXCEPTION_CALLBACK_IMPL(CLASS_NAME, METHODNAME) \
  void CLASS_NAME :: METHODNAME ## _excep (::Messaging::ExceptionHolder * excep_holder) throw () \
  { \
    finishByException(excep_holder); \
  }
#define GENERIC_VOID_CALLBACK_DECL(METHOD_NAME, EXCEPTIONS)	\
  virtual void METHOD_NAME () throw EXCEPTIONS
#define GENERIC_VOID_CALLBACK_IMPL(CLASS_NAME, METHODNAME, EXCEPTIONS) \
  void CLASS_NAME :: METHODNAME () throw EXCEPTIONS \
  { \
    finish(); \
  }
#define GENERIC_TYPED_CALLBACK_DECL(METHOD_NAME, RETURN_TYPE, EXCEPTIONS) \
  virtual void METHOD_NAME (RETURN_TYPE) throw EXCEPTIONS
#define GENERIC_TYPED_CALLBACK_IMPL(CLASS_NAME, METHODNAME, RETURN_TYPE, EXCEPTIONS) \
  void CLASS_NAME :: METHODNAME (RETURN_TYPE rc) throw EXCEPTIONS \
  { \
    finish(rc); \
  }

namespace Miro
{
  MIRO_EXCEPTION_TYPE(ENotYetCompleted);
  MIRO_EXCEPTION_TYPE(ETimeout);
  MIRO_EXCEPTION_TYPE(EInterrupted);

  //! Base class for a generic AMI callback handler.
  /** This class it not for client side usage, but a mix-in helper
   * class for implementing a generic AMI callback handler.
   *
   * It provides a polling model of AMI through the callback.
   *
   * This is a mixin class for the AMI handler implementation.  It's
   * inteded usage is through the AmiHelper template class and in
   * conjunction with the helper macros, for generic callback
   * implemenations.
   *
   * See examples/ami for an application example of this class.
   */
  class miro_Export AmiHandlerBase : public virtual POA_Messaging::ReplyHandler
  {
  public:
    //! Initializing default ctor.
    AmiHandlerBase() throw();
    //! Resource cleanup.
    virtual ~AmiHandlerBase();

    //! Returns true, if the call already completed.
    bool finished() const throw();
    //! Waits for the completion of the call.
    /** A timeout of 0 (default argument), disables the timout.
     * @throws ETimeout, if the wait timed out.
     * @throws EInterrupted, if the wait was interrupted by a signal.
     */
    void wait(ACE_Time_Value const& timeout = ACE_Time_Value::zero) const throw(Miro::ETimeout, Miro::EInterrupted);

    //! Query method for return with exception.
    /** Delivers the exception, if one was thrown.
     */
    void throwOnException() const throw(CORBA::Exception, ENotYetCompleted);

    //! Default implementation for an exception callback.
    void finishByException(Messaging::ExceptionHolder * exception_holder) throw();
    //! Default implementation for completion of a method returning void.
    /** Deregisters the servant from the POA. */
    void finish() throw();

  protected:
    mutable ACE_Thread_Mutex _mutex;
    mutable ACE_Condition_Thread_Mutex _condition;

    //! Set to true, if the call already completed.
    bool _finished;
    //! Holds the exception, if one occured.
    Messaging::ExceptionHolder_var _exception_holder;
  };

  inline
  bool
  AmiHandlerBase::finished() const throw()
  {
    return _finished;
  }

  //----------------------------------------------------------------------------

  //! AMI callback handler for a method returning void.
  class miro_Export VoidAmiHandler : public AmiHandlerBase
  {
    typedef AmiHandlerBase Super;
  public:
    //! The type of the return parameter of the method.
    /** This is obviously void in this case. */
    typedef void Return_Type;
    //! Query method for the return code.
    /** Delivers the exception, if one is thrown.
     */
    void rc() const throw(CORBA::Exception, ENotYetCompleted);

  protected:
    //! Refinement of the method completion.
    void finish() throw();
  };

  inline
  void
  VoidAmiHandler::rc() const throw(CORBA::Exception, ENotYetCompleted)
  {
    throwOnException();
  }

  //----------------------------------------------------------------------------

  //! AMI callback handler for a method returning a single parameter.
  template<class RT>
  class TypedAmiHandler : public AmiHandlerBase
  {
    typedef AmiHandlerBase Super;
  public:
    //! The type of the return parameter of the method.
    typedef RT Return_Type;

    //! Query method for the return code.
    /** Returns the return value of the method.
     * Delivers the exception, if one is thrown.
     */
    Return_Type const& rc() const throw(CORBA::Exception, ENotYetCompleted);

  protected:
    //! Refinement of the method completion for a single return parameter.
    void finish(Return_Type const& rc) throw();

    Return_Type _rc;
  };

  template<class RT>
  void
  TypedAmiHandler<RT>::finish(Return_Type const& rc) throw()
  {
    {
      ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
      _rc = rc;
      _finished = true;
      _condition.broadcast();
    }
    Super::finish();
  }

  template<class RT>
  RT const&
  TypedAmiHandler<RT>::rc() const throw(CORBA::Exception, ENotYetCompleted)
  {
    ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
    if (!_finished)
      throw ENotYetCompleted("AMI call did not complete, yet.");
    if (_exception_holder != NULL)
      _exception_holder->raise_exception();
    return _rc;
  }
}

#endif // miro_AmiHandler_h
