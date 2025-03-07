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
 * @file GlobalTropModel.hpp
 * Base class for tropospheric models, plus implementations
 * of several published models
 */

#ifndef GLOBAL_TROP_MODEL_HPP
#define GLOBAL_TROP_MODEL_HPP

#include "CommonTime.hpp"
#include "TropModel.hpp"

namespace gnsstk
{
   /** @addtogroup GPSsolutions */
   //@{

      /** Tropospheric model based on the Global mapping functions (GMF)
       * and Global Pressure and Temperature (GPT) models.
       *
       * Ref (GMF). Boehm, J., A.E. Niell, P. Tregoning, H. Schuh (2006),
       *   "Global Mapping Functions (GMF): A new empirical mapping function
       *   based on numerical weather model data," Geoph. Res. Letters,
       *   Vol. 33, L07304, doi:10.1029/2005GL025545.
       * Ref (GPT). J. Böhm, R. Heinkelmann, H. Schuh (2007), "Short Note:
       *   A Global Model of Pressure and Temperature for Geodetic Applications,"
       *   Journal of Geodesy, doi:10.1007/s00190-007-0135-3.
       *
       * The user must specify a time (date) and a receiver position.
       * The model then computes the (GPT) pressure and temperature, as well as
       * (GMF) wet and hydrostatic (dry) zenith delays and, with the elevation
       * angle, mapping functions.
       * @note the GPT model is valid only for receiver height less than 44247 meters.
       *
       * <pre>
       *  depedency cheat sheet:
       *   User provides:    Model computes/stores:           Output of model:
       *     lat,lon,ht ---> coeffs  [in updateGTMCoeff()]
       *     time (doy) ---> dayfactor [ setTime(mjd) ]
       *     humidity%  ---> humid
       *
       *     [in getGPT():]  height,coeffs,dayfactor -> P,T
       *                     humid,T -----------------------> wet_zenith_delay()
       *                     P,T,lat -----------------------> dry_zenith_delay()
       *                     dayfactor,coeffs --------------> wet_mapping_function(elev)
       *                     dayfactor,coeffs --------------> dry_mapping_function(elev)
       *  So, change lat   => coeffs => P,T => wet/dry zen/map
       *             lon   => coeffs => P,T => wet/dry zen/map
       *             ht    => coeffs => P,T => wet/dry zen/map
       *             time  => dayfactor => P,T => wet/dry zen/map
       *             humid => wet zen
       *
       * </pre>
       *
       * @note Members of base TropModel::temp,press,humid,valid. 
       *   Members of GlobalTropModel::height,latitude,longitude,dayfactor,undul,
       *   validHeight, validLat, validLon, validDay
       *
       * A typical way to use this model follows:
       *
       * @code
       *   GlobalTropModel globalTM;
       *   globalTM.setReceiverLatitude(lat);
       *   globalTM.setReceiverLongitude(lon);
       *   globalTM.setReceiverHeight(height);
       *   globalTM.setDayOfYear(doy);
       *   globalTM.setHumidity(humidity);
       * @endcode
       *
       * Once all the basic model parameters are set, then we are able to
       * compute the tropospheric correction as a function of elevation:
       *
       * @code
       *   trop = globalTM.correction(elevation);
       * @endcode
       *
       * @warning The Global mapping functions are defined for elevation
       *   angles down to 3 degrees, below that the correction is set to zero.
       */
   class GlobalTropModel : public TropModel
   {
   public:
         /// Default constructor
      GlobalTropModel();

         /** Constructor to create a Global trop model providing the height of
          * the receiver above mean sea level (as defined by ellipsoid
          * model), its latitude and the day of year.
          * @param ht Height of the receiver above mean sea level, in meters.
          * @param lat Latitude of receiver, in degrees.
          * @param lon Longitude of receiver, in degrees.
          * @param mjd MJD (double)
          */
      GlobalTropModel(const double& ht, const double& lat, const double& lon,
                      const double& mjd)
      {
         validCoeff = validHeight = validLat = validLon = validDay = valid =
            false;
         setReceiverHeight(ht);
         setReceiverLatitude(lat);
         setReceiverLongitude(lon);
         setTime(mjd);
      }

         /** Constructor to create a Global trop model providing the position
          * of the receiver and current time.
          * @param RX Receiver position.
          * @param time Time.
          */
      GlobalTropModel(const Position& RX, const CommonTime& time)
      {
         validCoeff = validHeight = validLat = validLon = validDay = valid = false;
         setReceiverHeight(RX.getAltitude());
         setReceiverLatitude(RX.getGeodeticLatitude());
         setReceiverLongitude(RX.getLongitude());
         setTime(time);
      }

         /// @copydoc TropModel::name()
      virtual std::string name()
      { return std::string("Global"); }

         /// @copydoc TropModel::correction(double elevation) const
      virtual double correction(double elevation) const;

         /** Compute and return the full tropospheric delay, in meters,
          * given the positions of receiver and satellite.
          *
          * This version is more useful within positioning algorithms, where
          * the receiver position may vary; it computes the elevation (and
          * other receiver location information as height and latitude) and
          * passes them to appropriate methods.
          *
          * You must set time using method setReceiverDOY() before calling
          * this method.
          *
          * @param RX Receiver position.
          * @param SV Satellite position.
          * @return The tropospheric delay (meters)
          * @throw InvalidTropModel
          */
      virtual double correction(const Position& RX, const Position& SV);

         /// @copydoc TropModel::correction(const Position&,const Position&,const CommonTime&)
      virtual double correction(const Position& RX,
                                const Position& SV,
                                const CommonTime& tt)
      {
         setTime(tt);
         return correction(RX,SV);
      }

         /** Compute and return the zenith delay for hydrostatic (dry)
          * component of the troposphere. Use the Saastamoinen value.
          * Ref. Davis etal 1985 and Leick, 3rd ed, pg 197.
          * @throw InvalidTropModel
          */
      virtual double dry_zenith_delay() const;

         /** Compute and return the zenith delay for wet component of
          * the troposphere. Ref. Leick, 3rd ed, pg 197.
          * @throw InvalidTropModel
          */
      virtual double wet_zenith_delay() const;

         /// @copydoc dry_mapping_function(double) const
      virtual double dry_mapping_function(double elevation) const;

         /// @copydoc wet_mapping_function(double) const
      virtual double wet_mapping_function(double elevation) const;

         /** Compute the pressure and temperature at height, and the undulation,
          * for the given position and time.
          * @param P output pressure
          * @param T output temperature
          * @param U output undulation
          * @throw InvalidTropModel if the height is larger than 44247
          *   meters, which is beyond the model.
          */
      void getGPT(double& P, double& T, double& U);

         /** @copydoc TropModel::setWeather(const double&,const double&,const double&)
          *
          * @note The Global model will only use the humidity parameter.
          */
      virtual void setWeather(const double& T, const double& P, const double& H)
      { setHumidity(H); }

         /** @copydoc TropModel::setWeather(const WxObservation& wx)
          *
          * @note The Global model will only use the humidity parameter.
          */
      virtual void setWeather(const WxObservation& wx)
      { setHumidity(wx.humidity); }

         /** GlobalTropModel does not accept weather input, except
          * humid; thus the setWeather() routines are dummies; this
          * sets the relative humidity.
          * @note Humidity enters only in the wet zenith delay, which
          *   is not part of GTM.
          * @param rh double relative humidity in percent (0 <= rh <= 100)
          * @throw InvalidParameter
          */
      void setHumidity(const double& rh)
      {
         if(rh < 0.0 || rh > 100.)
            GNSSTK_THROW(InvalidParameter("Invalid humidity (%)"));
         humid = rh;
      }

      /// @copydoc TropModel::setReceiverLatitude(const double&)
      virtual void setReceiverLatitude(const double& lat);

      /// @copydoc TropModel::setReceiverLongitude(const double&)
      virtual void setReceiverLongitude(const double& lon);

      /// @copydoc TropModel::setReceiverHeight(const double&)
      virtual void setReceiverHeight(const double& ht);

      /// @copydoc TropModel::setDayOfYear
      virtual void setDayOfYear(const int& doy);

      /** Convenient method to set all non-weather model parameters in one call
       * @param time CommonTime of interest
       * @param rxPos Receiver position object.
       */
      virtual void setParameters(const CommonTime& time, const Position& rxPos);

      /// Get the height limit for this model, in meters
      double getHeightLimit()
      { return HEIGHT_LIMIT; }

   private:
      /** Define the time of interest; this is required before calling
       * correction() or any of the zenith_delay routines.
       * @param mjd  MJD (double)
       */
      void setTime(const double& mjd);

      /** Define the time of interest; this is required before calling
       * correction() or any of the zenith_delay routines.
       * @param time  CommonTime of interest
       */
      void setTime(const CommonTime& time);

      // GMF
      static const double ADryMean[55];
      static const double BDryMean[55];
      static const double ADryAmp[55];
      static const double BDryAmp[55];
      static const double AWetMean[55];
      static const double BWetMean[55];
      static const double AWetAmp[55];
      static const double BWetAmp[55];
      // GPT
      static const double Ageoid[55];
      static const double Bgeoid[55];
      static const double APressMean[55];
      static const double BPressMean[55];
      static const double APressAmp[55];
      static const double BPressAmp[55];
      static const double ATempMean[55];
      static const double BTempMean[55];
      static const double ATempAmp[55];
      static const double BTempAmp[55];

      static const double Factorial[19];

      /// Model is limited in height, at this value, in m
      GNSSTK_EXPORT
      static const double HEIGHT_LIMIT;

      double height, latitude, longitude, dayfactor, undul;
      double P[10][10], aP[55], bP[55];
      bool validHeight, validLat, validLon, validDay, validCoeff;

      /// Update coefficients when latitude and/or longitude changes
      void updateGTMCoeff();

         /** Utility to test valid flags
          * @throw InvalidTropModel
          */
      void testValidity() const;

         /** Utility to set valid based on the other flags,
          * and update coefficients and press, temp as needed
          * @throw InvalidTropModel
          */
      void setValid()
      {
         try{
            valid = validHeight && validLat && validLon && validDay;
            if(valid && !validCoeff) {
               updateGTMCoeff();
               validCoeff = true;
               getGPT(press,temp,undul);
            }
         } catch(Exception& e) { GNSSTK_RETHROW(e); }
      }

   }; // end class GlobalTropModel

      //@}
}  // end namespace

#endif   // #define GLOBAL_TROP_MODEL_HPP
