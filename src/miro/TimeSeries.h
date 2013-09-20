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
// $Id$
//
#ifndef miro_TimeSeries_h
#define miro_TimeSeries_h

#include "Log.h"
#include "miroCore_Export.h"

#include <ace/Time_Value.h>
#include <ace/OS_NS_sys_time.h>
#include <ace/OS_NS_time.h>

#include <iosfwd>

#ifndef MIRO_NO_PERFORMANCE_LOGGING

#define MIRO_TIME_SERIES(x, y) \
  Miro::TimeSeries<> x(y)
#define MIRO_TIME_SERIES_N(x, y, n) \
  Miro::TimeSeries<n> x(y)
#define  MIRO_TIME_PROBE_START(x) \
  do {x.start();} while(false)
#define MIRO_TIME_PROBE_DONE(x) \
  do {x.done();} while(false)

#else //!MIRO_NO_PERFORMANCE_LOGGING

#define MIRO_TIME_SERIES(x, y)
#define MIRO_TIME_SERIES_N(x, y, n)
#define MIRO_TIME_PROBE_START(x)
#define MIRO_TIME_PROBE_DONE(x)

#endif // !MIRO_PERFORMANCE_LOGGING

namespace Miro
{
  //! Struct holding timing statistics.
  struct TimeStats {
    ACE_Time_Value min;
    ACE_Time_Value max;
    ACE_Time_Value mean;
    ACE_Time_Value var;
  };

  //! Output operator for TimeStats.
  miroCore_Export std::ostream& operator<<(std::ostream& _ostr, TimeStats const& _rhs);

  //! Class for simple time series evaluation.
  /** It calculates a floating mean of the last N time probes.  The
   * time probes are based upon the gettimeofday() system call and
   * lack the precise accuracy one might prefer when doing a
   * statistical evaluation for a scientific paper. But they provide
   * enough precision for generic timing evaluation.
   */
  template < unsigned int N = 100UL >
  class miroCore_Export TimeSeries
  {
  public:
    static const unsigned int SIZE = N;

    //! The constructor takes a name argument.
    TimeSeries(std::string const& _name = "Miro") throw();

    //! Start the time probe.
    void start(ACE_Time_Value const& _stamp = ACE_OS::gettimeofday()) throw();
    //! Stop the time proble.
    void done(ACE_Time_Value const& _stamp = ACE_OS::gettimeofday()) throw();

    //! Query last value.
    ACE_Time_Value const& last() const throw();
    //! Evaluate statistics.
    void eval(TimeStats& _stats) const throw();
    //! Evaluate statistics.
    TimeStats eval() const throw();
    //! Return the size of the histogram
    unsigned int size() const throw();
    //! True if the histogram holds the maximum number of entries.
    bool full() const throw();
    //! Returns a reference to the name of the time series.
    std::string const& name() const throw();

  private:
    //! The name of the series.
    std::string const name_;

    //! The timing histogram.
    ACE_Time_Value stamp_[SIZE];
    //! Pointer to the beginning of the histogram.
    ACE_Time_Value * const begin_;
    //! Pointer to the end of the histogram.
    ACE_Time_Value * const end_;
    //! Pointer to the next entry.
    ACE_Time_Value * next_;
    //! Start time of the current time probe.
    ACE_Time_Value start_;

    //! Flag indicating the ring buffer is full.
    bool full_;
    //! Flag indicating a time probe was started.
    bool started_;

    //! Internal helper method for squaring a time value.
    static ACE_Time_Value tSquare(ACE_Time_Value const& _t) throw();
  };

  template<unsigned int N>
  inline
  TimeSeries<N>::TimeSeries(std::string const& _name) throw() :
      name_(_name),
      begin_(stamp_),
      end_(stamp_ + SIZE),
      next_(stamp_),
      full_(false),
      started_(false)
  {
  }

  template<unsigned int N>
  inline
  bool
  TimeSeries<N>::full() const throw()
  {
    return full_;
  }

  template<unsigned int N>
  inline
  unsigned int
  TimeSeries<N>::size() const  throw()
  {
    return (full_) ? SIZE : next_ - begin_;
  }

  template<unsigned int N>
  inline
  void
  TimeSeries<N>::start(ACE_Time_Value const& _stamp) throw()
  {
    start_ = _stamp;
    started_ = true;
  }

  template<unsigned int N>
  inline
  void
  TimeSeries<N>::done(ACE_Time_Value const& _stamp) throw()
  {
    // prevent nonsense
    MIRO_ASSERT(started_);

    *next_++ = _stamp - start_;
    started_ = false;

    if (next_ == end_) {
      next_ = begin_;
      full_ = true;
    }
  }

  template<unsigned int N>
  ACE_Time_Value const&
  TimeSeries<N>::last() const throw()
  {
    ACE_Time_Value * l = next_;
    if (l == begin_)
      l = end_;
    --l;
    return *l;
  }

  template<unsigned int N>
  void
  TimeSeries<N>::eval(TimeStats& _stats) const throw()
  {
    ACE_Time_Value * first = begin_;
    ACE_Time_Value * last = (full_) ? end_ : next_;

    if (last - first > 2) {
      ACE_Time_Value * pMin = begin_;
      ACE_Time_Value * pMax = begin_;

      _stats.mean = *begin_;
      _stats.var = tSquare(*begin_);
      for (++first; first != last; ++first) {
        _stats.mean += *first;
        _stats.var += tSquare(*first);
        if (*first > *pMax) {
          pMax = first;
        }
        else if (*first < *pMin) {
          pMin = first;
        }
      }
      _stats.min = *pMin;
      _stats.max = *pMax;
      _stats.mean *= 1. / (double)size();
      _stats.var -= tSquare(_stats.mean);
      _stats.var *= 1. / ((double)(size() - 1));
    }
    else {
      _stats.min = 0;
      _stats.max = 0;
      _stats.mean = 0;
      _stats.var = 0;
    }
  }

  template<unsigned int N>
  TimeStats
  TimeSeries<N>::eval() const throw()
  {
    TimeStats stats;
    this->eval(stats);
    return stats;
  }

  template<unsigned int N>
  inline
  ACE_Time_Value
  TimeSeries<N>::tSquare(ACE_Time_Value const& _t)  throw()
  {
    ACE_Time_Value v;
    double t = (double)_t.sec() + (double)_t.usec() / 1000000.;
    t *= t;
    v.set(t);
    return v;
  }

  template<unsigned int N>
  inline
  std::string const&
  TimeSeries<N>::name() const throw()
  {
    return name_;
  }
}

#endif // miro_TimeSeries_h
