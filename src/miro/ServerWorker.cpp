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
#include "ServerWorker.h"
#include "Client.h"
#include "Log.h"
#include "Exception.h"

#ifndef MIRO_NO_CORBA
#  include <tao/ORB_Core.h>
#  include <tao/AnyTypeCode/Any.h>
#  include <tao/BiDir_GIOP/BiDirGIOP.h>
#endif // MIRO_NO_CORBA
#include <ace/Thread_Manager.h>

namespace
{
  int dummyInt = 0;
}

static void signal_handler(int signum)
{
  // check of sigint and sigterm
  if (signum == SIGINT || signum == SIGTERM) {
    MIRO_LOG_OSTR(LL_CRITICAL, "Miro SignalHandler: Signal received (shutting down): " << signum);
    Miro::ServerWorker::shutdown();
  }
  else {
    MIRO_LOG_OSTR(LL_ERROR, "Miro SignalHanlder: Unhandled signal received (ignored): " << signum);

  }
}


namespace Miro
{
  ACE_Thread_Mutex ServerWorker::instance_mutex_;
  ServerWorker * ServerWorker::instance_ = NULL;
  bool ServerWorker::shutdown_ = false;

  ServerWorker *
  ServerWorker::instance()
  {
    ACE_Guard<ACE_Thread_Mutex> guard(instance_mutex_);
    if (NULL == instance_) {
      instance_ = new ServerWorker();
    }
    instance_->add_ref();
    return instance_;
  }

  void
  ServerWorker::shutdown() throw()
  {
    shutdown_ = true;
  }

  bool
  ServerWorker::isShutdown() throw()
  {
    return shutdown_;
  }

  /**
   * @param orb      The Object request broker.
   * @param shutdown Cooperative shutdown indicator.
   */
  ServerWorker::ServerWorker() :
#ifndef MIRO_NO_CORBA
      orb_(CORBA::ORB_init(dummyInt, NULL)),
      poa_(Miro::CORBA_dynamic_cast<PortableServer::POA>(orb_->resolve_initial_references("RootPOA"))),
      poa_mgr_(poa_->the_POAManager()),
#endif // MIRO_NO_CORBA
      ref_count_(0)
  {
    MIRO_LOG_CTOR("Miro::ServerWorker");

    shutdown_ = false;

#ifndef MIRO_NO_CORBA
    // initialize the child POA.
    init_child_poa();

    // Activate the POA manager.
    poa_mgr_->activate();
#endif // MIRO_NO_CORBA

    // register Signal handler for Ctr+C
    // Signal set to be handled by the signal handler.

    // catch all:
    //    ACE_Sig_Set ss(1);
    //    ACE_Sig_Action sa (ss, (ACE_SignalHandler) signal_handler);

    ACE_Sig_Action sa((ACE_SignalHandler) signal_handler);
    ACE_Sig_Set ss;

    ss.sig_add(SIGINT);
    ss.sig_add(SIGTERM);

    sa.mask(ss);

    sa.register_action(SIGINT);
    sa.register_action(SIGTERM);

    MIRO_LOG_CTOR_END("Miro::ServerWorker");
  }

  ServerWorker::~ServerWorker()
  {
    MIRO_LOG_DTOR("Miro::ServerWorker");

    ACE_OS::sleep(ACE_Time_Value::zero);
    shutdown();
    wait();

#ifndef MIRO_NO_CORBA

    // there were shutdown issues with this particular version of TAO
    // unfortunately it was used in the hmp field test, so we need to
    // preserve this special-casing
#if !( (TAO_MAJOR_VERSION == 1) && (TAO_MINOR_VERSION == 5) && (TAO_BETA_VERSION == 8) )
    MIRO_LOG(LL_NOTICE, "Deactivating POA manager.");
    poa_mgr_->deactivate(0, 0);
    MIRO_LOG(LL_NOTICE, "Destroying the child POA.");
    child_poa_->destroy(0, 0);
    MIRO_LOG(LL_NOTICE, "Destroying the POA.");
    poa_->destroy(0, 0);
    MIRO_LOG(LL_NOTICE, "Performing remaining work in ORB.");
    ACE_Time_Value timeSlice(0, 200000);
    orb_->perform_work(timeSlice);
#endif
    MIRO_LOG(LL_NOTICE, "Destroying the ORB.");
    orb_->shutdown(1);

#endif // MIRO_NO_CORBA

    ACE_Guard<ACE_Thread_Mutex> guard(instance_mutex_);
    instance_ = NULL;

    MIRO_LOG_DTOR_END("Miro::ServerWorker");
  }

  int
  ServerWorker::svc()
  {
    MIRO_LOG(LL_NOTICE, "Entering (detached) server loop.");
    while (!isShutdown()) {
      ACE_Time_Value timeSlice(0, 200000);
#ifndef MIRO_NO_CORBA
      orb_->perform_work(timeSlice);
#else // MIRO_NO_CORBA
      ACE_OS::sleep(timeSlice);
#endif // MIRO_NO_CORBA
    }

    // register Signal handler for Ctr+C
    // Signal set to be handled by the signal handler.
    ACE_Sig_Action sa(NULL);
    ACE_Sig_Set ss;

    ss.sig_add(SIGINT);
    ss.sig_add(SIGTERM);

    sa.mask(ss);

    sa.register_action(SIGINT);
    sa.register_action(SIGTERM);



    MIRO_LOG(LL_NOTICE, "Leaving (detached) server loop.");
    return 0;
  }

#ifndef MIRO_NO_CORBA
  // Create the default policies - user-supplied ID, and persistent
  void
  ServerWorker::init_child_poa()
  {
    // objects.
    // Id Assignment policy
    CORBA::Policy_var policy0 =
      poa_->create_id_assignment_policy(PortableServer::USER_ID);
    // Lifespan policy
    CORBA::Policy_var policy1 =
      poa_->create_lifespan_policy(PortableServer::PERSISTENT);


    CORBA::PolicyList policies;
    policies.length(2);
    policies[0] = policy0;
    policies[1] = policy1;

    child_poa_ =
      poa_->create_POA("MIRO_NP_POA", poa_mgr_, policies);

    // Destroy the policy objects
    for (CORBA::ULong i = 0; i < policies.length(); ++i) {
      policies[i]->destroy();
    }

    // Create a POA with the Bidirectional policy
    policies.length(1);
    ::CORBA::Any a;
    a <<= BiDirPolicy::BOTH;
    policies[0] = orb_->create_policy(BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE, a);

    bidir_poa_ = poa_->create_POA("MIRO_BIDIR_POA", poa_mgr_, policies);

    // Destroy the policy objects
    for (CORBA::ULong i = 0; i < policies.length(); ++i) {
      policies[i]->destroy();
    }
  }

  /**
   * Following the normal CORBA memory management rules of return
   * values from functions, this function duplicates the poa return
   * value before returning it.
   */
  PortableServer::POA_ptr
  ServerWorker::rootPoa() const
  {
    return PortableServer::POA::_duplicate(poa_.in());
  }

  /**
   * Following the normal CORBA memory management rules of return
   * values from functions, this function duplicates the child poa return
   * value before returning it.
   */
  PortableServer::POA_ptr
  ServerWorker::bdPoa() const
  {
    return PortableServer::POA::_duplicate(bidir_poa_.in());
  }

  /**
   * Following the normal CORBA memory management rules of return
   * values from functions, this function duplicates the child poa return
   * value before returning it.
   */
  PortableServer::POA_ptr
  ServerWorker::npPoa() const
  {
    return PortableServer::POA::_duplicate(child_poa_.in());
  }

  /**
   * Be careful to use this reactor with any mutex protected servant
   * code. It's easy to deadlock. Create your own reactor task
   * instead.  Anyhow, as long as you just want to use some timers
   * etc. go ahead.
   */
  ACE_Reactor *
  ServerWorker::tao_reactor()
  {
    // @@ Please see if there's a way to get to the Reactor without
    // using the TAO_ORB_Core_instance().
    return TAO_ORB_Core_instance()->reactor();
  }
#endif // MIRO_NO_CORBA

  void
  ServerWorker::add_ref()
  {
    ++ref_count_;
  }

  void
  ServerWorker::remove_ref()
  {
    unsigned int const new_count = --ref_count_;

    if (new_count == 0)
      delete this;
  }

  unsigned int
  ServerWorker::refcount_value() const
  {
    return ref_count_.value();
  }
}
