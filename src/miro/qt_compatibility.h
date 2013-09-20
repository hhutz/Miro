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
#ifndef miro_qt_compatibility_h
#define miro_qt_compatibility_h

// temporary compatibility file for porting to Qt4
// the Qt3Support library from trolltech is heavyweight
// and has some problems...

#if QT_VERSION >= 0x040000

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const QString& str)
{
  return os << qPrintable(str);
}

// these #defs are gross, but hopefully temporary...
#define IO_ReadOnly QIODevice::ReadOnly

#else // QT_VERSION >= 0x040000

#define qPrintable( qstring ) qstring.latin1()



#endif // QT_VERSION >= 0x040000

#endif // miro_qt_compatibility_h
