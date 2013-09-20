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
#ifndef Validators_h
#define Validators_h

#include <qvalidator.h>

#include "miroWidgets_Export.h"

//! Validator for intager values.
class miroWidgets_Export MyIntValidator : public QIntValidator
{
  Q_OBJECT
  //! Base class type.
  typedef QIntValidator Super;

public:
  //! Inherited method.
  MyIntValidator(QObject * parent, const char *name = 0);
  //! Inherited method.
  MyIntValidator(int bottom, int top,
		 QObject * parent, const char *name = 0);
  //! Inherited method.
  QValidator::State validate(QString &, int &) const;

signals:
  //! Signals, if the input is valid.
  void acceptable(bool);

protected:
  //! Remember, last state.
  mutable bool accepted_;
};

//! Validator for double values.
class MyFloatValidator : public QDoubleValidator
{
  Q_OBJECT
  //! Base class type.
  typedef QDoubleValidator Super;

public:
  //! Inherited method.
  MyFloatValidator(QObject * parent, const char *name = 0);
  //! Inherited method.
  MyFloatValidator(float bottom, float top, int decimals,
		    QObject * parent, const char *name = 0);
  //! Inherited method.
  QValidator::State validate(QString &, int &) const;

signals:
  //! Signals, if the input is valid.
  void acceptable(bool);

protected:
  //! Remember, last state.
  mutable bool accepted_;
};

//! Validator for double values.
class MyDoubleValidator : public QDoubleValidator
{
  Q_OBJECT
  //! Base class type.
  typedef QDoubleValidator Super;

public:
  //! Inherited method.
  MyDoubleValidator(QObject * parent, const char *name = 0);
  //! Inherited method.
  MyDoubleValidator(double bottom, double top, int decimals,
		    QObject * parent, const char *name = 0);
  //! Inherited method.
  QValidator::State validate(QString &, int &) const;

signals:
  //! Signals, if the input is valid.
  void acceptable(bool);

protected:
  //! Remember, last state.
  mutable bool accepted_;
};

//! Validator for boolean values.
class MyBoolValidator : public QValidator
{
  Q_OBJECT
  //! Base class type.
  typedef QValidator Super;

public:
  //! Inherited method.
  MyBoolValidator(QObject * parent, const char *name = 0);
  //! Inherited method.
  QValidator::State validate(QString &, int &) const;

signals:
  //! Signals, if the input is valid.
  void acceptable(bool);

protected:
  //! Remember, last state.
  mutable bool accepted_;
};

#endif // Validators_h

