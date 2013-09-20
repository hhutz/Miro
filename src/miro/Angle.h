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
#ifndef miroAngle_h
#define miroAngle_h

#include "MiroConfig.h"

#include <cmath>
#include <iosfwd>

#include "miroBase_Export.h"

namespace Miro
{
  // forward declarations
  class Angle;

  // global helper functions declarations

  //! Converting a radiant vaule into degrees.
  double rad2Deg(double _rad);
  //! Converting degrees to radiant.
  double deg2Rad(double _deg);

  // Calculation operators

  //! Add @ref Angle beta to @ref Angle alpha.
  /**
   * @return The resulting @ref Angle gamma, which is normalized to -PI < x <= PI.
   */
  Angle operator+(Angle alpha, Angle beta);
  //! Subtract @ref Angle beta from @ref Angle alpha.
  /**
   * @return The resulting @ref Angle gamma, which is normalized to -PI < x <= PI.
   */
  Angle operator-(Angle alpha, Angle beta);
  //! Multiply @ref Angle alpha by the scalar val.
  /**
   * @return The resulting @ref Angle gamma, which is normalized to -PI < x <= PI.
   */
  Angle operator*(Angle alpha, double val);
  //! Divide @ref Angle alpha by the scalar val.
  /**
   * @return The resulting @ref Angle gamma, which is normalized to -PI < x <= PI.
   */
  Angle operator/(Angle alpha, double val);

  // Logical operators

  //! Test whether @ref Angle alpha is equal to @ref Angle beta.
  bool operator==(Angle alpha, Angle beta);
  //! Test whether @ref Angle alpha is not equal to @ref Angle beta.
  bool operator!=(Angle alpha, Angle beta);

  // IO friend functions

  //! Ostream operator.
  /**
   * Print the @ref Angle alpha on the stream ostr.
   * The value is printed as a double representing the radiant form.
   *
   * @return A reference to the ostream.
   */
  std::ostream& operator<<(std::ostream& ostr, Angle alpha);
  //! Istream operator.
  /**
   * Read the @ref Angle alpha from the stream istr.
   * The value is expected to be a double representing the radiant form.
   *
   * @return A reference to the istream.
   */
  std::istream& operator>>(std::istream& istr,  Angle& alpha);

  //! Class to simplify mathematics with angles.
  /**
   * An Angle is asserted to be always within the range of
   * -PI < Angle <= PI.
   * The internal representation of the data type is a double,
   * storing the angle in the radiant representation.
   * Conversion to and from degree is provided.
   *
   * @author Hans Utz
   */
  class Angle
  {
  public:
    // Constructors

    //! Default constructor.
    /**
     * Initializes the angle to 0.
     */
    Angle() :
        angle_(0.) {}
    //! Initializing constructor: radiant.
    /**
     * Initializes the angle to the in radiant specified value.
     */
    explicit Angle(double alpha)  :
        angle_(alpha) {
      normalize();
    }
    //! Copy constructor.
    /**
     * Initializes the angle by the given Angle alpha.
     */
    Angle(const Angle& alpha) :
        angle_(alpha) {}

    // Access methods

    //! Get the Angle in radiant.
    double rad() const {
      return angle_;
    }
    //! Get the Angle in degree.
    double deg() const {
      return (180. / M_PI * angle_);
    }
    //! Set the Angle in radiant.
    void setRad(double alpha) {
      angle_ = alpha;
      normalize();
    }
    //! Set the Angle in degree.
    void setDeg(double alpha) {
      angle_ = (M_PI / 180.  * alpha);
      normalize();
    }

    // Type conversion

    //! Conversion from double: radiant.
    /**
     * Converts an Angle to a double of the in radiant specified value.
     */
    operator double() const {
      return angle_;
    }

    // Assignment operators

    //! Self assignment.
    /**
     * You can assign an Angle to another Angle. Wow!
     *
     * @return Reference to self.
     */
    Angle& operator = (const Angle& alpha) {
      angle_ = alpha.angle_;
      return *this;
    }
    //! Assignment from double: radiant.
    /**
     * Converts a double to an angle of the in radiant specified value
     * and assigns it to the Angle.
     *
     * @return Reference to self.
     */
    Angle& operator = (double alpha) {
      angle_ = alpha;
      normalize();
      return *this;
    }

    // Unary operators

    //! The positive Angle.
    /**
     * Actually this method does nothing, but is presented for
     * mathematical completeness.
     *
     * @return Self.
     */
    Angle operator+() const {
      return Angle(angle_);
    }
    //! The negative Angle.
    /**
     * Vied in geometrical terms, the Angle is mirrored a the x axis.
     */
    Angle operator-() const {
      return Angle(-angle_);
    }

    // Combined assignment - calculation operators

    //! In place addition.
    /**
     * Add Angle alpha to this Angle.
     *
     * @return Reference to self.
     */
    Angle& operator+=(const Angle& alpha) {
      angle_ += alpha.angle_;
      normalize1();
      return *this;
    }
    //! In place subtraction.
    /**
     * Subtract Angle alpha from this Angle.
     *
     * @return Reference to self.
     */
    Angle& operator-=(const Angle& alpha) {
      angle_ -= alpha.angle_;
      normalize1();
      return *this;
    }
    //! In place addition.
    /**
     * Add the scalar val to this Angle.
     *
     * @return Reference to self.
     */
    Angle& operator+=(double val) {
      angle_ += val;
      normalize();
      return *this;
    }
    //! In place subtraction.
    /**
     * Subtract the scalar val to this Angle.
     *
     * @return Reference to self.
     */
    Angle& operator-=(double val) {
      angle_ -= val;
      normalize();
      return *this;
    }
    //! In place scalar multiplication.
    /**
     * Multiply the Angle by the scalar val.
     *
     * @return Reference to self.
     */
    Angle& operator*=(double val) {
      angle_ *= val;
      normalize();
      return *this;
    }
    //! In place scalar division.
    /**
     * Divide the Angle by the scalar val.
     *
     * @return Reference to self.
     */
    Angle& operator/=(double val) {
      angle_ /= val;
      normalize();
      return *this;
    }

    // Calculation operators
    friend Angle    operator + (Angle alpha, Angle beta);
    friend Angle    operator + (Angle alpha, double beta);
    friend Angle    operator + (double alpha, Angle beta);
    friend Angle    operator - (Angle alpha, Angle beta);
    friend Angle    operator - (Angle alpha, double beta);
    friend Angle    operator - (double alpha, Angle beta);
    friend Angle    operator * (Angle alpha, double val);
    friend Angle    operator / (Angle alpha, double val);

    // Logical operators
    friend bool     operator == (Angle alpha, Angle beta);
    friend bool     operator != (Angle alpha, Angle beta);
    friend bool     operator < (Angle alpha, Angle beta);

    // IO friend functions
    friend std::ostream& operator << (std::ostream& ostr, Angle alpha);
    friend std::istream& operator >> (std::istream& istr,  Angle& alpha);

    static void normalize(double& _alpha);
    static void normalize1(double& _alpha);

  protected:
    void normalize();
    void normalize1();

  private:
    double angle_;
  };
}

#include "Angle.i"
#endif




