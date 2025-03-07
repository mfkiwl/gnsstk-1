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

#ifndef GNSSTK_RINEX_SATID_HPP
#define GNSSTK_RINEX_SATID_HPP

#include <iostream>
#include <sstream>
#include <iomanip>

#include "gnsstk_export.h"
#include "Exception.hpp"
#include "SatID.hpp"

/**
 * @file RinexSatID.hpp
 * gnsstk::RinexSatID - Navigation system-independent representation of a
 *                     satellite, as defined by the RINEX specification.
 */

namespace gnsstk
{
      /// @todo determine if this really belongs with the RINEX files

      /// @ingroup FileHandling
      //@{

   class RinexSatID : public SatID
   {
   public:
         /// Empty constructor; creates an invalid object (Unknown, ID = -1).
      RinexSatID() = default;

         /// Explicit constructor, no defaults, RINEX systems only.
      RinexSatID(int p, SatelliteSystem s)
         noexcept;

         /** Constructor from a string.
          * @throw Exception
          */
      RinexSatID(const std::string& str)
      {
         try { fromString(str); }
         catch(Exception& e) { GNSSTK_RETHROW(e); }
      }

         /// Cast a SatID to a RinexSatID.
      RinexSatID(const SatID& sat)
         noexcept
            : SatID(sat)
      { validate(); }

         /// Set the fill character used in output and
         /// return the current fill character.
      char setfill(char c)
         noexcept
      { char csave = fillchar; fillchar = c; return csave; }


         /// Get the fill character used in output.
      char getfill() const
         noexcept
      { return fillchar; }

         // operator=, copy constructor and destructor built by compiler


         /** Return the single-character system descriptor.
          * @note return only RINEX types, for non-RINEX systems return '?'
          */
      char systemChar() const
         noexcept;

         /* Return the system name as a string.
          * @note Return only RINEX types or 'Unknown'.
          */
      std::string systemString() const
         noexcept;

         /** Return the system name as a string of length 3.
          * @note Return only RINEX types or 'Unknown'.
          */
      std::string systemString3() const
         noexcept;

         /** Set the RinexSatID from a string (1 character plus
          * 2-digit integer).
          * @note GPS is default system (no or unknown system char)
          * @throw Exception
          */
      void fromString(const std::string& s);

         /** Convert the RinexSatID to string (1 character plus
          * 2-digit integer). */
      std::string toString() const
         noexcept;

   private:
         /// If an unsupported system is used, set to unknown and PRN -1.
      void validate();

      GNSSTK_EXPORT
      static char fillchar;  ///< Fill character used during stream output.

   }; // class RinexSatID

      /// Stream output for RinexSatID.
   inline std::ostream& operator<<(std::ostream& s, const RinexSatID& sat)
   {
      s << sat.toString();
      return s;
   }

      //@}

} // namespace gnsstk

#endif
