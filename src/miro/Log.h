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
#ifndef miro_Log_h
#define miro_Log_h

#include "miroCore_Export.h"

#include <ace/Log_Msg.h>

#include <sstream>
#include <cassert>

// Macro definitions for logging and debug information
//
// MIRO_ASSERT: the standart assert macro.
// It is provided to enable disabling of assert macros in inline code
// of Miro without disabling them for user code too.
//
// MIRO_LOG:
// The first argument is the log level/priority.
// The second argument is (Category, Message);
//
// MIRO_LOG_OSTR: a logging macro, providing an ostream.
// This enables the use of ostream operators for logging output.
// The data is first buffered in a stringstream.
//
// The MIRO_DBG* macros are used so we can separate
// debug and logging output.
//
// MIRO_DBG: the standart ACE_LOG macro syntax.
// The first argument is the log level/priority.
// The second argument is (Category, Message, ...);
//
// MIRO_DBG_OSTR: a logging macro, providing an ostream.
// This enables the use of ostream operators for logging output.
// The data is first buffered in a stringstream.
//
// MIRO_DBG_HEX_DUMP: logging of a hex dump.
// Untested up to now.
//
// MIRO_DBG_TRACE: trace macro.
// It uses the ctor/dtor of a local variable to
// set/reset the trace depth.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_WIN32)
//# define MIRO_FUNCTION __FUNCTION__ VC2005: predefined macro cannot appear outside function body
#  define MIRO_FUNCTION "UNKNOWN"
#elif defined(__STDC__)
# if __STDC_VERSION__ - 0 >= 199901L /* c99 says this is here */
#   define MIRO_FUNCTION __func__
# elif defined(__GNUC__)   /* less than C99, try GCC extension */
#   define MIRO_FUNCTION __PRETTY_FUNCTION__
# else
#   define MIRO_FUNCTION "UNKNOWN"
# endif
#else
# define MIRO_FUNCTION "UNKNOWN"
#endif

#if defined (MIRO_NO_DEBUG)
#define MIRO_ASSERT(X) do {} while (0)
#elif !defined (MIRO_ASSERT)
#define MIRO_ASSERT(X) assert(X)
#endif  // MIRO_NO_DEBUG

#if defined (MIRO_NO_LOGGING)

#define MIRO_LOG(L, X) do {} while (0)
#define MIRO_LOG_OSTR(L, O) do {} while (0)
#define MIRO_DBG(Category, Loglevel, X) do {} while (0)
#define MIRO_DBG_OSTR(Category, Loglevel, O) do {} while (0)
#define MIRO_DBG_HEX_DUMP(N, X) do {} while (0)
#define MIRO_DBG_TRACE(X) do {} while (0)

#else // !MIRO_NO_LOGGING

#define MIRO_LOG(Loglevel, Output) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::Loglevel) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->conditional_set (__FILE__, __LINE__, 0, 0); \
      ace___->log(::Miro::Log::ll2LM(::Miro::Log::Loglevel), \
                  ::Miro::Log::format(), Output); \
    } \
  } while (0)
#define MIRO_LOG_OSTR(Loglevel, Output) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::Loglevel) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      std::ostringstream ostr__; \
      ostr__ << Output; \
      ace___->log(::Miro::Log::ll2LM(::Miro::Log::Loglevel), \
                  ::Miro::Log::format(), ostr__.str().c_str()); \
    } \
  } while (0)

#if defined (MIRO_NO_DEBUG)

#define MIRO_DBG(L, C, X) do {} while (0)
#define MIRO_DBG_OSTR(L, C, O) do {} while (0)
#define MIRO_DBG_TRACE(X) do {} while (0)

#else // !MIRO_NO_DEBUG

#define MIRO_DBG(Category, Loglevel, X) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::Loglevel && \
	::Miro::Log::enabled(::Miro::LC::Category)) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->log(::Miro::Log::ll2LM(::Miro::Log::Loglevel), \
		  ::Miro::Log::format(), X); \
    } \
  } while (0)
#define MIRO_DBG_OSTR(Category, Loglevel, O) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::Loglevel && \
	::Miro::Log::enabled(::Miro::LC::Category)) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      std::ostringstream ostr__; \
      ostr__ << O; \
      ace___->log(static_cast<ACE_Log_Priority>(::Miro::Log::ll2LM(::Miro::Log::Loglevel)), \
		  ::Miro::Log::format(), ostr__.str().c_str()); \
    } \
  } while (0)
// Debug HexDump
#define MIRO_DBG_HEX_DUMP(L, X) \
  do { \
    unsigned int n__ = L; \
    if (::Miro::Log::level() >= n__) { \
     int __ace_error = ACE_OS::last_error (); \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
      ace___->log_hexdump X; \
    } \
  } while (0)
// Constructor log message (Miro::Log::level() >= 1)
#define MIRO_LOG_CTOR(X) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::LL_CTOR_DTOR) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->conditional_set (__FILE__, __LINE__, 0, 0); \
      ace___->log(LM_DEBUG, "Constructing " X ".\n"); \
    } \
  } while (0)
// Constructor log message (Miro::Log::level() >= 1)
#define MIRO_LOG_CTOR_END(X) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::LL_CTOR_DTOR) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->conditional_set (__FILE__, __LINE__, 0, 0); \
      ace___->log(LM_DEBUG, "Construction of " X " finished.\n"); \
    } \
  } while (0)
// Destructor log message (Miro::Log::level() >= 1)
#define MIRO_LOG_DTOR(X) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::LL_CTOR_DTOR) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->conditional_set (__FILE__, __LINE__, 0, 0); \
      ace___->log(LM_DEBUG, "Destructing "  X ".\n"); \
    } \
  } while (0)
// Destructor log message (Miro::Log::level() >= 1)
#define MIRO_LOG_DTOR_END(X) \
  do { \
    if (::Miro::Log::level() >= ::Miro::Log::LL_CTOR_DTOR) { \
      ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
      ace___->conditional_set (__FILE__, __LINE__, 0, 0); \
      ace___->log(LM_DEBUG, "Destruction of " X " finished.\n"); \
    } \
  } while (0)

#define MIRO_DBG_TRACE(X) \
  ::Miro::Trace X


#endif // !MIRO_NO_DEBUG

#endif // !MIRO_NO_LOGGING

namespace Miro
{
  namespace LC {
    //! Log category of the miro core components.
    static unsigned int const MIRO = 0x00000800;
    //! Log category of the notify components.
    static unsigned int const NMC =  0x00001000;
    //! Text version of the category.
    extern ACE_TCHAR const * const MIRO_NAME;
    //! Text version of the category.
    extern ACE_TCHAR const * const NMC_NAME;
  }

  //! Logging facilities of the MIRO project.
  /**
   * MIRO uses the ACE_Log framework for logging purposes.
   * However it extends the ACE_LOG macros in order to provide a more
   * elaborate set of logging facilities.
   *
   * MIRO logging facilities use two variables to determine log
   * verbosity, the log level, and the log category. These can be
   * specified through command line parameters by the user. Each
   * message, who's priority lies below the selected log level or
   * who's category is not for output is rejected.
   */
  class miroCore_Export Log
  {
  public:
    //! Initializethe logging module.
    static void init(int& argc, ACE_TCHAR * argv[]);
    //! Query the current logging level.
    static int level() throw();
    //! Set the current logging level.
    static void level(int _level) throw();
    //! Query the current logging maks.
    static unsigned int mask() throw();
    //! Set the current logging level.
    static void mask(unsigned int _mask) throw();
    //! Query if selected priority is currently enabled.
    static bool enabled(int _prioriy);
    //! Query if Miro was compiled with logging support.
    static bool compiledWithLog();
    //! Query if Miro was compiled with debug support.
    /** This requires logging support. */
    static bool compiledWithDebug();
    //! Helper method to convert a Miro LogLevel into an ACE LogMessage
    static ACE_Log_Priority ll2LM(int _level);
    /** Setting the log format.
     * IMPORTANT: <format> must be a statically allocated const ACE_TCHAR*
     */
    static void format(ACE_TCHAR const * _format);
    //! Accessor returning a pointer to the current log format.
    static char const * format();
    /** Register an additonal log category.
     * IMPORTANT: <name> must be a statically allocated const ACE_TCHAR*
     */
    static void register_category(unsigned int _category, ACE_TCHAR const * _name);

    //! Log level of messages reporting an emergency.
    /**
     * Your robot is on fire etc. This log level is not maskable,
     * except if you turn of logging at configure time.
     */
    static signed int const LL_EMERGENCY = 0;
    //! Log level of messages reporting an alert.
    static signed int const LL_ALERT = 1;
    //! Log level of messages reporting a critical condition.
    /**
     * This usually is an unrecoverable error, that leads to
     * the termination of reporting program.
     */
    static signed int const LL_CRITICAL = 2;
    //! Log level of messages reporting an error.
    /**
     * This indicates a real error, but the program will usually
     * try to contiue anyway.
     */
    static signed int const LL_ERROR = 3;
    //! Log level of messages reporting a warning.
    /**
     * A warning should be fixed, but the program is likely work
     * anyways.
     */
    static signed int const LL_WARNING = 4;
    //! Log level of messages reporting a notice.
    /** Make a postit and add it to the other 500 ones. */
    static signed int const LL_NOTICE = 5;
    //! Debug level of messages reporting a constructor/destructor entry.
    /**
     * This debug level is designed to hunt segfaults on startup and
     * exit. - This is when all the big ctors/dtors are run.
     */
    static signed int const LL_CTOR_DTOR = 6;
    //! Log level of messages reporting debug output.
    static signed int const LL_DEBUG = 7;
    //! Log level of messages reporting program trace output.
    static signed int const LL_TRACE = 8;
    //! Log level of messages reporting really verbose comments on the progam execution.
    static signed int const LL_PRATTLE = 9;

    //! The highest log level.
    static signed int const MAX_LOG_LEVEL = LL_CTOR_DTOR;
    //! The lowest deug level.
    static signed int const MIN_DBG_LEVEL = LL_DEBUG;
    //! The highest debug level.
    static signed int const MAX_DBG_LEVEL = LL_PRATTLE;

    //! Helper class for method traces.
    class Trace
    {
    public:
      Trace(unsigned int _level = LL_TRACE,
            int _priority = LM_DEBUG,
            char const * _fun = MIRO_FUNCTION);
      ~Trace();
    protected:
      //! Remember the log level for the dtor.
      unsigned int level_;
      //! Remember the cathegory for the dtor.
      int priority_;
      //! Remember the method name for the dtor.
      char const * const fun_;
    };

  protected:

    //! Default is LL_WARNING - 4.
    static unsigned int level_;
    //! Default is MIRO.
    static unsigned int mask_;
    static char const * format_;
    static ACE_Log_Priority aceLM_[MAX_DBG_LEVEL + 1];
  };

  /**
   * It reports to the logging framework in its ctor and dtor.
   * Instance this class via the MIRO_TRACE macro on the beginning
   * of your method and you'll have trace log messages on every
   * execution of the method.
   */
  inline
  Log::Trace::Trace(unsigned int _level, int _priority, char const * _fun) :
      level_(_level),
      priority_(_priority),
      fun_(_fun)
  {
//    MIRO_DBG(level_, priority_, std::string("Entering ") + fun_);
  }
  inline
  Log::Trace::~Trace()
  {
//    MIRO_DBG(level_, priority_, std::string("Leaving ") + fun_);
  }
  /** The default logging level is @ref LL_WARNING (4). */
  inline
  int
  Log::level() throw()
  {
    return level_;
  }
  /** This is usually done by the command line option -MiroLogLevel. */
  inline
  void
  Log::level(int _level) throw()
  {
    level_ = _level;
  }
  inline
  unsigned int
  Log::mask() throw()
  {
    return mask_;
  }
  inline
  void
  Log::mask(unsigned int _mask) throw()
  {
    mask_ = _mask;
  }

  inline bool Log::compiledWithLog()
  {
#if defined (MIRO_NO_LOGGING)
    return false;
#else
    return true;
#endif
  }

  inline bool Log::compiledWithDebug()
  {
#if defined (MIRO_NO_DEBUG)
    return false;
#else
    return true;
#endif
  }

  inline
  bool
  Log::enabled(int _priority)
  {
    return
      (mask_ & _priority) != 0;
  }
  inline
  char const *
  Log::format()
  {
    return format_;
  }
  inline
  void
  Log::format(ACE_TCHAR const * _format)
  {
    format_ = _format;
  }
  inline
  ACE_Log_Priority
  Log::ll2LM(int _level)
  {
    MIRO_ASSERT(_level <= MAX_DBG_LEVEL);
    return aceLM_[_level];
  }
}
#endif // miro_Log_h
