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
#ifndef Miro_Configuration
#define Miro_Configuration

#include "Exception.h"
#include "Singleton.h"
#include "ConfigDocument.h"

#include "miroXml_Export.h"

namespace Miro
{
  //! Class holding static methods for initializing and accessing the configuration.
  struct miroXml_Export Configuration {
    //! Initializing the document.
    /**
     * Loading the document from default location if not overridden
     * by command line parameter -MiroConfigFile (-MCF).
     */
    static void init(int& argc, char * argv[]) throw(Exception);
    //! Singleton accesor for the robots configuation document.
    static Singleton<ConfigDocument> document;
  };

  typedef ACE_Singleton<ConfigDocument, ACE_SYNCH_RECURSIVE_MUTEX> ConfigDocumentSingleton;
}

MIROXML_SINGLETON_DECLARE(ACE_Singleton, Miro::ConfigDocument, ACE_SYNCH_RECURSIVE_MUTEX);

#endif // Miro_Configuration
