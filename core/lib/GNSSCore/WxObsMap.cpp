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

/**
 * @file WxObsMap.cpp
 * A class encapsulating wx data.
 */

#include "WxObsMap.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gnsstk;

namespace gnsstk
{

   WxObservation WxObsData::getMostRecent( const CommonTime& t ) const
      noexcept
   {
      if(obs.size() == 0)
         return WxObservation();

      WxObsMap::const_iterator i = obs.upper_bound(t);
      if (i== obs.end())
         i--;
      if (i != obs.begin())
         i--;
      return  i->second;
   };

   void WxObsData::insertObservation( const WxObservation& wx )
      noexcept
   {
      obs[wx.t] = wx;
      if (wx.t > lastTime)  lastTime=wx.t;
      if (wx.t < firstTime) firstTime=wx.t;
   }

   bool WxObservation::isAllValid() const
      noexcept
   {
      return temperatureSource != noWx
         && pressureSource != noWx
         && humiditySource != noWx;
   };

   void WxObsData::flush(const CommonTime& t) noexcept
   {
      // remove data from the WxObsMap
      // map is sorted by time, stop removing data at
      // first point after t
      WxObsMap::iterator i = obs.begin();
      while (i != obs.end())
      {
         if (i->first < t)
         {
            obs.erase(i);
            i = obs.begin();
            firstTime = i->second.t;
         }
         else
            break;
      }
   }

   WxObservation WxObsData::getWxObservation(const CommonTime& t,
                                             unsigned iv,
                                             bool interpolate) const
   {
      if (obs.empty())
      {
         ObjectNotFound e("No WxObservation available near time " +
                          printTime(t,"%02H:%02M:%02S on day %03j of %4Y"));
         GNSSTK_THROW(e);
      }

      // get the first object after time t;
      WxObsMap::const_iterator after = obs.upper_bound(t);

      if (after == obs.begin())
      {
         const WxObservation& wxa = after->second;
         if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
         {
            // only after point fits
            return wxa;
         }
         else
         {
            ObjectNotFound e("No WxObservation available near time " +
                             printTime(t,"%02H:%02M:%02S on day %03j of %4Y"));
            GNSSTK_THROW(e);
         }
      }


      // get the first object at or before time t;
      WxObsMap::const_iterator before = after;
      before--;

      if (after == obs.end())
      {
         const WxObservation& wxb = before->second;
         if((wxb.t >= (t - iv)) && (wxb.t <= (t + iv)))
         {
            // only before point fits
            return wxb;
         }
         else
         {
            ObjectNotFound e("No WeatherData available near time " +
                             printTime(t,"%02H:%02M:%02S on day %03j of %4Y"));
            GNSSTK_THROW(e);
         }
      }
      else
      {
         const WxObservation& wxa = after->second;
         const WxObservation& wxb = before->second;

         if (interpolate)
         {
            if((wxb.t >= (t - iv)) && (wxb.t <= (t + iv)))
            {
               if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
               {
                  // both points fit, linearly interpolate and create
                  // a WeatherData object with those values
                  double dtw = wxa.t - wxb.t;
                  double dt = t - wxb.t;

                  double slope = (wxa.pressure - wxb.pressure) / dtw;
                  double pressure = slope * dt + wxb.pressure;

                  slope = (wxa.humidity - wxb.humidity) / dtw;
                  double humidity = slope * dt + wxb.humidity;

                  slope = (wxa.temperature - wxb.temperature) / dtw;
                  double temp = slope * dt + wxb.temperature;

                  WxObservation wx(t, temp, pressure, humidity);
                  return wx;
               }
               else
               {
                  // only before point fits
                  return wxb;
               }
            }
            else if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
            {
               // only after point fits
               return wxa;
            }
            else
            {
               ObjectNotFound e("No WeatherData available near time " +
                                printTime(t,"%02H:%02M:%02S on day %03j of %4Y"));
               GNSSTK_THROW(e);
            }
         }
         else
         {
            if((wxb.t >= (t - iv)) && (wxb.t <= (t + iv)))
            {
               if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
               {
                  // both points fit, return closer point, or
                  // before point if at same distance
                  double diffa = wxa.t - t;
                  double diffb = t - wxb.t;
                  return(diffa < diffb ? wxa : wxb);
               }
               else
               {
                  // only before point fits
                  return wxb;
               }
            }
            else if ((wxa.t >= (t - iv)) && (wxa.t <= (t + iv)))
            {
               // only after point fits
               return wxa;
            }
            else
            {
               ObjectNotFound e("No WeatherData available near time " +
                                printTime(t,"%02H:%02M:%02S on day %03j of %4Y"));
               GNSSTK_THROW(e);
            }
         }
      }
   }

   // These are just to facilitate debugging.
   std::ostream& operator<<(std::ostream& s, const gnsstk::WxObservation& obs)
      noexcept
   {
      // Note that this does not flag where the wx data came from
      s << obs.t << ", t=" << obs.temperature
        << ", p=" << obs.pressure
        << ", rh=" << obs.humidity;
      return s;
   }


}  // namespace
