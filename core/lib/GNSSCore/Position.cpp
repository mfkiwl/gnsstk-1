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

#include "Position.hpp"
#include "WGS84Ellipsoid.hpp"
#include "GNSSconstants.hpp"    // for TWO_PI, etc
#include "GNSSconstants.hpp"             // for RAD_TO_DEG, etc
#include "MiscMath.hpp"             // for RSS, SQRT
#include "Angle.hpp"
#include "DebugTrace.hpp"

namespace gnsstk
{

   using namespace std;
   using namespace StringUtils;

   // ----------- Part  1: coordinate systems --------------------------------
      // Labels for coordinate systems supported by Position
   static const char *SystemNames[] = {
      "Unknown",
      "Geodetic",
      "Geocentric",
      "Cartesian",
      "Spherical"};

      // return string giving name of coordinate system
   string Position::getSystemName()
      noexcept
   { return SystemNames[system]; }

   // ----------- Part  2: tolerance -----------------------------------------
      // One millimeter tolerance.
   const double Position::ONE_MM_TOLERANCE = 0.001;
      // One centimeter tolerance.
   const double Position::ONE_CM_TOLERANCE = 0.01;
      // One micron tolerance.
   const double Position::ONE_UM_TOLERANCE = 0.000001;

      // Default tolerance for time equality in meters.
   double Position::POSITION_TOLERANCE = Position::ONE_MM_TOLERANCE;

      // Sets the tolerance for output and comparisons, for this object only.
      // See the constants in this file (e.g. ONE_MM_TOLERANCE)
      // for some easy to use tolerance values.
      // @param tol Tolerance in meters to be used by comparison operators.
   Position& Position::setTolerance(const double tol)
      noexcept
   {
      tolerance = tol;
      return *this;
   }

   // ----------- Part  3: member functions: constructors --------------------
   //
      // Default constructor.
   Position::Position()
      noexcept
   {
      WGS84Ellipsoid WGS84;
      initialize(0.0,0.0,0.0,Unknown,&WGS84);
   }

   Position::Position(const double& a,
                      const double& b,
                      const double& c,
                      Position::CoordinateSystem s,
                      const EllipsoidModel *ell,
                      ReferenceFrame frame)
   {
      try {
         initialize(a,b,c,s,ell,frame);
      }
      catch(GeometryException& ge) {
         GNSSTK_RETHROW(ge);
      }
   }

   Position::Position(const double ABC[3],
                      CoordinateSystem s,
                      const EllipsoidModel *ell,
                      ReferenceFrame frame)
   {
      double a=ABC[0];
      double b=ABC[1];
      double c=ABC[2];
      try {
         initialize(a,b,c,s,ell,frame);
      }
      catch(GeometryException& ge) {
         GNSSTK_RETHROW(ge);
      }
   }

   Position::Position(const Triple& ABC,
                      CoordinateSystem s,
                      const EllipsoidModel *ell,
                      ReferenceFrame frame)
   {
      double a=ABC[0];
      double b=ABC[1];
      double c=ABC[2];
      try {
         initialize(a,b,c,s,ell,frame);
      }
      catch(GeometryException& ge) {
      }
   }

   Position::Position(const Xvt& xvt)
      noexcept
   {
      double a=xvt.x[0];
      double b=xvt.x[1];
      double c=xvt.x[2];
      initialize(a,b,c,Cartesian, NULL, xvt.frame);
   }

   // ----------- Part  4: member functions: arithmetic ----------------------
   //
   Position& Position::operator-=(const Position& right)
      noexcept
   {
      Position r(right);
      CoordinateSystem savesys=system;    // save the original system

         // convert to cartestian and difference there
      transformTo(Cartesian);
      r.transformTo(Cartesian);

      for(int i=0; i<3; i++)
         theArray[i] -= r.theArray[i];

      transformTo(savesys);               // transform back to the original system
      return *this;
   }

   Position& Position::operator+=(const Position& right)
      noexcept
   {
      Position r(right);
      CoordinateSystem savesys=system;    // save the original system

         // convert to cartestian and difference there
      transformTo(Cartesian);
      r.transformTo(Cartesian);

      for(int i=0; i<3; i++)
         theArray[i] += r.theArray[i];

      transformTo(savesys);               // transform back to the original system
      return *this;
   }

   Position operator-(const Position& left,
                            const Position& right)
      noexcept
   {
      Position l(left),r(right);
         // convert both to Cartesian
      l.transformTo(Position::Cartesian);
      r.transformTo(Position::Cartesian);
         // difference
      l -= r;

      return l;
   }

   Position operator+(const Position& left,
                            const Position& right)
      noexcept
   {
      Position l(left),r(right);
         // convert both to Cartesian
      l.transformTo(Position::Cartesian);
      r.transformTo(Position::Cartesian);
         // add
      l += r;

      return l;
   }

   // ----------- Part  5: member functions: comparisons ---------------------
   //
      // Equality operator. Returns false if ell values differ.
   bool Position::operator==(const Position &right) const
      noexcept
   {
      if(AEarth != right.AEarth || eccSquared != right.eccSquared)
         return false;
      if(right.getReferenceFrame() != refFrame)
         return false;   //Unknown frames are considered the same.
      if(range(*this,right) < tolerance)
         return true;
      else
         return false;
   }

      // Inequality operator. Returns true if ell values differ.
   bool Position::operator!=(const Position &right) const
      noexcept
   {
      return !(operator==(right));
   }

   // ----------- Part  6: member functions: coordinate transformations ------
   //
      // Transform coordinate system. Does nothing if sys already matches the
      // current value of member CoordinateSystem 'system'.
      // @param sys coordinate system into which *this is to be transformed.
      // @return *this
   Position& Position::transformTo(CoordinateSystem sys)
      noexcept
   {
      if(sys == Unknown || sys == system) return *this;

      // this copies geoid information and tolerance
      Position target(*this);

      // transform target.theArray and set target.system
      switch(system) {
         case Unknown:
            return *this;
         case Geodetic:
            // --------------- Geodetic to ... ------------------------
            switch(sys) {
               case Unknown: case Geodetic: return *this;
               case Geocentric:
                  convertGeodeticToGeocentric(*this,target,AEarth,eccSquared);
                  target.system = Geocentric;
                  break;
               case Cartesian:
                  convertGeodeticToCartesian(*this,target,AEarth,eccSquared);
                  target.system = Cartesian;
                  break;
               case Spherical:
                  convertGeodeticToGeocentric(*this,target,AEarth,eccSquared);
                  target.theArray[0] = 90 - target.theArray[0];   // geocen -> sph
                  target.system = Spherical;
                  break;
            }
            break;
         case Geocentric:
            // --------------- Geocentric to ... ----------------------
            switch(sys) {
               case Unknown: case Geocentric: return *this;
               case Geodetic:
                  convertGeocentricToGeodetic(*this,target,AEarth,eccSquared);
                  target.system = Geodetic;
                  break;
               case Cartesian:
                  convertGeocentricToCartesian(*this,target);
                  target.system = Cartesian;
                  break;
               case Spherical:
                  target.theArray[0] = 90 - target.theArray[0];   // geocen -> sph
                  target.system = Spherical;
                  break;
            }
            break;
         case Cartesian:
            // --------------- Cartesian to ... -----------------------
            switch(sys) {
               case Unknown: case Cartesian: return *this;
               case Geodetic:
                  convertCartesianToGeodetic(*this,target,AEarth,eccSquared);
                  target.system = Geodetic;
                  break;
               case Geocentric:
                  convertCartesianToGeocentric(*this,target);
                  target.system = Geocentric;
                  break;
               case Spherical:
                  convertCartesianToSpherical(*this,target);
                  target.system = Spherical;
                  break;
            }
            break;
         case Spherical:
            // --------------- Spherical to ... -----------------------
            switch(sys) {
               case Unknown: case Spherical: return *this;
               case Geodetic:
                  theArray[0] = 90 - theArray[0];   // sph -> geocen
                  convertGeocentricToGeodetic(*this,target,AEarth,eccSquared);
                  target.system = Geodetic;
                  break;
               case Geocentric:
                  target.theArray[0] = 90 - target.theArray[0];   // sph -> geocen
                  target.system = Geocentric;
                  break;
               case Cartesian:
                  convertSphericalToCartesian(*this,target);
                  target.system = Cartesian;
                  break;
            }
            break;
      }  // end switch(system)

      *this = target;
      return *this;
   }

   // ----------- Part  7: member functions: get -----------------------------
   //
   // These routines retrieve coordinate values in all coordinate systems.
   // Note that calling these will transform the Position to another coordinate
   // system if that is required.
   //

   const ReferenceFrame& Position::getReferenceFrame() const
      noexcept
   {
      return refFrame;
   }

      // Get X coordinate (meters)
   double Position::X() const
      noexcept
   {
      if(system == Cartesian)
         return theArray[0];
      Position t(*this);
      t.transformTo(Cartesian);
      return t.theArray[0];
   }

      // Get Y coordinate (meters)
   double Position::Y() const
      noexcept
   {
      if(system == Cartesian)
         return theArray[1];
      Position t(*this);
      t.transformTo(Cartesian);
      return t.theArray[1];
   }

      // Get Z coordinate (meters)
   double Position::Z() const
      noexcept
   {
      if(system == Cartesian)
         return theArray[2];
      Position t(*this);
      t.transformTo(Cartesian);
      return t.theArray[2];
   }

      // Get geodetic latitude (degrees North).
   double Position::geodeticLatitude() const
      noexcept
   {
      if(system == Geodetic)
         return theArray[0];
      Position t(*this);
      t.transformTo(Geodetic);
      return t.theArray[0];
   }

      // Get geocentric latitude (degrees North),
      // equal to 90 degress - theta in regular spherical coordinates.
   double Position::geocentricLatitude() const
      noexcept
   {
      if(system == Geocentric)
         return theArray[0];
      Position t(*this);
      t.transformTo(Geocentric);
      return t.theArray[0];
   }

      // Get spherical coordinate theta in degrees
   double Position::theta() const
      noexcept
   {
      if(system == Spherical)
         return theArray[0];
      Position t(*this);
      t.transformTo(Spherical);
      return t.theArray[0];
   }

      // Get spherical coordinate phi in degrees
   double Position::phi() const
      noexcept
   {
      if(system == Spherical)
         return theArray[1];
      Position t(*this);
      t.transformTo(Spherical);
      return t.theArray[1];
   }

      // Get longitude (degrees East),
      // equal to phi in regular spherical coordinates.
   double Position::longitude() const
      noexcept
   {
      if(system != Cartesian)
         return theArray[1];
      Position t(*this);
      t.transformTo(Spherical);
      return t.theArray[1];
   }

      // Get radius or distance from the center of Earth (meters),
      // Same as radius in spherical coordinates.
   double Position::radius() const
      noexcept
   {
      if(system == Spherical || system == Geocentric)
         return theArray[2];
      Position t(*this);
      t.transformTo(Spherical);
      return t.theArray[2];
   }

      // Get height above ellipsoid (meters) (Geodetic).
   double Position::height() const
      noexcept
   {
      if(system == Geodetic)
         return theArray[2];
      Position t(*this);
      t.transformTo(Geodetic);
      return t.theArray[2];
   }

   // ----------- Part  8: member functions: set -----------------------------
   //
   void Position::setReferenceFrame(const ReferenceFrame& frame)
      noexcept
   {
      refFrame = frame;
   }

      /**
      * Set the ellipsoid values for this Position given a ellipsoid.
      * @param ell  Pointer to the EllipsoidModel.
      * @throw      GeometryException if input is NULL.
      */
   void Position::setEllipsoidModel(const EllipsoidModel *ell)
   {
      if(!ell)
      {
         GeometryException ge("Given EllipsoidModel pointer is NULL.");
         GNSSTK_THROW(ge);
      }
      AEarth = ell->a();
      eccSquared = ell->eccSquared();
   }

      // Set the Position given geodetic coordinates, system is set to Geodetic.
      // @param lat geodetic latitude in degrees North
      // @param lon geodetic longitude in degrees East
      // @param ht height above the ellipsoid in meters
      // @return a reference to this object.
      // @throw GeometryException on invalid input
   Position& Position::setGeodetic(const double lat,
                                   const double lon,
                                   const double ht,
                                   const EllipsoidModel *ell)
   {
      if(lat > 90 || lat < -90)
      {
         GeometryException ge("Invalid latitude in setGeodetic: "
                                 + StringUtils::asString(lat));
         GNSSTK_THROW(ge);
      }
      theArray[0] = lat;

      theArray[1] = lon;
      if(theArray[1] < 0)
         theArray[1] += 360*(1+(unsigned long)(theArray[1]/360));
      else if(theArray[1] >= 360)
         theArray[1] -= 360*(unsigned long)(theArray[1]/360);

      theArray[2] = ht;

      if(ell) {
         AEarth = ell->a();
         eccSquared = ell->eccSquared();
      }
      system = Geodetic;

      return *this;
   }

      // Set the Position given geocentric coordinates, system is set to Geocentric
      // @param lat geocentric latitude in degrees North
      // @param lon geocentric longitude in degrees East
      // @param rad radius from the Earth's center in meters
      // @return a reference to this object.
      // @throw GeometryException on invalid input
   Position& Position::setGeocentric(const double lat,
                                     const double lon,
                                     const double rad)
   {
      if(lat > 90 || lat < -90)
      {
         GeometryException ge("Invalid latitude in setGeocentric: "
                                 + StringUtils::asString(lat));
         GNSSTK_THROW(ge);
      }
      if(rad < 0)
      {
         GeometryException ge("Invalid radius in setGeocentric: "
                                          + StringUtils::asString(rad));
         GNSSTK_THROW(ge);
      }
      theArray[0] = lat;
      theArray[1] = lon;
      theArray[2] = rad;

      if(theArray[1] < 0)
         theArray[1] += 360*(1+(unsigned long)(theArray[1]/360));
      else if(theArray[1] >= 360)
         theArray[1] -= 360*(unsigned long)(theArray[1]/360);
      system = Geocentric;

      return *this;
   }

      // Set the Position given spherical coordinates, system is set to Spherical
      // @param theta angle from the Z-axis (degrees)
      // @param phi angle from the X-axis in the XY plane (degrees)
      // @param rad radius from the center in meters
      // @return a reference to this object.
      // @throw GeometryException on invalid input
   Position& Position::setSpherical(const double theta,
                                    const double phi,
                                    const double rad)
   {
      if(theta < 0 || theta > 180)
      {
         GeometryException ge("Invalid theta in setSpherical: "
                                 + StringUtils::asString(theta));
         GNSSTK_THROW(ge);
      }
      if(rad < 0)
      {
         GeometryException ge("Invalid radius in setSpherical: "
                                          + StringUtils::asString(rad));
         GNSSTK_THROW(ge);
      }

      theArray[0] = theta;
      theArray[1] = phi;
      theArray[2] = rad;

      if(theArray[1] < 0)
         theArray[1] += 360*(1+(unsigned long)(theArray[1]/360));
      else if(theArray[1] >= 360)
         theArray[1] -= 360*(unsigned long)(theArray[1]/360);
      system = Spherical;

      return *this;
   }

      // Set the Position given ECEF coordinates, system is set to Cartesian.
      // @param X ECEF X coordinate in meters.
      // @param Y ECEF Y coordinate in meters.
      // @param Z ECEF Z coordinate in meters.
      // @return a reference to this object.
   Position& Position::setECEF(const double X,
                               const double Y,
                               const double Z)
      noexcept
   {
      theArray[0] = X;
      theArray[1] = Y;
      theArray[2] = Z;
      system = Cartesian;
      return *this;
   }

   // ----------- Part 9: member functions: setToString, printf -------------
   //
      // setToString, similar to scanf, this function takes a string and a
      // format describing string in order to define Position
      // values.  The parameters it can take are listed below and
      // described above with the printf() function.
      //
      // The specification must be sufficient to define a Position.
      // The following table lists combinations that give valid
      // Positions. Anything more or other combinations will give
      // unknown (read as: "bad") results so don't try it.  Anything
      // less will throw an exception.
      //
      // @code
      //  %X %Y %Z  (cartesian or ECEF)
      //  %x %y %z  (cartesian or ECEF)
      //  %a %l %r  (geocentric)
      //  %A %L %h  (geodetic)
      //  %t %p %r  (spherical)
      // @endcode
      //
      // So
      // @code
      // pos.setToString("123.4342,9328.1982,-128987.399", "%X,%Y,%Z");
      // @endcode
      //
      // works but
      //
      // @code
      // pos.setToString("123.4342,9328.1982", "%X,%Y");
      // @endcode
      // doesn't work (incomplete specification because it doesn't specify
      // a Position).
      //
      // Whitespace is unimportant here, the function will handle it.
      // The caller must ensure that that the extra characters in
      // the format string (ie '.' ',') are in the same relative
      // location as they are in the actual string, see the example above.
      //
      // @param str string from which to get the Position coordinates
      // @param fmt format to use to parse \c str.
      // @throw GeometryException if \c fmt is an incomplete or invalid specification
      // @throw StringException if an error occurs manipulating the
      // \c str or \c fmt strings.
      // @return a reference to this object.
   Position& Position::setToString(const std::string& str,
                                   const std::string& fmt)
   {
      try {
            // make an object to return (so we don't fiddle with *this
            // until it's necessary)
         Position toReturn;

            // flags indicated these defined
         bool hX=false, hY=false, hZ=false;
         bool hglat=false, hlon=false, hht=false;
         bool hclat=false, hrad=false;
         bool htheta=false, hphi=false;
            // store input values
         double x=0.0, y=0.0, z=0.0, glat=0.0, lon=0.0, ht=0.0, clat=0.0,
                rad=0.0, theta=0.0, phi=0.0;
            // copy format and input string to parse
         string f = fmt;
         string s = str;
         stripLeading(s);
         stripTrailing(f);

            // parse strings...  As we process each part, it's removed from both
            // strings so when we reach 0, we're done
         while ( (s.size() > 0) && (f.size() > 0) )
         {
               // remove everything in f and s up to the first % in f
               // (these parts of the strings must be identical or this will break
               // after it tries to remove it!)
            while ( (s.length() != 0) && (f.length() != 0) && (f[0] != '%') )
            {
                  // remove that character now and other whitespace
               s.erase(0,1);
               f.erase(0,1);
               stripLeading(s);
               stripLeading(f);
            }

               // check just in case we hit the end of either string...
            if ( (s.length() == 0) || (f.length() == 0) )
               break;

               // lose the '%' in f...
            f.erase(0,1);

               // if the format string is like %03f, get '3' as the field
               // length.
            string::size_type fieldLength = string::npos;

            if (!isalpha(f[0]))
            {
               fieldLength = asInt(f);

                  // remove everything else up to the next character
                  // (in "%03f", that would be 'f')
               while ((!f.empty()) && (!isalpha(f[0])))
                  f.erase(0,1);
               if (f.empty())
                  break;
            }

               // finally, get the character that should end this field, if any
            char delimiter = 0;
            if (f.size() > 1)
            {
               if (f[1] != '%')
               {
                  delimiter = f[1];

                  if (fieldLength == string::npos)
                     fieldLength = s.find(delimiter,0);
               }
                  // if the there is no delimiter character and the next field
                  // is another part of the time to parse, assume the length
                  // of this field is 1
               else if (fieldLength == string::npos)
               {
                  fieldLength = 1;
               }
            }

               // figure out the next string to be removed.  if there is a field
               // length, use that first
            string toBeRemoved = s.substr(0, fieldLength);

               // based on char at f[0], we know what to do...
            switch (f[0])
            {
          //%x   X() (meters)
          //%y   Y() (meters)
          //%z   Z() (meters)
          //%X   X()/1000 (kilometers)
          //%Y   Y()/1000 (kilometers)
          //%Z   Z()/1000 (kilometers)
               case 'X':
                  x = asDouble(toBeRemoved) * 1000;
                  hX = true;
                  break;
               case 'x':
                  x = asDouble(toBeRemoved);
                  hX = true;
                  break;
               case 'Y':
                  y = asDouble(toBeRemoved) * 1000;
                  hY = true;
                  break;
               case 'y':
                  y = asDouble(toBeRemoved);
                  hY = true;
                  break;
               case 'Z':
                  z = asDouble(toBeRemoved) * 1000;
                  hZ = true;
                  break;
               case 'z':
                  z = asDouble(toBeRemoved);
                  hZ = true;
                  break;
          //%A   geodeticLatitude() (degrees North)
          //%a   geocentricLatitude() (degrees North)
               case 'A':
                  glat = asDouble(toBeRemoved);
                  if(glat > 90. || glat < -90.) {
                     InvalidRequest f(
                           "Invalid geodetic latitude for setTostring: "
                           + toBeRemoved);
                     GNSSTK_THROW(f);
                  }
                  hglat = true;
                  break;
               case 'a':
                  clat = asDouble(toBeRemoved);
                  if(clat > 90. || clat < -90.) {
                     InvalidRequest f(
                           "Invalid geocentric latitude for setTostring: "
                           + toBeRemoved);
                     GNSSTK_THROW(f);
                  }
                  hclat = true;
                  break;
          //%L   longitude() (degrees East)
          //%l   longitude() (degrees East)
          //%w   longitude() (degrees West)
          //%W   longitude() (degrees West)
               case 'L':
               case 'l':
                  lon = asDouble(toBeRemoved);
                  if(lon < 0)
                     lon += 360*(1+(unsigned long)(lon/360));
                  else if(lon >= 360)
                     lon -= 360*(unsigned long)(lon/360);
                  hlon = true;
                  break;
               case 'w':
               case 'W':
                  lon = 360.0 - asDouble(toBeRemoved);
                  if(lon < 0)
                     lon += 360*(1+(unsigned long)(lon/360));
                  else if(lon >= 360)
                     lon -= 360*(unsigned long)(lon/360);
                  hlon = true;
                  break;
          //%t   theta() (degrees)
          //%T   theta() (radians)
               case 't':
                  theta = asDouble(toBeRemoved);
                  if(theta > 180. || theta < 0.) {
                     InvalidRequest f("Invalid theta for setTostring: "
                                                + toBeRemoved);
                     GNSSTK_THROW(f);
                  }
                  htheta = true;
                  break;
               case 'T':
                  theta = asDouble(toBeRemoved) * RAD_TO_DEG;
                  if(theta > 90. || theta < -90.) {
                     InvalidRequest f("Invalid theta for setTostring: "
                                                + toBeRemoved);
                     GNSSTK_THROW(f);
                  }
                  htheta = true;
                  break;
          //%p   phi() (degrees)
          //%P   phi() (radians)
               case 'p':
                  phi = asDouble(toBeRemoved);
                  if(phi < 0)
                     phi += 360*(1+(unsigned long)(phi/360));
                  else if(phi >= 360)
                     phi -= 360*(unsigned long)(phi/360);
                  hphi = true;
                  break;
               case 'P':
                  phi = asDouble(toBeRemoved) * RAD_TO_DEG;
                  if(phi < 0)
                     phi += 360*(1+(unsigned long)(phi/360));
                  else if(phi >= 360)
                     phi -= 360*(unsigned long)(phi/360);
                  hphi = true;
                  break;
          //%r   radius() meters
          //%R   radius()/1000 kilometers
          //%h   height() meters
          //%H   height()/1000 kilometers
               case 'r':
                  rad = asDouble(toBeRemoved);
                  if(rad < 0.0) {
                     InvalidRequest f("Invalid radius for setTostring: "
                                                + toBeRemoved);
                     GNSSTK_THROW(f);
                  }
                  hrad = true;
                  break;
               case 'R':
                  rad = asDouble(toBeRemoved) * 1000;
                  if(rad < 0.0) {
                     InvalidRequest f("Invalid radius for setTostring: "
                                                + toBeRemoved);
                     GNSSTK_THROW(f);
                  }
                  hrad = true;
                  break;
               case 'h':
                  ht = asDouble(toBeRemoved);
                  hht = true;
                  break;
               case 'H':
                  ht = asDouble(toBeRemoved) * 1000;
                  hht = true;
                  break;
               default: // do nothing
                  break;
            }
               // remove the part of s that we processed
            stripLeading(s,toBeRemoved,1);

               // remove the character we processed from f
            f.erase(0,1);

               // check for whitespace again...
            stripLeading(f);
            stripLeading(s);

         }

         if ( s.length() != 0  )
         {
               // throw an error - something didn't get processed in the strings
            InvalidRequest fe(
               "Processing error - parts of strings left unread - " + s);
            GNSSTK_THROW(fe);
         }

         if (f.length() != 0)
         {
               // throw an error - something didn't get processed in the strings
            InvalidRequest fe(
               "Processing error - parts of strings left unread - " + f);
            GNSSTK_THROW(fe);
         }

            // throw if the specification is incomplete
         if ( !(hX && hY && hZ) && !(hglat && hlon && hht) &&
              !(hclat && hlon && hrad) && !(htheta && hphi && hrad)) {
            InvalidRequest fe("Incomplete specification for setToString");
            GNSSTK_THROW(fe);
         }

            // define the Position toReturn
         if(hX && hY && hZ)
            toReturn.setECEF(x,y,z);
         else if(hglat && hlon && hht)
            toReturn.setGeodetic(glat,lon,ht);
         else if(hclat && hlon && hrad)
            toReturn.setGeocentric(clat,lon,rad);
         else if(htheta && hphi && hrad)
            toReturn.setSpherical(theta,phi,rad);

         *this = toReturn;
         return *this;
      }
      catch(gnsstk::Exception& exc)
      {
         GeometryException ge(exc);
         ge.addText("Failed to convert string to Position");
         GNSSTK_THROW(ge);
      }
      catch(std::exception& exc)
      {
         GeometryException ge(exc.what());
         ge.addText("Failed to convert string to Position");
         GNSSTK_THROW(ge);
      }
   }

      // Format this Position into a string.
      //
      // Generate and return a string containing formatted
      // Position coordinates, formatted by the specification \c fmt.
      //
      // \li \%x   X() (meters)
      // \li \%y   Y() (meters)
      // \li \%z   Z() (meters)
      // \li \%X   X()/1000 (kilometers)
      // \li \%Y   Y()/1000 (kilometers)
      // \li \%Z   Z()/1000 (kilometers)
      // \li \%A   geodeticLatitude() (degrees North)
      // \li \%a   geocentricLatitude() (degrees North)
      // \li \%L   longitude() (degrees East)
      // \li \%l   longitude() (degrees East)
      // \li \%w   longitude() (degrees West)
      // \li \%W   longitude() (degrees West)
      // \li \%t   theta() (degrees)
      // \li \%T   theta() (radians)
      // \li \%p   phi() (degrees)
      // \li \%P   phi() (radians)
      // \li \%r   radius() meters
      // \li \%R   radius()/1000 kilometers
      // \li \%h   height() meters
      // \li \%H   height()/1000 kilometers
      //
      // @param fmt format to use for this time.
      // @return a string containing this Position in the
      // representation specified by \c fmt.
   std::string Position::printf(const char *fmt) const
   {
      string rv = fmt;
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?x"),
                          string("xf"), X());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?y"),
                          string("yf"), Y());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?z"),
                          string("zf"), Z());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?X"),
                          string("Xf"), X()/1000);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?Y"),
                          string("Yf"), Y()/1000);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?Z"),
                          string("Zf"), Z()/1000);

      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?A"),
                          string("Af"), geodeticLatitude());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?a"),
                          string("af"), geocentricLatitude());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?L"),
                          string("Lf"), longitude());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?l"),
                          string("lf"), longitude());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?w"),
                          string("wf"), 360-longitude());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?W"),
                          string("Wf"), 360-longitude());

      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?t"),
                          string("tf"), theta());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?T"),
                          string("Tf"), theta()*DEG_TO_RAD);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?p"),
                          string("pf"), phi());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?P"),
                          string("Pf"), phi()*DEG_TO_RAD);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?r"),
                          string("rf"), radius());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?R"),
                          string("Rf"), radius()/1000);
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?h"),
                          string("hf"), height());
      rv = formattedPrint(rv, string("%[ 0-]?[[:digit:]]*(\\.[[:digit:]]+)?H"),
                          string("Hf"), height()/1000);
      return rv;
   }

      // Returns the string that operator<<() would print.
   string Position::asString() const
   {
      ostringstream o;
      o << *this;
      return o.str();
   }

   // ----------- Part 10: functions: fundamental conversions ---------------
   //
      // Fundamental conversion from spherical to cartesian coordinates.
      // @param trp (input): theta, phi, radius
      // @param xyz (output): X,Y,Z in units of radius
      // Algorithm references: standard geometry.
   void Position::convertSphericalToCartesian(const Triple& tpr,
                                              Triple& xyz)
      noexcept
   {
      double st=::sin(tpr[0]*DEG_TO_RAD);
      xyz[0] = tpr[2]*st*::cos(tpr[1]*DEG_TO_RAD);
      xyz[1] = tpr[2]*st*::sin(tpr[1]*DEG_TO_RAD);
      xyz[2] = tpr[2]*::cos(tpr[0]*DEG_TO_RAD);
   }

      // Fundamental routine to convert cartesian to spherical coordinates.
      // @param xyz (input): X,Y,Z
      // @param trp (output): theta, phi (deg), radius in units of input
      // Algorithm references: standard geometry.
   void Position::convertCartesianToSpherical(const Triple& xyz,
                                              Triple& tpr)
      noexcept
   {
      tpr[2] = RSS(xyz[0],xyz[1],xyz[2]);
      if(tpr[2] <= Position::POSITION_TOLERANCE/5) { // zero-length Cartesian vector
         tpr[0] = 90;
         tpr[1] = 0;
         return;
      }
      tpr[0] = ::acos(xyz[2]/tpr[2]);
      tpr[0] *= RAD_TO_DEG;
      if(RSS(xyz[0],xyz[1]) < Position::POSITION_TOLERANCE/5) {       // pole
         tpr[1] = 0;
         return;
      }
      tpr[1] = ::atan2(xyz[1],xyz[0]);
      tpr[1] *= RAD_TO_DEG;
      if(tpr[1] < 0) tpr[1] += 360;
   }

      // Fundamental routine to convert cartesian (ECEF) to geodetic coordinates,
      // (Geoid specified by semi-major axis and eccentricity squared).
      // @param xyz (input): X,Y,Z in meters
      // @param llh (output): geodetic lat(deg N), lon(deg E),
      //                             height above ellipsoid (meters)
      // @param A (input) Earth semi-major axis
      // @param eccSq (input) square of Earth eccentricity
      // Algorithm references:
   void Position::convertCartesianToGeodetic(const Triple& xyz,
                                             Triple& llh,
                                             const double A,
                                             const double eccSq)
      noexcept
   {
      double p,slat,N,htold,latold;
      p = SQRT(xyz[0]*xyz[0]+xyz[1]*xyz[1]);
      if(p < Position::POSITION_TOLERANCE/5) {  // pole or origin
         llh[0] = (xyz[2] > 0 ? 90.0: -90.0);
         llh[1] = 0;                            // lon undefined, really
         llh[2] = ::fabs(xyz[2]) - A*SQRT(1.0-eccSq);
         return;
      }
      llh[0] = ::atan2(xyz[2], p*(1.0-eccSq));
      llh[2] = 0;
      for(int i=0; i<5; i++) {
         slat = ::sin(llh[0]);
         N = A / SQRT(1.0 - eccSq*slat*slat);
         htold = llh[2];
         llh[2] = p/::cos(llh[0]) - N;
         latold = llh[0];
         llh[0] = ::atan2(xyz[2], p*(1.0-eccSq*(N/(N+llh[2]))));
         if(::fabs(llh[0]-latold) < 1.0e-9 && fabs(llh[2]-htold) < 1.0e-9 * A) break;
      }
      llh[1] = ::atan2(xyz[1],xyz[0]);
      if(llh[1] < 0.0) llh[1] += TWO_PI;
      llh[0] *= RAD_TO_DEG;
      llh[1] *= RAD_TO_DEG;
   }

      // Fundamental routine to convert geodetic to cartesian (ECEF) coordinates,
      // (Geoid specified by semi-major axis and eccentricity squared).
      // @param llh (input): geodetic lat(deg N), lon(deg E),
      //            height above ellipsoid (meters)
      // @param xyz (output): X,Y,Z in meters
      // @param A (input) Earth semi-major axis
      // @param eccSq (input) square of Earth eccentricity
      // Algorithm references:
   void Position::convertGeodeticToCartesian(const Triple& llh,
                                             Triple& xyz,
                                             const double A,
                                             const double eccSq)
      noexcept
   {
      double slat = ::sin(llh[0]*DEG_TO_RAD);
      double clat = ::cos(llh[0]*DEG_TO_RAD);
      double N = A/SQRT(1.0-eccSq*slat*slat);
      xyz[0] = (N+llh[2])*clat*::cos(llh[1]*DEG_TO_RAD);
      xyz[1] = (N+llh[2])*clat*::sin(llh[1]*DEG_TO_RAD);
      xyz[2] = (N*(1.0-eccSq)+llh[2])*slat;
   }

      // Fundamental routine to convert cartesian (ECEF) to geocentric coordinates.
      // @param xyz (input): X,Y,Z in meters
      // @param llr (output):
      //            geocentric lat(deg N),lon(deg E),radius (units of input)
   void Position::convertCartesianToGeocentric(const Triple& xyz,
                                               Triple& llr)
      noexcept
   {
      convertCartesianToSpherical(xyz, llr);
      llr[0] = 90 - llr[0];         // convert theta to latitude
   }

      // Fundamental routine to convert geocentric to cartesian (ECEF) coordinates.
      // @param llr (input): geocentric lat(deg N),lon(deg E),radius
      // @param xyz (output): X,Y,Z (units of radius)
   void Position::convertGeocentricToCartesian(const Triple& llr,
                                               Triple& xyz)
      noexcept
   {
      Triple llh(llr);
      llh[0] = 90 - llh[0];         // convert latitude to theta
      convertSphericalToCartesian(llh, xyz);
   }

      // Fundamental routine to convert geocentric to geodetic coordinates.
      // @param llr (input): geocentric Triple: lat(deg N),lon(deg E),radius (meters)
      // @param llh (output): geodetic latitude (deg N),
      //            longitude (deg E), and height above ellipsoid (meters)
      // @param A (input) Earth semi-major axis
      // @param eccSq (input) square of Earth eccentricity
   void Position::convertGeocentricToGeodetic(const Triple& llr,
                                               Triple& llh,
                                               const double A,
                                               const double eccSq)
      noexcept
   {
      double cl,p,sl,slat,N,htold,latold;
      llh[1] = llr[1];   // longitude is unchanged
      cl = ::sin((90-llr[0])*DEG_TO_RAD);
      sl = ::cos((90-llr[0])*DEG_TO_RAD);
      if(llr[2] <= Position::POSITION_TOLERANCE/5) {
         // radius is below tolerance, hence assign zero-length
         // arbitrarily set latitude = longitude = 0
         llh[0] = llh[1] = 0;
         llh[2] = -A;
         return;
      }
      else if(cl < 1.e-10) {
         // near pole ... note that 1mm/radius(Earth) = 1.5e-10
         if(llr[0] < 0) llh[0] = -90;
         else           llh[0] =  90;
         llh[1] = 0;
         llh[2] = llr[2] - A*SQRT(1-eccSq);
         return;
      }
      llh[0] = ::atan2(sl, cl*(1.0-eccSq));
      p = cl*llr[2];
      llh[2] = 0;
      for(int i=0; i<5; i++) {
         slat = ::sin(llh[0]);
         N = A / SQRT(1.0 - eccSq*slat*slat);
         htold = llh[2];
         llh[2] = p/::cos(llh[0]) - N;
         latold = llh[0];
         llh[0] = ::atan2(sl, cl*(1.0-eccSq*(N/(N+llh[2]))));
         if(fabs(llh[0]-latold) < 1.0e-9 && ::fabs(llh[2]-htold) < 1.0e-9 * A) break;
      }
      llh[0] *= RAD_TO_DEG;
   }

      // Fundamental routine to convert geodetic to geocentric coordinates.
      // @param geodeticllh (input): geodetic latitude (deg N),
      //            longitude (deg E), and height above ellipsoid (meters)
      // @param llr (output): geocentric lat (deg N),lon (deg E),radius (meters)
      // @param A (input) Earth semi-major axis
      // @param eccSq (input) square of Earth eccentricity
   void Position::convertGeodeticToGeocentric(const Triple& llh,
                                              Triple& llr,
                                              const double A,
                                              const double eccSq)
      noexcept
   {
      double slat = ::sin(llh[0]*DEG_TO_RAD);
      double N = A/SQRT(1.0-eccSq*slat*slat);
      // longitude is unchanged
      llr[1] = llh[1];
      // radius
      llr[2] = SQRT((N+llh[2])*(N+llh[2]) + N*eccSq*(N*eccSq-2*(N+llh[2]))*slat*slat);
      if(llr[2] <= Position::POSITION_TOLERANCE/5) {
         // radius is below tolerance, hence assign zero-length
         // arbitrarily set latitude = longitude = 0
         llr[0] = llr[1] = llr[2] = 0;
         return;
      }
      if(1-::fabs(slat) < 1.e-10) {             // at the pole
         if(slat < 0) llr[0] = -90;
         else         llr[0] =  90;
         llr[1] = 0.0;
         return;
      }
      // theta
      llr[0] = ::acos((N*(1-eccSq)+llh[2])*slat/llr[2]);
      llr[0] *= RAD_TO_DEG;
      llr[0] = 90 - llr[0];
   }

   // ----------- Part 11: operator<< and other useful functions -------------
   //
     // Stream output for Position objects.
     // @param s stream to append formatted Position to.
     // @param t Position to append to stream \c s.
     // @return reference to \c s.
   ostream& operator<<(ostream& s, const Position& p)
   {
      if(p.system == Position::Cartesian)
         s << p.printf("%.4x m %.4y m %.4z m");
      else if(p.system == Position::Geodetic)
         s << p.printf("%.8A degN %.8L degE %.4h m");
      else if(p.system == Position::Geocentric)
         s << p.printf("%.8a degN %.8L degE %.4r m");
      else if(p.system == Position::Spherical)
         s << p.printf("%.8t deg %.8p deg %.4r m");
      else
         s << " Unknown system! : " << p[0] << " " << p[1] << " " << p[2];

      return s;
   }

      // Compute the range in meters between this Position and
      // the Position passed as input.
      // @param right Position to which to find the range
      // @return the range (in meters)
      // @throw GeometryException if ell values differ
   double range(const Position& A,
                const Position& B)
   {
      if(A.AEarth != B.AEarth || A.eccSquared != B.eccSquared)
      {
         GeometryException ge("Unequal geoids");
         GNSSTK_THROW(ge);
      }

         Position L(A),R(B);
         L.transformTo(Position::Cartesian);
         R.transformTo(Position::Cartesian);
         double dif = RSS(L.X()-R.X(),L.Y()-R.Y(),L.Z()-R.Z());
         return dif;
      }

     // Compute the radius of the ellipsoidal Earth, given the geodetic latitude.
     // @param geolat geodetic latitude in degrees
     // @return the Earth radius (in meters)
   double Position::radiusEarth(const double geolat,
                                const double A,
                                const double eccSq)
      noexcept
   {
      double slat=::sin(DEG_TO_RAD*geolat);
      double e=(1.0-eccSq);
      double f=(1.0+(e*e-1.0)*slat*slat)/(1.0-eccSq*slat*slat);
      return (A * SQRT(f));
   }

      // A member function that computes the elevation of the input
      // (Target) position as seen from this Position.
      // @param Target the Position which is observed to have the
      //        computed elevation, as seen from this Position.
      // @return the elevation in degrees
   double Position::elevation(const Position& Target) const
   {
      Position R(*this),S(Target);
      R.transformTo(Cartesian);
      S.transformTo(Cartesian);
      // use Triple:: functions in cartesian coordinates (only)
      double elevation;
      try {
         elevation = R.elvAngle(S);
      }
      catch(GeometryException& ge)
      {
         GNSSTK_RETHROW(ge);
      }
      return elevation;
   }

      // A member function that computes the elevation of the input
      // (Target) position as seen from this Position, using a Geodetic
      // (i.e. ellipsoidal) system.
      // @param Target the Position which is observed to have the
      //        computed elevation, as seen from this Position.
      // @return the elevation in degrees
   double Position::elevationGeodetic(const Position& Target) const
   {
      Position R(*this),S(Target);
      double latGeodetic = R.getGeodeticLatitude()*DEG_TO_RAD;
      double longGeodetic = R.getLongitude()*DEG_TO_RAD;
      double localUp;
      double cosUp;
      R.transformTo(Cartesian);
      S.transformTo(Cartesian);
      Triple z;
      // Let's get the slant vector
      z = S.theArray - R.theArray;

      if (z.mag()<=1e-4) // if the positions are within .1 millimeter
      {
         GeometryException ge("Positions are within .1 millimeter");
         GNSSTK_THROW(ge);
      }

      // Compute k vector in local North-East-Up (NEU) system
      Triple kVector(::cos(latGeodetic)*::cos(longGeodetic), ::cos(latGeodetic)*::sin(longGeodetic), ::sin(latGeodetic));
      // Take advantage of dot method to get Up coordinate in local NEU system
      localUp = z.dot(kVector);
      // Let's get cos(z), being z the angle with respect to local vertical (Up);
      cosUp = localUp/z.mag();

      return 90.0 - ((::acos(cosUp))*RAD_TO_DEG);
   }

      // A member function that computes the azimuth of the input
      // (Target) position as seen from this Position.
      // @param Target the Position which is observed to have the
      //        computed azimuth, as seen from this Position.
      // @return the azimuth in degrees
   double Position::azimuth(const Position& Target) const
   {
      Position R(*this),S(Target);
      R.transformTo(Cartesian);
      S.transformTo(Cartesian);
      // use Triple:: functions in cartesian coordinates (only)
      double az;
      try
      {
         az = R.azAngle(S);

      }
      catch(GeometryException& ge)
      {
         GNSSTK_RETHROW(ge);
      }

      return az;
   }

      // A member function that computes the azimuth of the input
      // (Target) position as seen from this Position, using a Geodetic
      // (i.e. ellipsoidal) system.
      // @param Target the Position which is observed to have the
      //        computed azimuth, as seen from this Position.
      // @return the azimuth in degrees
   double Position::azimuthGeodetic(const Position& Target) const
   {
      Position R(*this),S(Target);
      double latGeodetic = R.getGeodeticLatitude()*DEG_TO_RAD;
      double longGeodetic = R.getLongitude()*DEG_TO_RAD;
      double localN, localE;
      R.transformTo(Cartesian);
      S.transformTo(Cartesian);
      Triple z;
      // Let's get the slant vector
      z = S.theArray - R.theArray;

      if (z.mag()<=1e-4) // if the positions are within .1 millimeter
      {
         GeometryException ge("Positions are within .1 millimeter");
         GNSSTK_THROW(ge);
      }

      // Compute i vector in local North-East-Up (NEU) system
      Triple iVector(-::sin(latGeodetic)*::cos(longGeodetic), -::sin(latGeodetic)*::sin(longGeodetic), ::cos(latGeodetic));
      // Compute j vector in local North-East-Up (NEU) system
      Triple jVector(-::sin(longGeodetic), ::cos(longGeodetic), 0);

      // Now, let's use dot product to get localN and localE unitary vectors
      localN = (z.dot(iVector))/z.mag();
      localE = (z.dot(jVector))/z.mag();

      // Let's test if computing azimuth has any sense
      double test = fabs(localN) + fabs(localE);

      // Warning: If elevation is very close to 90 degrees, we will return azimuth = 0.0
      if (test < 1.0e-16) return 0.0;

      double alpha = ((::atan2(localE, localN)) * RAD_TO_DEG);
      if (alpha < 0.0)
      {
         return alpha + 360.0;
      }
      else
      {
         return alpha;
      }
   }

     // A member function that computes the point at which a signal, which
     // is received at *this Position and there is observed at the input
     // azimuth and elevation, crosses a model ionosphere that is taken to
     // be a uniform thin shell at the input height. This algorithm is done
     // in geocentric coordinates.
     // A member function that computes the point at which a signal, which
     // is received at *this Position and there is observed at the input
     // azimuth and elevation, crosses a model ionosphere that is taken to
     // be a uniform thin shell at the input height. This algorithm is done
     // in geocentric coordinates.
     // @param elev elevation angle of the signal at reception, in degrees
     // @param azim azimuth angle of the signal at reception, in degrees
     // @param ionoht height of the ionosphere, in meters
     // @return Position IPP the position of the ionospheric pierce point,
     //     in the same coordinate system as *this; *this is not modified.
   Position Position::getIonosphericPiercePoint(const double elev,
                                                const double azim,
                                                const double ionoht) const
      noexcept
   {
      Position Rx(*this);

      // convert to Geocentric
      Rx.transformTo(Geocentric);

      // compute the geographic pierce point
      Position IPP(Rx);                   // copy system and geoid
      double el = elev * DEG_TO_RAD;
      // p is the angle subtended at Earth center by Rx and the IPP
      double p = PI/2.0 - el - ::asin(AEarth*::cos(el)/(AEarth+ionoht));
      double lat = Rx.theArray[0] * DEG_TO_RAD;
      double az = azim * DEG_TO_RAD;
      IPP.theArray[0] = std::asin(std::sin(lat)*std::cos(p) + std::cos(lat)*std::sin(p)*std::cos(az));
      IPP.theArray[1] = Rx.theArray[1]*DEG_TO_RAD
         + ::asin(::sin(p)*::sin(az)/::cos(IPP.theArray[0]));

      IPP.theArray[0] *= RAD_TO_DEG;
      IPP.theArray[1] *= RAD_TO_DEG;
      IPP.theArray[2] = AEarth + ionoht;

      // transform back
      IPP.transformTo(system);

      return IPP;
   }


        /**
        * A member function that computes the radius of curvature of the
        * meridian (Rm) corresponding to this Position.
        * @return radius of curvature of the meridian (in meters)
        */
    double Position::getCurvMeridian() const
        noexcept
    {

        double slat = ::sin(geodeticLatitude()*DEG_TO_RAD);
        double W = 1.0/SQRT(1.0-eccSquared*slat*slat);

        return AEarth*(1.0-eccSquared)*W*W*W;

    }

        /**
        * A member function that computes the radius of curvature in the
        * prime vertical (Rn) corresponding to this Position.
        * @return radius of curvature in the prime vertical (in meters)
        */
    double Position::getCurvPrimeVertical() const
        noexcept
    {

        double slat = ::sin(geodeticLatitude()*DEG_TO_RAD);

        return AEarth/SQRT(1.0-eccSquared*slat*slat);

    }


   Angle Position::getZenithAngle(const Position& target, AngleReduced& delta)
      const
   {
      Position p1(*this), p2(target);
      p1.transformTo(Geodetic);
      p2.transformTo(Geodetic);
      Angle phi1(p1.geodeticLatitude(), AngleType::Deg);
      Angle lambda1(p1.longitude(), AngleType::Deg);
      Angle phi2(p2.geodeticLatitude(), AngleType::Deg);
      Angle lambda2(p2.longitude(), AngleType::Deg);
         // radius requires spherical coordinates, so get them from
         // the original Position objects in the off-chance they were
         // already in the spherical system (we can be guaranteed p1
         // and p2 will not be).
      double r1 = radius();
      double r2 = target.radius();
      return getZenithAngle(phi1, lambda1, phi2, lambda2, r1, r2, delta);
   }


   Angle Position ::
   getZenithAngle(const Angle& phi1, const Angle& lambda1,
                  const Angle& phi2, const Angle& lambda2,
                  double r1, double r2,
                  AngleReduced& delta)
   {
      DEBUGTRACE_FUNCTION();
         // reference \cite galileo:iono though probably not exclusively
      delta.setValue(sin(phi1)*sin(phi2) +                              //eq.153
                     cos(phi1)*cos(phi2)*cos(lambda2-lambda1),
                     AngleType::Cos);
      DEBUGTRACE("delta.sin=" << scientific << delta.sin());
      DEBUGTRACE("delta.cos=" << scientific << delta.cos());
      return Angle(atan2(sin(delta),cos(delta) - (r1/r2)),              //eq.155
                   AngleType::Rad);
   }


   Position Position::getRayPerigee(const Position& target) const
   {
      DEBUGTRACE_FUNCTION();
         // reference \cite galileo:iono though probably not exclusively
      Position p1(*this), p2(target);
      p1.transformTo(Geodetic);
      p2.transformTo(Geodetic);
      Angle phi1(p1.geodeticLatitude(), AngleType::Deg);
      Angle lambda1(p1.longitude(), AngleType::Deg);
      Angle phi2(p2.geodeticLatitude(), AngleType::Deg);
      Angle lambda2(p2.longitude(), AngleType::Deg);
         // radius requires spherical coordinates, so get them from
         // the original Position objects in the off-chance they were
         // already in the spherical system (we can be guaranteed p1
         // and p2 will not be).
         // Also convert from m to km for the formulae below.
      double r1 = radius() / 1000.0;
      double r2 = target.radius() / 1000.0;
      AngleReduced delta;
      Angle zeta = getZenithAngle(phi1,lambda1,phi2,lambda2,r1,r2,delta);
      double rp = r1 * sin(zeta);                                       //eq.156
      DEBUGTRACE(setprecision(20) << "pStation_position->radius_km=" << scientific << r1);
      DEBUGTRACE("pZenith_angle->sin=" << scientific << sin(zeta));
      DEBUGTRACE("pRay->slant.perigee_radius_km=" << scientific << rp);
      Angle phiP, lambdaP;
      if (fabs(fabs(phi1.deg())-90) < 1e-10)
      {
         phiP = (phi1.deg() > 0) ? zeta : -zeta;                        //eq.157
         lambdaP.setValue((zeta.rad() >= 0) ? lambda2.rad() + PI :      //eq.164
                          lambda2.rad(), AngleType::Rad);
      }
      else
      {
         AngleReduced sigma(
            (sin(lambda2-lambda1) * cos(phi2)) / sin(delta),            //eq.158
            ((sin(phi2) - (cos(delta)*sin(phi1))) /                     //eq.159
             (sin(delta) * cos(phi1))));
         Angle deltaP(PI/2.0 - zeta.rad(), AngleType::Rad);             //eq.160
         phiP.setValue(sin(phi1)*cos(deltaP) -                          //eq.161
                       cos(phi1)*sin(deltaP)*cos(sigma), AngleType::Sin);
         Angle dLambda(-(sin(sigma)*sin(deltaP))/cos(phiP),             //eq.165
                       ((cos(deltaP)-sin(phi1)*sin(phiP)) /             //eq.166
                        (cos(phi1)*cos(phiP))));
         lambdaP = dLambda + lambda1;                                   //eq.167
      }
         // rp is in km, convert to meters for Position
      Position rv(phiP.deg(), lambdaP.deg(), rp*1000.0, Geocentric);
      rv.copyEllipsoidModelFrom(*this);
      return rv;
   }


   Position Position::getRayPosition(double dist, const Position& target) const
   {
      DEBUGTRACE_FUNCTION();
         // reference \cite galileo:iono though probably not exclusively
      Position p2(target);
      p2.transformTo(Geodetic);
      Position pp(getRayPerigee(target));
      Angle phi2(p2.geodeticLatitude(), AngleType::Deg);
      Angle lambda2(p2.longitude(), AngleType::Deg);
      Angle phip(pp.geodeticLatitude(), AngleType::Deg);
      Angle lambdap(pp.longitude(), AngleType::Deg);
      Angle dLambda(lambda2 - lambdap);
      AngleReduced psi; ///< Great circle angle from ray-perigee to satellite
      AngleReduced sigmap; ///< azimuth of satellite as seen from ray-perigee pp
      DEBUGTRACE("# pRay->latitude.rad=" << scientific << phip.rad());
      DEBUGTRACE("# pRay->latitude.degree=" << scientific << phip.deg());
      DEBUGTRACE("# pRay->latitude.sin=" << scientific << phip.sin());
      DEBUGTRACE("# pRay->latitude.cos=" << scientific << phip.cos());
      DEBUGTRACE("# pRay->longitude.rad=" << scientific << lambdap.rad());
      DEBUGTRACE("# pRay->longitude.degree=" << scientific << lambdap.deg());
      DEBUGTRACE("pRay->longitude.sin=" << scientific << lambdap.sin());
      DEBUGTRACE("pRay->longitude.cos=" << scientific << lambdap.cos());
      DEBUGTRACE("# pRay->satellite_position.latitude.rad=" << scientific << phi2.rad());
      DEBUGTRACE("# pRay->satellite_position.latitude.degree=" << scientific << phi2.deg());
      DEBUGTRACE("# pRay->satellite_position.latitude.sin=" << scientific << phi2.sin());
      DEBUGTRACE("# pRay->satellite_position.latitude.cos=" << scientific << phi2.cos());
      DEBUGTRACE("# pRay->satellite_position.longitude.rad=" << scientific << lambda2.rad());
      DEBUGTRACE("# pRay->satellite_position.longitude.degree=" << scientific << lambda2.deg());
      DEBUGTRACE("# pRay->satellite_position.longitude.sin=" << scientific << lambda2.sin());
      DEBUGTRACE("# pRay->satellite_position.longitude.cos=" << scientific << lambda2.cos());
      if (fabs(fabs(phip.deg())-90.0) < 1e-10)
      {
         psi.setValue(fabs(p2.geodeticLatitude()-pp.geodeticLatitude()),//eq.168
                      AngleType::Deg);
         if (phip.deg() > 0)
         {
            sigmap = Angle(0.0, -1.0);                                  //eq.173
         }
         else
         {
               // note that the equation says >0 or <0 but not ==0,
               // but the EU code does it this way as well (see
               // NeQuickG_JRC_ray.c, get_azimuth())
            sigmap = Angle(0.0, 1.0);                                   //eq.173
         }
      }
      else
      {
         psi = AngleReduced(sin(phip)*sin(phi2) +                       //eq.169
                            cos(phip)*cos(phi2)*cos(dLambda), AngleType::Cos);
         sigmap = AngleReduced(cos(phi2)*sin(dLambda)/sin(psi),         //eq.174
                               (sin(phi2)-sin(phip)*cos(psi)) /         //eq.175
                               (cos(phip)*sin(psi)));
      }
      DEBUGTRACE("# psi_angle.sin=" << scientific << psi.sin());
      DEBUGTRACE("# psi_angle.cos=" << scientific << psi.cos());
      DEBUGTRACE("# pRay->slant.azimuth.sin=" << scientific << sigmap.sin());
      DEBUGTRACE("# pRay->slant.azimuth.cos=" << scientific << sigmap.cos());
      double rp = pp.radius(); // radius in m
         // rs is also in meters now rather than km per the equation,
         // because the Position class prefers meters.  Also computing
         // as a geocentric radius so as to avoid dealing with
         // EllipsoidModels.
      double rs = sqrt(dist*dist + rp*rp);                              //eq.178
      double tanDeltas = dist / rp;                                     //eq.179
      double cosDeltas = 1/sqrt(1+tanDeltas*tanDeltas);                 //eq.180
      double sinDeltas = tanDeltas * cosDeltas;                         //eq.181
      Angle phis(sin(phip)*cosDeltas + cos(phip)*sinDeltas*cos(sigmap), //eq.182
                 AngleType::Sin);
      Angle dlambda(sinDeltas*sin(sigmap)*cos(phip),                    //eq.185
                    cosDeltas-sin(phip)*sin(phis));                     //eq.186
      double lambdas = dlambda.deg() + lambdap.deg();                   //eq.187
      Position rv(phis.deg(), lambdas, rs, Geocentric);
         // Still need to copy the ellipsoid model so that any
         // coordinate system conversions done by the user don't give
         // unexpected results
      rv.copyEllipsoidModelFrom(*this);
      DEBUGTRACE("current_position.radius_km=" << scientific << (rs / 1000.0));
      DEBUGTRACE("current_position.height=" << scientific << (rv.height() / 1000.0));
      DEBUGTRACE("current_position.latitude.rad=" << scientific << phis.rad());
      DEBUGTRACE("current_position.latitude.degree=" << scientific << phis.deg());
      DEBUGTRACE("current_position.latitude.sin=" << scientific << phis.sin());
      DEBUGTRACE("current_position.latitude.cos=" << scientific << phis.cos());
      DEBUGTRACE("current_position.longitude.degree=" << scientific << lambdas);
      return rv;
   }


   // ----------- Part 12: private functions and member data -----------------
   //
      // Initialization function, used by the constructors.
      // @param a coordinate [ X(m), or latitude (degrees N) ]
      // @param b coordinate [ Y(m), or longitude (degrees E) ]
      // @param c coordinate [ Z, height above ellipsoid or radius, in m ]
      // @param s CoordinateSystem, defaults to Cartesian
      // @param geiod pointer to a GeoidModel, default NULL (WGS84)
      // @throw GeometryException on invalid input.
   void Position::initialize(const double a,
                  const double b,
                  const double c,
                  Position::CoordinateSystem s,
                  const EllipsoidModel *ell,
                  ReferenceFrame frame)
   {
      double bb(b);
      if(s == Geodetic || s==Geocentric)
      {
         if(a > 90 || a < -90)
         {
            GeometryException ge("Invalid latitude in constructor: "
                                    + StringUtils::asString(a));
            GNSSTK_THROW(ge);
         }
         if(bb < 0)
            bb += 360*(1+(unsigned long)(bb/360));
         else if(bb >= 360)
            bb -= 360*(unsigned long)(bb/360);
      }
      if(s==Geocentric || s==Spherical)
      {
         if(c < 0)
         {
            GeometryException ge("Invalid radius in constructor: "
                                           + StringUtils::asString(c));
            GNSSTK_THROW(ge);
         }
      }
      if(s==Spherical)
      {
         if(a < 0 || a > 180)
         {
            GeometryException ge("Invalid theta in constructor: "
                                    + StringUtils::asString(a));
            GNSSTK_THROW(ge);
         }
         if(bb < 0)
            bb += 360*(1+(unsigned long)(bb/360));
         else if(bb >= 360)
            bb -= 360*(unsigned long)(bb/360);
      }

      theArray[0] = a;
      theArray[1] = bb;
      theArray[2] = c;

      if(ell) {
         AEarth = ell->a();
         eccSquared = ell->eccSquared();
      }
      else {
         WGS84Ellipsoid WGS84;
         AEarth = WGS84.a();
         eccSquared = WGS84.eccSquared();
      }
      system = s;
      tolerance = POSITION_TOLERANCE;
      refFrame = frame;
   }

}  // namespace gnsstk
