
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl libB
// ------------------------------
#ifndef LIBB_EXPORT_H
#define LIBB_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (LIBB_HAS_DLL)
#  define LIBB_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && LIBB_HAS_DLL */

#if !defined (LIBB_HAS_DLL)
#  define LIBB_HAS_DLL 1
#endif /* ! LIBB_HAS_DLL */

#if defined (LIBB_HAS_DLL) && (LIBB_HAS_DLL == 1)
#  if defined (LIBB_BUILD_DLL)
#    define libB_Export ACE_Proper_Export_Flag
#    define LIBB_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define LIBB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* LIBB_BUILD_DLL */
#    define libB_Export ACE_Proper_Import_Flag
#    define LIBB_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define LIBB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* LIBB_BUILD_DLL */
#else /* LIBB_HAS_DLL == 1 */
#  define libB_Export
#  define LIBB_SINGLETON_DECLARATION(T)
#  define LIBB_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* LIBB_HAS_DLL == 1 */

// Set LIBB_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (LIBB_NTRACE)
#  if (ACE_NTRACE == 1)
#    define LIBB_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define LIBB_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !LIBB_NTRACE */

#if (LIBB_NTRACE == 1)
#  define LIBB_TRACE(X)
#else /* (LIBB_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define LIBB_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (LIBB_NTRACE == 1) */

#endif /* LIBB_EXPORT_H */

// End of auto generated file.
