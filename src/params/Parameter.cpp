// -*- c++ -*- ///////////////////////////////////////////////////////////////
//
// This file is part of Miro (The Middleware for Robots)
// Copyright (C) 1999-2005
// Department of Neuroinformatics, University of Ulm, Germany
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
// $Id$
//
#include "Parameter.h"

namespace Miro
{
  namespace CFG {
    Parameter::Parameter()
    {}

    Parameter::Parameter(QString const& _type,
                         QString const& _name,
                         QString const& _default,
                         QString const& _fullDefault,
                         QString const& _measure,
                         QString const& _description) :
        type_(_type),
        name_(_name),
        default_(_default),
        fullDefault_(_fullDefault),
        measure_(_measure),
        description_(_description)
    {}

    bool operator < (Parameter const& lhs, Parameter const& rhs)
    {
      return (!(QString::compare(lhs.name_, rhs.name_) >= 0 &&
                QString::compare(lhs.type_, rhs.type_) >= 0 &&
                QString::compare(lhs.default_, rhs.default_) >= 0 &&
                QString::compare(lhs.measure_, rhs.measure_) >= 0 &&
                QString::compare(lhs.description_, rhs.description_) >= 0));
    }

    bool operator == (Parameter const& lhs, Parameter const& rhs)
    {
      return (QString::compare(lhs.name_, rhs.name_) == 0 &&
              QString::compare(lhs.type_, rhs.type_) == 0 &&
              QString::compare(lhs.default_, rhs.default_) == 0 &&
              QString::compare(lhs.measure_, rhs.measure_) == 0 &&
              QString::compare(lhs.description_, rhs.description_) == 0);
    }
  }
}

