
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl libC
// ------------------------------
#ifndef LIBC_EXPORT_H
#define LIBC_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (LIBC_HAS_DLL)
#  define LIBC_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && LIBC_HAS_DLL */

#if !defined (LIBC_HAS_DLL)
#  define LIBC_HAS_DLL 1
#endif /* ! LIBC_HAS_DLL */

#if defined (LIBC_HAS_DLL) && (LIBC_HAS_DLL == 1)
#  if defined (LIBC_BUILD_DLL)
#    define libC_Export ACE_Proper_Export_Flag
#    define LIBC_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define LIBC_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* LIBC_BUILD_DLL */
#    define libC_Export ACE_Proper_Import_Flag
#    define LIBC_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define LIBC_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* LIBC_BUILD_DLL */
#else /* LIBC_HAS_DLL == 1 */
#  define libC_Export
#  define LIBC_SINGLETON_DECLARATION(T)
#  define LIBC_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* LIBC_HAS_DLL == 1 */

// Set LIBC_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (LIBC_NTRACE)
#  if (ACE_NTRACE == 1)
#    define LIBC_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define LIBC_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !LIBC_NTRACE */

#if (LIBC_NTRACE == 1)
#  define LIBC_TRACE(X)
#else /* (LIBC_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define LIBC_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (LIBC_NTRACE == 1) */

#endif /* LIBC_EXPORT_H */

// End of auto generated file.
