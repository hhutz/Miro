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
#ifndef miro_JsonCpp_h
#define miro_JsonCpp_h

// We do not use json/json.h because two libraries (json-c and jsoncpp)
// both use the same header name. Hopefully one of these projects will 
// switch to a more specific naming convention, but until then, we 
// avoid conflict by using our own header that does the same thing
// as jsoncpp's json/json.h
#include <json/autolink.h>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include <json/features.h>

#endif // miro_JsonCpp_h
