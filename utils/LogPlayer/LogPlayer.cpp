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
#include "ChannelManager.h"
#include "FileSet.h"
#include "MainForm.h"

#include "miro/Exception.h"
#include "miro/NotifySvc.h"

#include <ace/Arg_Shifter.h>

#include <orbsvcs/Notify/Notify_Default_CO_Factory.h>
#include <orbsvcs/Notify/Notify_Default_POA_Factory.h>
#include <orbsvcs/Notify/Notify_Default_Collection_Factory.h>
#include <orbsvcs/Notify/Notify_Default_EMO_Factory.h>

#include <qapplication.h>
#include <qstring.h>
#include <qstringlist.h>

#include <iostream>

#include "LOG128.xpm"
#define ICON_XPM_NAME LOG128_xpm
#include <QPixmap>
#include <QIcon>

using namespace std;

namespace
{
  QString channelName = "EventChannel";
  bool verbose = false;
  bool shared = false;
  bool unified = false;
  bool localizeDebug = false;
  bool quit = false;
  bool fast = false;
  int consumers = -1;
  bool gui = true;
  bool asyncDispatching = false;

  QStringList excludeEvents;

  const char eventChannelOpt[] = "-event_channel";
  const char colocatedOpt[] = "-shared_ec";
  const char unifiedOpt[] = "-unified_ec";
  const char noGuiOpt[] = "-t";
  const char excludeOpt[] = "-x";
  const char autoStartOpt[] = "-a";
  const char asyncDispatchingOpt[] = "-d";
  const char fastOpt[] = "-f";
  const char quitOpt[] = "-q";
  const char verboseOpt[] = "-v";
  const char localizeOpt[] = "-l";
  const char helpOpt[] = "-?";
};

int
main(int argc, char *argv[])
{
  int rc = 0;
  try { // first try
    Miro::Log::init(argc, argv); // init log output
    Miro::Server server(argc, argv); // init orb, parsing command line options

    // parse optional args
    ACE_Arg_Shifter arg_shifter (argc, argv);
    while (arg_shifter.is_anything_left ()) {
      char *current_arg = (char *)arg_shifter.get_current ();

      if (ACE_OS::strcasecmp(current_arg, colocatedOpt) == 0) {
        arg_shifter.consume_arg();
        shared = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, eventChannelOpt) == 0) {
        arg_shifter.consume_arg();
        channelName = arg_shifter.get_current();
        arg_shifter.consume_arg();
      }
      else if (ACE_OS::strcasecmp(current_arg, excludeOpt) == 0) {
        arg_shifter.consume_arg();
        excludeEvents.push_back(arg_shifter.get_current());
        arg_shifter.consume_arg();
      }
      else if (ACE_OS::strcasecmp(current_arg, unifiedOpt) == 0) {
        arg_shifter.consume_arg();
        unified = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, localizeOpt) == 0) {
        arg_shifter.consume_arg();
        localizeDebug = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, autoStartOpt) == 0) {
        arg_shifter.consume_arg();
        consumers = atoi(arg_shifter.get_current());
        arg_shifter.consume_arg();
        if (consumers < 0) {
          return 1;
        }
      }
      else if (ACE_OS::strcasecmp(current_arg, verboseOpt) == 0) {
        arg_shifter.consume_arg();
        verbose = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, asyncDispatchingOpt) == 0) {
        arg_shifter.consume_arg();
        asyncDispatching = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, fastOpt) == 0) {
        arg_shifter.consume_arg();
        fast = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, noGuiOpt) == 0) {
        arg_shifter.consume_arg();
        gui = false;
      }
      else if (ACE_OS::strcasecmp(current_arg, quitOpt) == 0) {
        arg_shifter.consume_arg();
        quit = true;
      }
      else if (ACE_OS::strcasecmp(current_arg, helpOpt) == 0) {
        arg_shifter.consume_arg();
        std::cerr << "usage: " << argv[0] << "[-shared_ec name] [-unified_ec] [-a <n>] [-x <type_name>] [-ftqv?]" << std::endl
                  << "  -shared_ec use existing event channels" << std::endl
                  << "  -event_channel the name of the event channels (default: EventChannel)" << std::endl
                  << "  -unified_ec use one event channel for all robots" << std::endl
                  << "  -x <type_name> exclude events with specified <type_name> from replay" << std::endl
                  << "  -a <n> auto-start as soon as n consumers have connected" << std::endl
                  << "  -d sync dispatching" << std::endl
                  << "  -f fast mode: play as fast as possible" << std::endl
                  << "  -q quit on completion of run" << std::endl
                  << "  -t text mode, no gui window" << std::endl
                  << "  -v verbose mode" << std::endl
                  << "  -? help: emit this text and stop" << std::endl;
        return 0;
      }
      else
        arg_shifter.ignore_arg ();
    }

    if (verbose) {
      if (shared)
        std::cout << "Using a shared event channel factory." << std::endl;
      if (unified)
        std::cout << "Using a unified event channel factory." << std::endl;
    }


    int ac = 4;
    char * av[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    av[0] = argv[0];
    av[1] = strdup("-f");
    av[2] = strdup(channelName + "Factory");
    av[3] = strdup("-C"); // no default channel
    if (verbose) {
      ++ac;
      av[ac - 1] = strdup("-v");
    }

    Miro::NotifySvc notifySvc;
    if (!shared) {
      // set up a notification service manually
      if (asyncDispatching) {
        static const char directive[] = "static Notify_Default_Event_Manager_Objects_Factory \"-DispatchingThreads 1\"";
        if (ACE_Service_Config::process_directive(directive)) {
          cerr << "failed to process async dispatching" << endl;
          return 1;
        }
      }

      notifySvc.init(ac, av);
    }

    for (int i = 1; i < ac; ++i) {
      free(av[i]);
    }

    // Create Miro server
    if (verbose)
      std::cout << "Initialize server daemon." << std::endl;

    ChannelManager channelManager(argc, argv, shared, unified, channelName);
    //    channelManager.setDebugLoclaize(localizeDebug);
    try { // second try
      QApplication app(argc, argv);     // Create Qt application
      FileSet fileSet(&channelManager);
      MainForm mainWindow(app, fileSet);

      QPixmap iconPix(ICON_XPM_NAME);
      QIcon appIcon(iconPix);
      mainWindow.setWindowIcon(appIcon);

      // Scope operator to delimit the lifetime
      // of MyWidget, to prevent conflicts with CORBA cleanup
      {
        if (verbose)
          std::cout << "set main widget" << std::endl;
        app.setMainWidget( &mainWindow );
        QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
        if (gui) {
          mainWindow.show();
        }
        channelManager.detach(4);

        // parse remaining args
        for (int i = 1; i < argc; ++i) {
          if (verbose)
            std::cout << "using file " << argv[i] << std::endl;
          mainWindow.loadFile(argv[i]);
        }
        if (verbose)
          std::cout << "files loaded." << std::endl;

        if (excludeEvents.size() > 0) {
          if (verbose)
            std::cout << "adding excluded events" << std::endl;
          for (int i = 0; i < excludeEvents.size(); ++i) {
            if (verbose)
              std::cout << qPrintable(excludeEvents[i]) << std::endl;
            mainWindow.addExclude(excludeEvents[i]);
          }
        }

        // set exit strategy
        mainWindow.exitOnReplayEnd(quit);
        // set fast mode
        if (fast) {
          mainWindow.setSpeed(20);
        }
        if (consumers >= 0) {
          mainWindow.setStartOnConsumers(consumers);
        }


        if (verbose)
          std::cout << "exec application" << std::endl;
        app.exec();
      }

      channelManager.shutdown();
      channelManager.wait();
    }
    catch (const CORBA::Exception& e) {
      std::cerr << "CORBA exception: " << e << std::endl;
      rc = 1;
    }

    if (!shared)
      notifySvc.fini();
  }
  catch (Miro::Exception const& e) {
    std::cerr << "Miro exception: " << e << std::endl;
    rc = 1;
  }
  catch (const CORBA::Exception& e) {
    std::cerr << "CORBA exception: " << e << std::endl;
    rc = 1;
  }
  catch (...) {
    std::cerr << "Unknown exception." << std::endl;
    rc = 1;
  }

  return rc;
}
