//==============================================================================
//
//  This file is part of GNSSTk, the ARL:UT GNSS Toolkit.
//
//  The GNSSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GNSSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GNSSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2022, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef GNSSTK_POWERSUM_HPP
#define GNSSTK_POWERSUM_HPP

#include <iostream>
#include <list>

namespace gnsstk
{
      /// @ingroup MathGroup
      //@{

      /** This class computes the power sums of a list of numbers and
       * computes various statistical values based upon these
       * sums. This is a generalization of the Stats class that
       * supports the higher-order moments. See
       * http://mathworld.wolfram.com/PowerSum.html for a discussion
       * of this approach.
       */
   class PowerSum
   {
   public:
      PowerSum() {clear();}

      const static int order = 5;

      /// Reset all sums
      void clear() noexcept;

      /// Add a single value to the sums
      void add(double x) noexcept;

      /// Remove a single value from the sums. Note that the higher order sums
      /// can get quite large in magnitude. When removing a value that is
      /// far from the average, it is possible for numerical error to creep
      /// into the sums. One way around this is to simply recompute the sums
      /// from scratch when this happens.
      void subtract(double x) noexcept;

      typedef std::list<double>::const_iterator dlc_iterator;

      /// Adds all value in the list to the sums.
      void add(dlc_iterator b, dlc_iterator e) noexcept;

      /// Removes all values in the list from the sums. See the warning with the
      /// subtract(double) method.
      void subtract(dlc_iterator b, dlc_iterator e) noexcept;

      /// Computes the ith order central moment
      double moment(int i) const noexcept;

      /// Reuturn the number of points in the current sum
      long size() const noexcept {return n;}

      /// Computes the indicated value
      double average() const noexcept;
      double variance() const noexcept;
      double skew() const noexcept;
      double kurtosis() const noexcept;

      void dump(std::ostream& str) const noexcept;

   private:
      double s[order+1];
      long n;
/*
  These are used to determine kurtosis values for specific confidence based upon
  the sample size.
  const double pnt[]={
  5,    7,    8,    9,   10,   12,   15,   20,   25,
  30,   40,   50,   75,  100,  200,  500, 1000, 1e5, 1.e30};
  // 5% critical values
  const double cv5[] = {
  2.90, 3.55, 3.70, 3.86, 3.95, 4.05, 4.13, 4.17, 4.16,
  4.11, 4.06, 3.99, 3.87, 3.77, 3.57, 3.37, 3.26, 3.10, 3.00};
  // 1% critical values
  const double cv1[]= {
  3.10, 4.23, 4.53, 4.82, 5.00, 5.20, 5.30, 5.36, 5.30,
  5.21, 5.04, 4.88, 4.59, 4.39, 3.98, 3.60, 3.41, 3.20, 3.00};
*/
   };

      //@}

}
#endif
