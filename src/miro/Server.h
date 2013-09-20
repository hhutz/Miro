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
#ifndef miro_Server_h
#define miro_Server_h

#include "miro_Export.h"

#include "Client.h"

#include <vector>

namespace Miro
{
  // forward declaration
  class ServerWorker;
  class ServerData;

  //! The server helper class.
  /** You normally need an instance of this class, if you want to
   * initialze your ORB for use as a Server.  It provides helper
   * methods for registering and activating your servant objects at a
   * POA and for running the ORB event loop with multiple threads and
   * also in the background, if required.
   *
   * Note that you can have multiple server objects. But make sure,
   * you passed the command line parameters to the first instance, if
   * you want to allow the processing of ORB command line parameters.
   *
   * Note that the Server helper class deactivates all objects from
   * the POA, that where activated through its activate_... member
   * methods on destruction.
   */
  class miro_Export Server : public Client
  {
    typedef Client Super;
  public:

    //! Constructor.
    Server(int& argc = _dummy_argc, ACE_TCHAR *argv[] = NULL);
    //! Resource cleanup.
    virtual ~Server();

    //! Executes the server main routine.
    void run(unsigned int nthreads = 1);
    //! Detaches the server main routine.
    void detach(unsigned int nthreads = 1);
    //! Wait for server main loop completion.
    void wait();

    //! Accessor to the ServerWorker instance, which holds the CORBA details.
    ServerWorker * worker() throw() {
      return _worker;
    }

    //! Shutdown the server threads.
    /** All server threads will return. Even the once spawned through another
     * instance of the Server class.
     */
    virtual void shutdown();

    bool isShutdown() const throw();

    /**
     * Activate @param servant, using the POA <activate_object> call.  Users
     * can call this method multiple times to activate multiple objects.
     *
     * @param servant  The servant to activate under the default POA.
     * @param pass_ownership Pass the ownership of the @ref servant instance
     *                     to the server. This requires the servant instance to
     *                     be heap-allocated.  The server object will then ensure
     *                     propper resource cleanup on destruction. - If for
     *                     some reason you do not want to pass
     *                     ownership, simply pass false as parameter
     *                     But you then have to make sure, that the @ref servant object
     *                     lifetime exceeds that of the @ref Server instance.
     */
    void activate(PortableServer::ServantBase * servant,
                  bool pass_ownership = true);

    /**
     * Activate @param servant, using the POA <activate_object> call with
     * a portable object adapter initialized with a bidir policy.  Users
     * can call this method multiple times to activate multiple objects.
     *
     * @param servant  The servant to activate under the bidir POA.
     * @param pass_ownership Pass the ownership of the @ref servant instance
     *                     to the server. This requires the servant instance to
     *                     be heap-allocated.  The server object will then ensure
     *                     propper resource cleanup on destruction. - If for
     *                     some reason you do not want to pass
     *                     ownership, simply pass false as parameter
     *                     But you then have to make sure, that the @ref servant object
     *                     lifetime exceeds that of the @ref Server instance.
     */
    void activateBiDir(PortableServer::ServantBase * servant,
                       bool pass_ownership = true);

    /**
     * Activate @param servant
     * using the POA <activate_object_with_id> created from the string
     * @param object_name. Users should call this to activate objects under
     * the child_poa.
     *
     * By default, the ownership of the servant object is passed to
     * the server. So make sure, to heap-allocate your servant. The
     * server object will ensure propper resource cleanup on
     * destruction. of the method
     * call.
     *
     * @param object_name  String name which will be used to create
     *                     an Object ID for the servant. This represents also
     *                     the name used in the naming service.
     * @param servant  The servant to activate under the child POA.
     * @param pass_ownership Pass the ownership of the @ref servant instance
     *                     to the server. This requires the servant instance to
     *                     be heap-allocated.  The server object will then ensure
     *                     propper resource cleanup on destruction. - If for
     *                     some reason you do not want to pass
     *                     ownership, simply pass false as parameter
     *                     But you then have to make sure, that the @ref servant object
     *                     lifetime exceeds that of the @ref Server instance.
     */
    void activateNamedObject(std::string const& object_name,
                             PortableServer::ServantBase * servant,
                             bool pass_ownership = true);

    //! Register an object reference at the CORBA naming service.
    void addToNameService(CORBA::Object * _obj, const std::string& _name);

  protected:
    //! Shared worker thread pool.
    ServerWorker * const _worker;
    //! Hidden server data.
    ServerData * const _data;

  private:
    //! Copy construction prohibited.
    Server(Server const&) : Client(), _worker(NULL), _data(NULL) {}
    //! Assignement operation prohibited.
    Server & operator = (Server const &) {
      return *this;
    }
  };
}
#endif // miro_Server_h




