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
#ifndef MIROJSON_EXPORT_H
#define MIROJSON_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (MIROJSON_HAS_DLL)
#  define MIROJSON_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && MIROJSON_HAS_DLL */

#if !defined (MIROJSON_HAS_DLL)
#  define MIROJSON_HAS_DLL 1
#endif /* ! MIROJSON_HAS_DLL */

#if defined (MIROJSON_HAS_DLL) && (MIROJSON_HAS_DLL == 1)
#  if defined (MIROJSON_BUILD_DLL)
#    define miroJson_Export ACE_Proper_Export_Flag
#    define MIROJSON_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define MIROJSON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* MIROJSON_BUILD_DLL */
#    define miroJson_Export ACE_Proper_Import_Flag
#    define MIROJSON_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define MIROJSON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* MIROJSON_BUILD_DLL */
#else /* MIROJSON_HAS_DLL == 1 */
#  define miroJson_Export
#  define MIROJSON_SINGLETON_DECLARATION(T)
#  define MIROJSON_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* MIROJSON_HAS_DLL == 1 */

// Set MIROJSON_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (MIROJSON_NTRACE)
#  if (ACE_NTRACE == 1)
#    define MIROJSON_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define MIROJSON_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !MIROJSON_NTRACE */

#if (MIROJSON_NTRACE == 1)
#  define MIROJSON_TRACE(X)
#else /* (MIROJSON_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define MIROJSON_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (MIROJSON_NTRACE == 1) */

#endif /* MIROJSON_EXPORT_H */

// End of auto generated file.
