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
#ifndef GNSSTK_GLOCNAVUT1TIMEOFFSET_HPP
#define GNSSTK_GLOCNAVUT1TIMEOFFSET_HPP

#include "StdNavTimeOffset.hpp"

namespace gnsstk
{
      /// @ingroup NavFactory
      //@{

      /** Defines the class that provides the ability to convert
       * GLONASS to/from UT1 (UTC Greenwich, to distinguish it from
       * UTC(SU)), using data extracted from GLONASS navigation
       * messages.
       * The algorithm is sufficiently different from StdNavTimeOffset
       * as to warrant its own implementation.  This is mainly due to
       * the terms of the polynomial being referenced to the time
       * difference in days rather than seconds like the rest of the
       * world. */
   class GLOCNavUT1TimeOffset : public TimeOffsetData
   {
   public:
         /// Initialize all data to 0.
      GLOCNavUT1TimeOffset();

         /** Print the contents of this object in a human-readable
          * format.  Uses parameters for GLONASS instead of generalized.
          * @param[in,out] s The stream to write the data to.
          * @param[in] dl The level of detail the output should contain. */
      void dump(std::ostream& s, DumpDetail dl) const override;

         /** Print the contents of this object in a terse format.
          * @param[in,out] s The stream to write the data to. */
      void dumpTerse(std::ostream& s) const;

         /** Get the offset, in seconds, to apply to times when
          * converting them from fromSys to toSys.
          * @param[in] fromSys The time system to convert from.
          * @param[in] toSys The time system to convert to.
          * @param[in] when The time being converted, in the same time
          *   system as fromSys.
          * @param[out] offset The offset in seconds where
          *   when(toSys)=when(fromSys)-offset.
          * @throw AssertionFailure if when's time system is not fromSys.
          * @return true if an offset is available, false if not. */
      bool getOffset(TimeSystem fromSys, TimeSystem toSys,
                     const CommonTime& when, double& offset)
         const override;

         /** The set of time system conversions this class is capable of making.
          * @return a set of supported time system conversion to/from pairs. */
      TimeCvtSet getConversions() const override;

         /** Checks the contents of this message against known
          * validity rules as defined in the appropriate ICD.
          * @return true if this message is valid according to ICD criteria.
          */
      bool validate() const override;

      CommonTime refTime; ///< Reference time for computation.
      unsigned NB;   ///< Day since the most recent leap year-aligned 4 years.
      double B0;     ///< Time bias in seconds.
      double B1;     ///< Time drift in s/s.
      double B2;     ///< Time drift rate in s/s**2.
      double UTCTAI; ///< Leap seconds - not really used.
   };

      //@}

}

#endif // GNSSTK_GLOCNAVUT1TIMEOFFSET_HPP
