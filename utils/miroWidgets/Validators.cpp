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
#include "Validators.h"

#include <cmath>

MyIntValidator::MyIntValidator(QObject * parent, const char *name) :
  Super(parent, name),
  accepted_(true)
{}

MyIntValidator::MyIntValidator(int bottom, int top,
			       QObject * parent, const char *name) :
  Super(bottom, top, parent, name),
  accepted_(true)
{}

QValidator::State 
MyIntValidator::validate( QString & input, int & pos) const
{
  QValidator::State s = Super::validate(input, pos);
  bool accept = input.isEmpty() || (s == Acceptable);
  if (s != Invalid &&
      accept != accepted_) {
    accepted_ = accept;
    emit const_cast<MyIntValidator *>(this)->acceptable(accept);
  }
  return s;
}

MyFloatValidator::MyFloatValidator(QObject * parent, const char *name) :

  Super(parent, name),
  accepted_(true)
{}

MyFloatValidator::MyFloatValidator(float bottom, float top, int decimals,
				   QObject * parent, const char *name) :
  Super(bottom, top, decimals, parent, name),
  accepted_(true)
{}

QValidator::State 
MyFloatValidator::validate( QString & input, int & pos) const
{
  QValidator::State s = Super::validate(input, pos);
  bool accept = input.isEmpty() || (s == Acceptable);
  if (s != Invalid &&
      accept != accepted_) {
    accepted_ = accept;
    emit const_cast<MyFloatValidator *>(this)->acceptable(accept);
  }

  return s;
}

MyDoubleValidator::MyDoubleValidator(QObject * parent, const char *name) :
  Super(parent, name),
  accepted_(true)
{}

MyDoubleValidator::MyDoubleValidator(double bottom, double top, int decimals,
				     QObject * parent, const char *name) :
  Super(bottom, top, decimals, parent, name),
  accepted_(true)
{}

QValidator::State 
MyDoubleValidator::validate( QString & input, int & pos) const
{
  QValidator::State s = Super::validate(input, pos);
  bool accept = input.isEmpty() || (s == Acceptable);
  if (s != Invalid &&
      accept != accepted_) {
    accepted_ = accept;
    emit const_cast<MyDoubleValidator *>(this)->acceptable(accept);
  }

  return s;
}

MyBoolValidator::MyBoolValidator(QObject * parent, const char *name) :
  Super(parent, name),
  accepted_(true)
{}

QValidator::State 
MyBoolValidator::validate( QString & input, int &) const
{
  QValidator::State s = Invalid;

  if (input == "true" ||
      input == "false" ||
      input.length() == 0)
    s = Acceptable;
  else if (input.length() <= 5)
    s = Intermediate;
 
  bool accept = (s == Acceptable);
  if (s != Invalid &&
      accept != accepted_) {
    accepted_ = accept;
    emit const_cast<MyBoolValidator *>(this)->acceptable(accept);
  }

  return s;
}

