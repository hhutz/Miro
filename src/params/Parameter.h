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
#ifndef Miro_CFG_Parameter_h
#define Miro_CFG_Parameter_h

#include <qstring.h>

#include <vector>

namespace Miro
{
  namespace CFG {
    struct Parameter {
      QString type_;
      QString name_;
      QString default_;
      QString fullDefault_;
      QString measure_;
      QString description_;

      Parameter();
      Parameter(QString const& _type,
                QString const& _name,
                QString const& _default,
                QString const& _fullDefault,
                QString const& _measure,
                QString const& _description);
    };

    bool operator < (Parameter const & lhs, Parameter const& rhs);
    bool operator == (Parameter const & lhs, Parameter const& rhs);

    typedef std::vector<Parameter> ParameterVector;
  }
}

#endif // Miro_CFG_Parameter_h
