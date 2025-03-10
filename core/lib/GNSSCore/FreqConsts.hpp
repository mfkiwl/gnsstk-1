/** @warning This code is automatically generated.
 *
 *  DO NOT EDIT THIS CODE BY HAND.
 *
 *  Refer to the documenation in the toolkit_docs gitlab project.
 */

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

#ifndef GNSSTK_FREQCONSTS_HPP
#define GNSSTK_FREQCONSTS_HPP

#include "GNSSconstants.hpp"
#include "SatID.hpp"

namespace gnsstk
{
      /// GPS L1 carrier frequency in Hz
   const double FREQ_GPS_L1 = 1575.42e6;
      /// GPS L1 carrier wavelength in meters
   const double WAVELENGTH_GPS_L1 = C_MPS / FREQ_GPS_L1;
      /// GPS L2 carrier frequency in Hz
   const double FREQ_GPS_L2 = 1227.6e6;
      /// GPS L2 carrier wavelength in meters
   const double WAVELENGTH_GPS_L2 = C_MPS / FREQ_GPS_L2;
      /// GPS L5 carrier frequency in Hz
   const double FREQ_GPS_L5 = 1176.45e6;
      /// GPS L5 carrier wavelength in meters
   const double WAVELENGTH_GPS_L5 = C_MPS / FREQ_GPS_L5;
      /// GLONASS G1 carrier frequency in Hz
   const double FREQ_GLONASS_G1 = 1602e6;
      /// GLONASS G1 carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G1 = C_MPS / FREQ_GLONASS_G1;
      /// GLONASS G1a carrier frequency in Hz
   const double FREQ_GLONASS_G1a = 1600.995e6;
      /// GLONASS G1a carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G1a = C_MPS / FREQ_GLONASS_G1a;
      /// GLONASS G2a carrier frequency in Hz
   const double FREQ_GLONASS_G2a = 1248.06e6;
      /// GLONASS G2a carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G2a = C_MPS / FREQ_GLONASS_G2a;
      /// GLONASS G2 carrier frequency in Hz
   const double FREQ_GLONASS_G2 = 1246e6;
      /// GLONASS G2 carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G2 = C_MPS / FREQ_GLONASS_G2;
      /// GLONASS G2c carrier frequency in Hz
   const double FREQ_GLONASS_G2c = 1242e6;
      /// GLONASS G2c carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G2c = C_MPS / FREQ_GLONASS_G2c;
      /// GLONASS G5 carrier frequency in Hz
   const double FREQ_GLONASS_G5 = 1207.14e6;
      /// GLONASS G5 carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G5 = C_MPS / FREQ_GLONASS_G5;
      /// GLONASS G3 carrier frequency in Hz
   const double FREQ_GLONASS_G3 = 1202.025e6;
      /// GLONASS G3 carrier wavelength in meters
   const double WAVELENGTH_GLONASS_G3 = C_MPS / FREQ_GLONASS_G3;
      /// Galileo E1 carrier frequency in Hz
   const double FREQ_GALILEO_E1 = 1575.42e6;
      /// Galileo E1 carrier wavelength in meters
   const double WAVELENGTH_GALILEO_E1 = C_MPS / FREQ_GALILEO_E1;
      /// Galileo E5b carrier frequency in Hz
   const double FREQ_GALILEO_E5b = 1207.14e6;
      /// Galileo E5b carrier wavelength in meters
   const double WAVELENGTH_GALILEO_E5b = C_MPS / FREQ_GALILEO_E5b;
      /// Galileo E5 carrier frequency in Hz
   const double FREQ_GALILEO_E5 = 1191.795e6;
      /// Galileo E5 carrier wavelength in meters
   const double WAVELENGTH_GALILEO_E5 = C_MPS / FREQ_GALILEO_E5;
      /// Galileo E5a carrier frequency in Hz
   const double FREQ_GALILEO_E5a = 1176.45e6;
      /// Galileo E5a carrier wavelength in meters
   const double WAVELENGTH_GALILEO_E5a = C_MPS / FREQ_GALILEO_E5a;
      /// Galileo E6 carrier frequency in Hz
   const double FREQ_GALILEO_E6 = 1278.75e6;
      /// Galileo E6 carrier wavelength in meters
   const double WAVELENGTH_GALILEO_E6 = C_MPS / FREQ_GALILEO_E6;
      /// SBAS L1 carrier frequency in Hz
   const double FREQ_SBAS_L1 = 1575.42e6;
      /// SBAS L1 carrier wavelength in meters
   const double WAVELENGTH_SBAS_L1 = C_MPS / FREQ_SBAS_L1;
      /// SBAS L5 carrier frequency in Hz
   const double FREQ_SBAS_L5 = 1176.45e6;
      /// SBAS L5 carrier wavelength in meters
   const double WAVELENGTH_SBAS_L5 = C_MPS / FREQ_SBAS_L5;
      /// QZSS L1 carrier frequency in Hz
   const double FREQ_QZSS_L1 = 1575.42e6;
      /// QZSS L1 carrier wavelength in meters
   const double WAVELENGTH_QZSS_L1 = C_MPS / FREQ_QZSS_L1;
      /// QZSS L2 carrier frequency in Hz
   const double FREQ_QZSS_L2 = 1227.6e6;
      /// QZSS L2 carrier wavelength in meters
   const double WAVELENGTH_QZSS_L2 = C_MPS / FREQ_QZSS_L2;
      /// QZSS L5 carrier frequency in Hz
   const double FREQ_QZSS_L5 = 1176.45e6;
      /// QZSS L5 carrier wavelength in meters
   const double WAVELENGTH_QZSS_L5 = C_MPS / FREQ_QZSS_L5;
      /// QZSS L6 carrier frequency in Hz
   const double FREQ_QZSS_L6 = 1278.75e6;
      /// QZSS L6 carrier wavelength in meters
   const double WAVELENGTH_QZSS_L6 = C_MPS / FREQ_QZSS_L6;
      /// BeiDou B1U carrier frequency in Hz
   const double FREQ_BEIDOU_B1U = 1589.742e6;
      /// BeiDou B1U carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_B1U = C_MPS / FREQ_BEIDOU_B1U;
      /// BeiDou L1 carrier frequency in Hz
   const double FREQ_BEIDOU_L1 = 1575.42e6;
      /// BeiDou L1 carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_L1 = C_MPS / FREQ_BEIDOU_L1;
      /// BeiDou B1 carrier frequency in Hz
   const double FREQ_BEIDOU_B1 = 1561.098e6;
      /// BeiDou B1 carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_B1 = C_MPS / FREQ_BEIDOU_B1;
      /// BeiDou B2b carrier frequency in Hz
   const double FREQ_BEIDOU_B2b = 1207.14e6;
      /// BeiDou B2b carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_B2b = C_MPS / FREQ_BEIDOU_B2b;
      /// BeiDou B3 carrier frequency in Hz
   const double FREQ_BEIDOU_B3 = 1268.52e6;
      /// BeiDou B3 carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_B3 = C_MPS / FREQ_BEIDOU_B3;
      /// BeiDou B2 carrier frequency in Hz
   const double FREQ_BEIDOU_B2 = 1191.795e6;
      /// BeiDou B2 carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_B2 = C_MPS / FREQ_BEIDOU_B2;
      /// BeiDou B2a carrier frequency in Hz
   const double FREQ_BEIDOU_B2a = 1176.45e6;
      /// BeiDou B2a carrier wavelength in meters
   const double WAVELENGTH_BEIDOU_B2a = C_MPS / FREQ_BEIDOU_B2a;
      /// NavIC S carrier frequency in Hz
   const double FREQ_NAVIC_S = 2492.028e6;
      /// NavIC S carrier wavelength in meters
   const double WAVELENGTH_NAVIC_S = C_MPS / FREQ_NAVIC_S;
      /// NavIC L5 carrier frequency in Hz
   const double FREQ_NAVIC_L5 = 1176.45e6;
      /// NavIC L5 carrier wavelength in meters
   const double WAVELENGTH_NAVIC_L5 = C_MPS / FREQ_NAVIC_L5;
      /// GLONASS G1 carrier frequency step size in Hz
   const double FREQ_STEP_GLONASS_G1 = 562.5e3;
      /// GLONASS G2 carrier frequency step size in Hz
   const double FREQ_STEP_GLONASS_G2 = 437.5e3;

      /** Compute wavelength for the given satellite system (sat.id is
       * ignored) at the given RINEX frequency band.
       * @param[in] sys The satellite system whose wavelength is desired.
       * @param[in] rinexBandNum The number used by RINEX 3 to
       *   represent the desired band.
       * @param[in] gloChan The GLONASS frequency channel number, when
       *   getting the wavelength for a GLONASS FDMA channel.
       *   Ignored for GLONASS CDMA and all other systems.
       * @return 0 if rinexBandNum is not a valid band for the given
       *   system, otherwise the wavelength in meters.
       */
   inline double getWavelength(SatelliteSystem sys,
                               int rinexBandNum, int gloChan=0)
      noexcept
   {
      switch(sys)
      {
         case SatelliteSystem::GPS:
            switch (rinexBandNum)
            {
               case 1: return WAVELENGTH_GPS_L1;
               case 2: return WAVELENGTH_GPS_L2;
               case 5: return WAVELENGTH_GPS_L5;
            }
            break;
         case SatelliteSystem::Glonass:
            switch (rinexBandNum)
            {
               case 1:
                  return (C_MPS/(FREQ_GLONASS_G1 + gloChan*FREQ_STEP_GLONASS_G1));
               case 4: return WAVELENGTH_GLONASS_G1a;
               case 6: return WAVELENGTH_GLONASS_G2a;
               case 2:
                  return (C_MPS/(FREQ_GLONASS_G2 + gloChan*FREQ_STEP_GLONASS_G2));
               case 3: return WAVELENGTH_GLONASS_G3;
            }
            break;
         case SatelliteSystem::Galileo:
            switch (rinexBandNum)
            {
               case 1: return WAVELENGTH_GALILEO_E1;
               case 7: return WAVELENGTH_GALILEO_E5b;
               case 8: return WAVELENGTH_GALILEO_E5;
               case 5: return WAVELENGTH_GALILEO_E5a;
               case 6: return WAVELENGTH_GALILEO_E6;
            }
            break;
         case SatelliteSystem::Geosync:
            switch (rinexBandNum)
            {
               case 1: return WAVELENGTH_SBAS_L1;
               case 5: return WAVELENGTH_SBAS_L5;
            }
            break;
         case SatelliteSystem::QZSS:
            switch (rinexBandNum)
            {
               case 1: return WAVELENGTH_QZSS_L1;
               case 2: return WAVELENGTH_QZSS_L2;
               case 5: return WAVELENGTH_QZSS_L5;
               case 6: return WAVELENGTH_QZSS_L6;
            }
            break;
         case SatelliteSystem::BeiDou:
            switch (rinexBandNum)
            {
               case 1: return WAVELENGTH_BEIDOU_L1;
               case 2: return WAVELENGTH_BEIDOU_B1;
               case 7: return WAVELENGTH_BEIDOU_B2b;
               case 6: return WAVELENGTH_BEIDOU_B3;
               case 8: return WAVELENGTH_BEIDOU_B2;
               case 5: return WAVELENGTH_BEIDOU_B2a;
            }
            break;
         case SatelliteSystem::IRNSS:
            switch (rinexBandNum)
            {
               case 9: return WAVELENGTH_NAVIC_S;
               case 5: return WAVELENGTH_NAVIC_L5;
            }
            break;
         default:
            break;
      }
      return 0.;
   }

      /** Compute beta(a,b), the ratio of 2 frequencies fb/fa for the
       * given satellite system.
       * @return 0 if either of the input n's are not valid RINEX bands
       *   for the system. */
   inline double getBeta(SatelliteSystem sys, int na, int nb)
      noexcept
   {
      double wla = getWavelength(sys,na);
      double wlb = getWavelength(sys,nb);
      if (wla == 0.0 || wlb == 0.0)
         return 0.0;
      return wlb/wla;
   }

      /** Compute alpha (also called gamma) = (beta^2-1) =
       * ((fa/fb)^2-1) for 2 frequencies fa,fb for the given satellite
       * system (sat.id is ignored).
       * @return 0 if either of the input n's are not valid RINEX
       *   bands for the satellite system. */
   inline double getAlpha(SatelliteSystem sys, int na, int nb)
      noexcept
   {
      double beta(getBeta(sys,na,nb));
      if (beta == 0.0)
         return 0.0;
      return (beta*beta-1.0);
   }
} // namespace gnsstk

#endif // GNSSTK_FREQCONSTS_HPP
