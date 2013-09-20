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
namespace Miro
{

  inline
  void
  Angle::normalize(double& _alpha)
  {
    //    if (_alpha < 10. && _alpha > -10.) {
      while (_alpha > M_PI)
        _alpha -= 2. * M_PI;
      while (_alpha <= -M_PI)
        _alpha += 2. * M_PI;
      return;
      //    }

      //    _alpha = asin(sin(_alpha));
  }

  inline
  void
  Angle::normalize1(double& _alpha)
  {
    if (_alpha > M_PI)
      _alpha -= 2. * M_PI;
    else if (_alpha <= -M_PI)
      _alpha += 2. * M_PI;
  }

  inline
  void
  Angle::normalize()
  {
    if (angle_ < 10. && angle_ > -10) {
      while (angle_ > M_PI)
        angle_ -= 2. * M_PI;
      while (angle_ <= -M_PI)
        angle_ += 2. * M_PI;

      return;
    }

    angle_ = asin(sin(angle_));
  }

  inline
  void
  Angle::normalize1()
  {
    if (angle_ > M_PI)
      angle_ -= 2. * M_PI;
    else if (angle_ <= -M_PI)
      angle_ += 2. * M_PI;
  }

  // helper functions
  inline
  double deg2Rad(double _deg)
  {
    return _deg * M_PI / 180.;
  }
  inline
  double rad2Deg(double _rad)
  {
    return _rad * 180. * M_1_PI;
  }

  // Calculation operators
  inline
  Angle
  operator + (Angle alpha, Angle beta)
  {
    return Angle(alpha.angle_ + beta.angle_);
  }
  inline
  Angle
  operator + (Angle alpha, double beta)
  {
    return Angle(alpha.angle_ + beta);
  }
  inline
  Angle
  operator + (double alpha, Angle beta)
  {
    return Angle(alpha + beta.angle_);
  }
  inline
  Angle
  operator - (Angle alpha, Angle beta)
  {
    return Angle(alpha.angle_ - beta.angle_);
  }
  inline
  Angle
  operator - (Angle alpha, double beta)
  {
    return Angle(alpha.angle_ - beta);
  }
  inline
  Angle
  operator - (double alpha, Angle beta)
  {
    return Angle(alpha - beta.angle_);
  }
  inline
  Angle
  operator * (Angle alpha, double val)
  {
    return Angle(alpha.angle_ * val);
  }
  inline
  Angle
  operator / (Angle alpha, double val)
  {
    return Angle(alpha.angle_ / val);
  }

  // Logical operators
  inline
  bool
  operator == (Angle alpha, Angle beta)
  {
    return alpha.angle_ == beta.angle_;
  }
  inline
  bool
  operator != (Angle alpha, Angle beta)
  {
    return alpha.angle_ != beta.angle_;
  }
  inline
  bool
  operator < (Angle alpha, Angle beta)
  {
    return alpha.angle_ < beta.angle_;
  }
}
