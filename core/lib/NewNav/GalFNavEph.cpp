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
#include "GalFNavEph.hpp"
#include "GPSWeekSecond.hpp"
#include "TimeString.hpp"

using namespace std;

namespace gnsstk
{
   GalFNavEph ::
   GalFNavEph()
         : bgdE5aE1(0.0),
           sisaIndex(255),
           svid(0),
           iodnav1(0),
           iodnav2(0),
           iodnav3(0),
           iodnav4(0),
           hsE5a(GalHealthStatus::Unknown),
           dvsE5a(GalDataValid::Unknown),
           wn1(0), tow1(0), wn2(0), tow2(0), wn3(0), tow3(0), tow4(0)
   {
      signal.messageType = NavMessageType::Ephemeris;
      weekFmt = "%4L(%4l)";
      msgLenSec = 10.0;
      frame = ReferenceFrame::ITRF;
   }


   bool GalFNavEph ::
   validate() const
   {
      return true;
   }


   CommonTime GalFNavEph ::
   getUserTime() const
   {
      gnsstk::CommonTime unset;
      gnsstk::CommonTime rv = xmitTime;
         // If any xmit time is unset, make a best guess at what it
         // should be by adding 10 seconds (the transmit time of an
         // F/NAV page) to the return value.
         // This is mostly to handle formats like RINEX where we don't
         // have the transmit time of each of the pages.
      if (xmit2 != unset)
         rv = std::max(rv, xmit2);
      else
         rv += 10.0;
      if (xmit3 != unset)
         rv = std::max(rv, xmit3);
      else
         rv += 10.0;
      if (xmit4 != unset)
         rv = std::max(rv, xmit4);
      else
         rv += 10.0;
         // Galileo F/NAV nominal page transmit time is 10 seconds per
         // OS-SIS-ICD figure 14.
      return rv + 10.0;
   }


   void GalFNavEph ::
   fixFit()
   {
      beginFit = xmitTime;
      endFit = Toe + 14400; // four hours
   }


   void GalFNavEph ::
   dumpSVStatus(std::ostream& s) const
   {
         /** @todo We're currently printing the transmit time of the
          * subframes, which may be different from the TOW value
          * that's in the page.  A quick comparison between the
          * earlier implementation's output and the current
          * implementation's output show that the values being dumped
          * are the same.  Not sure if we're consistently mislabeling
          * or if the TOW really is the same as the transmit time
          * we're printing. */
      const ios::fmtflags oldFlags = s.flags();
      s.setf(ios::scientific, ios::floatfield);
      const std::string poFmt("TOW :  " + weekFmt +
                              "  %6.0g  %3a-%1w:%02H:%02M:%02S");
      s << "           PAGE OVERHEAD" << endl
        << endl
        << "              Week(10bt)     SOW    DOW:HH:MM:SS      IOD_NAV"
        << endl
        << printTime(xmitTime,"Page 1 "+poFmt)
        << setw(13) << iodnav1 << endl
        << printTime(xmit2,"Page 2 "+poFmt)
        << setw(13) << iodnav2 << endl
        << printTime(xmit3,"Page 3 "+poFmt)
        << setw(13) << iodnav3 << endl
        << printTime(xmit4,"Page 4 "+poFmt)
        << setw(13) << iodnav4 << endl
        << endl
        << "           SV STATUS" << endl
        << endl
        << "E5a_DVS             : " << setw(9) << static_cast<int>(dvsE5a)
        << " (" << gnsstk::StringUtils::asString(dvsE5a) << ")" << endl
        << "E5a_HS              : " << setw(9) << static_cast<int>(hsE5a)
        << " (" << gnsstk::StringUtils::asString(hsE5a) << ")" << endl
        << "SISA                : " << setw(9) << (unsigned)sisaIndex << endl
        << setprecision(6)
        << "Bgd(E1-E5a)         : " << setw(13) << bgdE5aE1 << " sec" << endl;
      s.flags(oldFlags);
   }
}
