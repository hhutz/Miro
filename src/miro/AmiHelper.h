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
#ifndef miro_AmiHelper_h
#define miro_AmiHelper_h

#include "AmiHandler.h"

#include "miro_Export.h"

namespace Miro
{
  class miro_Export AmiHelperBase
  {
  protected:
    AmiHelperBase() {}
  public:
    virtual ~AmiHelperBase() throw();
    //! Returns true, if the hander already finished.
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
    void throwOnException() const throw(CORBA::Exception, Miro::ENotYetCompleted);

  protected:
    //! Pointer to the actual AMI callback handler servant.
    AmiHandlerBase * _handler;
  };

  //! Helper class for managing the generic AMI callback implemenations.
  /** This class provides a generic interface for the polling the
   * status of an asynchronously executed method call. It becomes
   * instanciated with a generic callback implementation based upon
   * the Miro::AmiHandler class. The user creates an instance of a such
   * instanciated AmiHelper and uses it for polling the callback
   * status.
   *
   * The class ensures propper resource management of the callback
   * servant instance. It allows default construction,
   * copy-construction and assignement for propper use with stl
   * containers.
   *
   * Calling the handler() method implicitly registers the contained
   * callback servant at the default POA in the ORB. The servant
   * object deregisters itself, after processing of the callback. So
   * if for some reason you descide to not use the callback helper for
   * an AMI call, after calling the handler() method, you better
   * deregister this object from the POA by yourself. (If you do not
   * understand the last paragraph and follow the usage illustrated in
   * the example, you probably don't have to worry about anything
   * discussed there.)
   *
   * @param AMI_HANDLER A AMI callback implemenation, based upon the Miro::AmiHandler mix-in class.
   */
  template<class AMI_HANDLER>
  class AmiHelper : public AmiHelperBase
  {
  public:
    //! The type of the hander implementation.
    typedef AMI_HANDLER Handler_Type;
    //! The return type of the AMI method.
    typedef typename AMI_HANDLER::Return_Type Return_Type;

    //! Initializing ctor.
    /** Instanciates a callback servant and activtes it at the default POA. */
    AmiHelper();
    //! Copy constructor.
    /** If we want to allow storing such stuff in stl containers,
     * we better make our homework.
     */
    AmiHelper(AmiHelper const& rhs) throw();
    //! Resource cleanup.
    /** If we want to allow storing such stuff in stl containers,
     * we better make our homework.
     */
    /** Diminishes the ref count on the callback servant. */
    virtual ~AmiHelper() throw();
    // Assignement operator.
    AmiHelper& operator = (AmiHelper const& rhs) throw();
    Return_Type rc() const throw(CORBA::Exception);

    //! Accessor for the AMI handler.
    /** The handler reference is passed as first argument to an AMI call. */
    typename Handler_Type::_stub_ptr_type handler() const;
  };

  template<class AMI_HANDLER>
  inline
  AmiHelper<AMI_HANDLER>::AmiHelper()
  {
    // create handler instance
    _handler = new Handler_Type();
  }

  template<class AMI_HANDLER>
  inline
  AmiHelper<AMI_HANDLER>::AmiHelper(AmiHelper const& rhs) throw()
  {
    // copy handler instance reference
    _handler = rhs._handler;
    // increase reference count.
    _handler->_add_ref();
  }

  template<class AMI_HANDLER>
  inline
  AmiHelper<AMI_HANDLER>::~AmiHelper() throw()
  {
    // unblock waiting handlers.
    _handler->finish();
    // diminish reference count on handler
    _handler->_remove_ref();
  }

  template<class AMI_HANDLER>
  inline
  AmiHelper<AMI_HANDLER> &
  AmiHelper<AMI_HANDLER>::operator = (AmiHelper const& rhs) throw()
  {
    if (this != rhs) {
      _handler->_remove_ref();
      _handler = rhs._handler;
      _handler->_add_ref();
    }
    return *this;
  }

  template<class AMI_HANDLER>
  inline
  typename AmiHelper<AMI_HANDLER>::Return_Type
  AmiHelper<AMI_HANDLER>::rc() const throw(CORBA::Exception)
  {
    return dynamic_cast<AMI_HANDLER *>(_handler)->rc();
  }

  template<class AMI_HANDLER>
  inline
  typename AmiHelper<AMI_HANDLER>::Handler_Type::_stub_ptr_type
  AmiHelper<AMI_HANDLER>::handler() const
  {
    return dynamic_cast<AMI_HANDLER *>(_handler)->_this();
  }
}

#endif // miro_AmiHelper_h
