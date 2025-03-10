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
#ifndef GNSSTK_DUMPDETAIL_HPP
#define GNSSTK_DUMPDETAIL_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gnsstk
{
      /// @ingroup NavFactory
      //@{

      /// Specify level of detail for dump output.
   enum class DumpDetail
   {
      Unknown, ///< Uninitialized or unknown value.
      OneLine, ///< Limit output to minimal information on a single line.
      Brief,   ///< Limit output to <= 5 lines of minimal information.
      Terse,   ///< Aptly named, multiple lines of output with no labels.
      Full,    ///< Include all detailed information.
      Last     ///< Used to create an iterator.
   };

      /** Define an iterator so C++11 can do things like
       * for (DumpDetail i : DumpDetailIterator()) */
   typedef EnumIterator<DumpDetail, DumpDetail::Unknown, DumpDetail::Last> DumpDetailIterator;

   namespace StringUtils
   {
         /// Convert a DumpDetail to a whitespace-free string name.
      std::string asString(DumpDetail e) noexcept;
         /// Convert a string name to an DumpDetail
      DumpDetail asDumpDetail(const std::string& s) noexcept;
   }

      //@}

}

#endif // GNSSTK_DUMPDETAIL_HPP
