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

#ifndef GNSSTK_POSITION_HPP
#define GNSSTK_POSITION_HPP

#include "Exception.hpp"
#include "StringUtils.hpp"
#include "Triple.hpp"
#include "EllipsoidModel.hpp"
#include "ReferenceFrame.hpp"
#include "Xvt.hpp"
#include "Angle.hpp"
#include "gnsstk_export.h"

namespace gnsstk
{
      /// @ingroup Geodetic
      //@{

      // forward declarations
   class Position;
      /**
       * @throw GeometryException
       */
   double range(const Position& A, const Position& B);

      /**
       * A position representation class for common 3D geographic
       * position formats, including geodetic (geodetic latitude,
       * longitude, and height above the ellipsoid) geocentric
       * (geocentric latitude, longitude, and radius from Earth's
       * center), cartesian (Earth-centered, Earth-fixed) and
       * spherical (theta,phi,radius).
       *
       * Internally, the representation of Position consists of three
       * coordinate values (double), two doubles from a ellipsoid
       * model (see below, storing these doubles is preferred over
       * adding EllipsoidModel to calling arguments everywhere), a
       * flag of type 'enum CoordinateSystem' giving the coordinate
       * system, and a tolerance for use in comparing Positions. Class
       * Position inherits from class Triple, which is how the
       * coordinate values are stored (Triple actually uses
       * std::valarray<double> of length 3). It is important to note
       * that Triple:: routines are properly used by Positions ONLY in
       * the Cartesian coordinate system.
       *
       * Only geodetic coordinates depend on a ellipsoid, and then
       * only on the semi-major axis of the Earth and the square of
       * its eccentricity. Input of this ellipsoid information
       * (usually a pointer to a EllipsoidModel) is required by
       * functions involving constructors of, or transformation to or
       * from, Geodetic coordinates. However since a default is
       * supplied (WGS84), the user need never deal with geiods unless
       * desired.  In fact, if the geodetic coordinate system is
       * avoided, the Position class can be interpreted simply as 3D
       * vectors in any context, particularly since the class inherits
       * from Triple, which includes many vector manipulation routines
       * (although the Triple:: routines assume Cartesian
       * coordinates).  Even the requirement that lengths (radius,
       * height and the cartesian coordinates) have units of meters is
       * required only if geodetic coordinates are used (because the
       * semi-major axis in EllipsoidModel is in meters); without
       * using Geodetic one could apply the class using any units for
       * length as long as setTolerance() is called appropriately.
       *
       * Position relies on a series of fundamental routines to
       * transform from one coordinate system to another, these
       * include, for example void
       * Position::convertGeodeticToCartesian(const Triple& llh,
       * Triple& xyz, const double A, const double eccSq); void
       * Position::convertSphericalToCartesian(const Triple& tpr,
       * Triple& xyz); These functions use Triple in the calling
       * arguments.
       *
       * Position will throw exceptions (gnsstk::GeometryException) on
       * bad input (e.g. negative radius or latitude > 90 degrees);
       * otherwise the class attempts to handle all points, even the
       * pole and the origin, consistently and without throwing
       * exceptions.  At or very near the poles, the transformation
       * routines will set latitude = +/-90 degrees, which is theta =
       * 0 or 180, and (arbitrarily) longitude = 0. At or very near
       * the origin, the transformation routines will set latitude =
       * 0, which is theta = 90, and (arbitrarily) longitude = 0;
       * radius will be set to zero and geodetic height will be set to
       * -radius(Earth) (= -6378137.0 in WGS84). The tolerance used in
       * testing 'at or near the pole or origin' is radius <
       * POSITION_TOLERANCE/5.  Note that this implies that a Position
       * that is very near the origin may be SET to the exact origin
       * by the transformation routines, and that thereby information
       * about direction (e.g. latitude and longitude) may be
       * LOST. The user is warned to be very careful when working near
       * either the pole or the origin.
       *
       * Position includes setToString() and printf() functions
       * similar to those in gnsstk::CommonTime; this allows flexible
       * and powerful I/O of Position to strings and streams.
       *
       * @sa positiontest.cpp for examples.
       */
   class Position : public Triple
   {
   public:
         // ----------- Part  1: coordinate systems ---------------------------
         //
         /// The coordinate systems supported by Position
      enum CoordinateSystem
      {
         Unknown=0,  ///< unknown coordinate system
         Geodetic,   ///< geodetic latitude, longitude, and height above ellipsoid
         Geocentric, ///< geocentric (regular spherical coordinates)
         Cartesian,  ///< cartesian (Earth-centered, Earth-fixed)
         Spherical   ///< spherical coordinates (theta,phi,radius)
      };

         /// return string giving name of coordinate system
      std::string getSystemName()
         noexcept;

         // ----------- Part  2: member functions: tolerance ------------------
         //
         /// One millimeter tolerance.
      GNSSTK_EXPORT static const double ONE_MM_TOLERANCE;
         /// One centimeter tolerance.
      GNSSTK_EXPORT static const double ONE_CM_TOLERANCE;
         /// One micron tolerance.
      GNSSTK_EXPORT static const double ONE_UM_TOLERANCE;

         /// Default tolerance for time equality in days.
      GNSSTK_EXPORT static double POSITION_TOLERANCE;

         /// Changes the POSITION_TOLERANCE for all Position objects
      static double setPositionTolerance(const double tol)
      { POSITION_TOLERANCE = tol;  return POSITION_TOLERANCE; }

         /// Returns the current POSITION_TOLERANCE.
      static double getPositionTolerance()
      { return POSITION_TOLERANCE; }

         /**
          * Sets the tolerance for output and comparisons, for this
          * object only.
          * See the constants in this file (e.g. ONE_MM_TOLERANCE)
          * for some easy to use tolerance values.
          * @param tol Tolerance in meters to be used by comparison operators.
          * @sa Position-Specific Definitions
          */
      Position& setTolerance(const double tol)
         noexcept;

         // ----------- Part  3: member functions: constructors ---------------
         //
         /**
          * Default constructor.
          * Initializes to zero, Unknown coordinates
          */
      Position()
      noexcept;

         /**
          * Explicit constructor. Coordinate system may be specified
          * on input, but defaults to Cartesian. Pointer to
          * EllipsoidModel may be specified, but default is NULL (in
          * which case WGS84 values will be used).
          * @param a first coordinate [ X(m), or latitude (degrees N) ]
          * @param b second coordinate [ Y(m), or longitude (degrees E) ]
          * @param c third coordinate [ Z, height above ellipsoid or radius, in m ]
          * @param s coordinate system
          * @param ell pointer to EllipsoidModel
          * @throw GeometryException on invalid input.
          */
      Position(const double& a,
               const double& b,
               const double& c,
               CoordinateSystem s = Cartesian,
               const EllipsoidModel *ell = nullptr,
               ReferenceFrame frame = ReferenceFrame::Unknown);

         /**
          * Explicit constructor. Coordinate system may be specified
          * on input, but defaults to Cartesian. Pointer to
          * EllipsoidModel may be specified, but default is NULL (in
          * which case WGS84 values will be used).
          * @param ABC double array[3] coordinate values
          * @param s CoordinateSystem
          * @param ell pointer to EllipsoidModel
          * @throw GeometryException on invalid input.
          */
      Position(const double ABC[3],
               CoordinateSystem s = Cartesian,
               const EllipsoidModel *ell = nullptr,
               ReferenceFrame frame = ReferenceFrame::Unknown);

         /**
          * Explicit constructor. Coordinate system may be specified
          * on input, but defaults to Cartesian. Pointer to
          * EllipsoidModel may be specified, but default is NULL (in
          * which case WGS84 values will be used).
          * @param ABC coordinate values
          * @param s CoordinateSystem
          * @param ell pointer to EllipsoidModel
          * @throw GeometryException on invalid input.
          */
      Position(const Triple& ABC,
               CoordinateSystem s = Cartesian,
               const EllipsoidModel *ell = nullptr,
               ReferenceFrame frame = ReferenceFrame::Unknown);

         /**
          * Explicit constructor from Xvt. The coordinate system is Cartesian,
          * and the velocity and time information in the input is ignored.
          * @param xvt Input Xvt object, xvt.x contains the Cartesian coordinates
          */
      Position(const Xvt& xvt)
         noexcept;

         /// Destructor.
      ~Position()
      noexcept
      {}

         // ----------- Part  4: member functions: arithmetic -----------------
         //
         /** Subtract a Position from this Position. Perform the subtraction in
          * Cartesian coordinates, but return this Position to the system it
          * had originally.
          * @param right Position to subtract from this one.
          * @return new Position, in the original system.
          */
      Position& operator-=(const Position& right)
         noexcept;

         /** Add a Position to this Position. Perform the addition in
          * Cartesian coordinates, but return this Position to the system it
          * had originally.
          * @param right Position to add to this one.
          * @return new Position, in the original system.
          */
      Position& operator+=(const Position& right)
         noexcept;

         /**
          * Difference two Positions, returning result as a Position
          * in Cartesian coordinates, the only system in which a
          * position difference makes sense.
          * @param right Position to subtract from this one.
          * @return difference as Position.
          */
      friend Position operator-(const Position& left,
                                const Position& right)
         noexcept;

         /**
          * Add two Positions, returning result as a Position in Cartesian
          * coordinates, the only system in which a position sum makes sense.
          * @param right Position to add to this one.
          * @return The new Position.
          */
      friend Position operator+(const Position& left,
                                const Position& right)
         noexcept;

         /** Multiply a Position by a double scalar on the left.
          * @param right Position to be multiplied by the scalar
          * @param scale the (double) scalar
          * @return The new Position.
          */
      friend Position operator*(const double& scale,
                                const Position& right)
      {
         Position tmp(right);
         tmp.theArray *= scale;
         return tmp;
      }

         /** Multiply a Position by a double scalar on the right.
          * @param left Position to be multiplied by the scalar
          * @param scale the (double) scalar
          * @return The new Position.
          */
      friend Position operator*(const Position& left,
                                const double& scale)
      {
         return operator*(scale, left);
      }

         /** Multiply a Position by an integer scalar on the left.
          * @param right Position to be multiplied by the scalar
          * @param scale the (int) scalar
          * @return The new Position.
          */
      friend Position operator*(const int& scale,
                                const Position& right)
      {
         return operator*(double(scale), right);
      }

         /** Multiply a Position by an integer scalar on the right.
          * @param left Position to be multiplied by the scalar
          * @param scale the (int) scalar
          * @return The new Position.
          */
      friend Position operator*(const Position& left,
                                const int& scale)
      {
         return operator*(double(scale), left);
      }

         // ----------- Part  5: member functions: comparisons ----------------
         //
         /** Equality operator. Return true if range between this
          * Position and the input Position is less than
          * tolerance. Return false if ellipsoid values differ.
          * @param right Position to be compared to this Position */
      bool operator==(const Position &right) const
         noexcept;

         /** Inequality operator. Return true if range between this
          * Position and the input Position is greater than
          * tolerance. Return true if ellipsoid values differ.
          * @param right Position to be compared to this Position */
      bool operator!=(const Position &right) const
         noexcept;

         // ----------- Part  6: member functions: coordinate transformations -
         //
         /**
          * Transform coordinate system. Does nothing if sys already
          * matches the current value of member CoordinateSystem
          * 'system'.
          * @param sys CoordinateSystem into which this Position is transformed
          */
      Position& transformTo(CoordinateSystem sys)
         noexcept;

         /// Convert to geodetic coordinates (does nothing if
         /// system == Geodetic already).
      Position& asGeodetic()
         noexcept
      { transformTo(Geodetic); return *this; }

         /// Convert to another ell, then to geodetic coordinates.
         /// @return a reference to this.
         /// @throw GeometryException if input is NULL.
      Position& asGeodetic(EllipsoidModel *ell)
      {
         try { setEllipsoidModel(ell); }
         catch(GeometryException& ge) { GNSSTK_RETHROW(ge); }
         transformTo(Geodetic);
         return *this;
      }

         /// Convert to cartesian coordinates (does nothing if
         /// system == Cartesian already).
      Position& asECEF()
         noexcept
      { transformTo(Cartesian); return *this; }


         // ----------- Part  7: member functions: get ------------------------
         //
         // These routines retrieve coordinate values in all
         // coordinate systems.
         //

         /// return coordinate ReferenceFrame
      const ReferenceFrame& getReferenceFrame() const
         noexcept;

         /// return X coordinate (meters)
      double X() const
         noexcept;

         /// return Y coordinate (meters)
      double Y() const
         noexcept;

         /// return Z coordinate (meters)
      double Z() const
         noexcept;

         /// return geodetic latitude (degrees North).
      double geodeticLatitude() const
         noexcept;

         /// return geocentric latitude (degrees North);
         /// equal to 90 degress - theta in regular spherical coordinates.
      double geocentricLatitude() const
         noexcept;

         /// return spherical coordinate theta in degrees
      double theta() const
         noexcept;

         /// return spherical coordinate phi in degrees
      double phi() const
         noexcept;

         /// return longitude (degrees East);
         /// equal to phi in regular spherical coordinates.
      double longitude() const
         noexcept;

         /// return distance from the center of Earth (meters),
         /// Same as radius in spherical coordinates.
      double radius() const
         noexcept;

         /// return height above ellipsoid (meters) (Geodetic).
      double height() const
         noexcept;

         /// return the coordinate system for this Position
      CoordinateSystem getCoordinateSystem() const
         noexcept
      { return system; };

         /// return geodetic latitude (deg N)
      double getGeodeticLatitude() const
         noexcept
      { return geodeticLatitude(); }

         /// return geocentric latitude (deg N)
      double getGeocentricLatitude() const
         noexcept
      { return geocentricLatitude(); }

         /// return longitude (deg E) (either geocentric or geodetic)
      double getLongitude() const
         noexcept
      { return longitude(); }

         /// return height above ellipsoid (meters)
      double getAltitude() const
         noexcept
      { return height(); }

         /// return height above ellipsoid (meters)
      double getHeight() const
         noexcept
      { return height(); }

         /// return ECEF X coordinate (meters)
      double getX() const
         noexcept
      { return X(); }

         /// return ECEF Y coordinate (meters)
      double getY() const
         noexcept
      { return Y(); }

         /// return ECEF Z coordinate (meters)
      double getZ() const
         noexcept
      { return Z(); }

         /** @return spherical coordinate angle theta (deg) (90 -
          * geocentric latitude) */
      double getTheta() const
         noexcept
      { return theta(); }

         /// return spherical coordinate angle phi (deg) (same as longitude)
      double getPhi() const
         noexcept
      { return phi(); }

         /// return radius
      double getRadius() const
         noexcept
      { return radius(); }

         // ----------- Part  8: member functions: set ------------------------
         //
         /**
          * Set the ReferenceFrame that this position is in.
          * @param frame The ReferenceFrame to set to.
          */
      void setReferenceFrame(const ReferenceFrame& frame)
         noexcept;

         /**
          * Set the ellipsoid values for this Position given a ellipsoid.
          * @param ell  Pointer to the EllipsoidModel.
          * @throw      GeometryException if input is NULL.
          */
      void setEllipsoidModel(const EllipsoidModel *ell);

         /**
          * Set the Position given geodetic coordinates; system is set
          * to Geodetic.
          * @param lat geodetic latitude in degrees North
          * @param lon geodetic longitude in degrees East
          * @param ht height above the ellipsoid in meters
          * @return a reference to this object.
          * @throw GeometryException on invalid input
          */
      Position& setGeodetic(const double lat,
                            const double lon,
                            const double ht,
                            const EllipsoidModel *ell = nullptr);

         /**
          * Set the Position given geocentric coordinates; system is
          * set to Geocentric
          * @param lat geocentric latitude in degrees North
          * @param lon geocentric longitude in degrees East
          * @param rad radius from the Earth's center in meters
          * @return a reference to this object.
          * @throw GeometryException on invalid input
          */
      Position& setGeocentric(const double lat,
                              const double lon,
                              const double rad);

         /**
          * Set the Position given spherical coordinates; system is
          * set to Spherical
          * @param theta angle from the Z-axis (degrees)
          * @param phi angle from the X-axis in the XY plane (degrees)
          * @param rad radius from the center in meters
          * @return a reference to this object.
          * @throw GeometryException on invalid input
          */
      Position& setSpherical(const double theta,
                             const double phi,
                             const double rad);

         /**
          * Set the Position given ECEF coordinates; system is set to
          * Cartesian.
          * @param X ECEF X coordinate in meters.
          * @param Y ECEF Y coordinate in meters.
          * @param Z ECEF Z coordinate in meters.
          * @return a reference to this object.
          */
      Position& setECEF(const double X,
                        const double Y,
                        const double Z)
         noexcept;

         /**
          * Set the Position given an array of ECEF coordinates;
          * system is set to Cartesian.
          * @param XYZ array[3] ECEF X,Y,Z coordinate in meters.
          * @return a reference to this object.
          */
      Position& setECEF(const double XYZ[3])
         noexcept
      { return setECEF(XYZ[0],XYZ[1],XYZ[2]); }

         /**
          * Set the Position given ECEF coordinates; system is set to
          * Cartesian.
          * @param XYZ ECEF X,Y,Z coordinates in meters.
          * @return a reference to this object.
          */
      Position& setECEF(const Triple& XYZ)
         noexcept
      { return setECEF(XYZ[0],XYZ[1],XYZ[2]); }

         // ----------- Part 9: member functions: setToString, printf ---------
         //
         /**
          * setToString, similar to scanf, this function takes a string and a
          * format describing string in order to define Position
          * values.  The parameters it can take are listed below and
          * described above with the printf() function.
          *
          * The specification must be sufficient to define a Position.
          * The following table lists combinations that give valid
          * Positions. Anything more or other combinations will give
          * unknown (read as: "bad") results so don't try it.  Anything
          * less will throw an exception.
          *
          * @code
          *  %X %Y %Z  (cartesian or ECEF in kilometers)
          *  %x %y %z  (cartesian or ECEF in meters)
          *  %a %l %r  (geocentric lat,lon,radius, longitude E, radius in meters)
          *  %A %L %h  (geodetic lat,lon,height, longitude E, height in meters)
          *  %a %w %R  (geocentric lat,lon,radius, longitude W, radius in kilometers)
          *  %A %W %H  (geodetic lat,lon,height, longitude W, height in kilometers)
          *  %t %p %r  (spherical theta, phi, radius, degrees and meters)
          *  %T %P %R  (spherical theta, phi, radius, radians and kilometers)
          * @endcode
          *
          * So
          * @code
          * pos.setToString("123.4342,9328.1982,-128987.399", "%X,%Y,%Z");
          * @endcode
          *
          * works but
          *
          * @code
          * pos.setToString("123.4342,9328.1982", "%X,%Y");
          * @endcode
          * doesn't work (incomplete specification because it doesn't specify
          * a Position).
          *
          * Whitespace is unimportant here; the function will handle it.
          * The caller must ensure that that the extra characters in
          * the format string (ie '.' ',') are in the same relative
          * location as they are in the actual string; see the example above.
          *
          * @param str string from which to get the Position coordinates
          * @param fmt format to use to parse \c str.
          * @throw GeometryException if \c fmt is an incomplete or invalid
          *    specification
          * @throw StringException if an error occurs manipulating the
          * \c str or \c fmt strings.
          * @return a reference to this object.
          */
      Position& setToString(const std::string& str,
                            const std::string& fmt);


         // if you can see this, ignore the \'s below, as they are for
         // the nasty html-ifying of doxygen.  Browsers try to
         // interpret the % and they get all messed up.
         /**
          * Format this Position into a string.
          *
          * Generate and return a string containing formatted
          * Position coordinates, formatted by the specification \c fmt.
          *
          * \li \%x   X() (meters)
          * \li \%y   Y() (meters)
          * \li \%z   Z() (meters)
          * \li \%X   X()/1000 (kilometers)
          * \li \%Y   Y()/1000 (kilometers)
          * \li \%Z   Z()/1000 (kilometers)
          * \li \%A   geodeticLatitude() (degrees North)
          * \li \%a   geocentricLatitude() (degrees North)
          * \li \%L   longitude() (degrees East)
          * \li \%l   longitude() (degrees East)
          * \li \%w   longitude() (degrees West)
          * \li \%W   longitude() (degrees West)
          * \li \%t   theta() (degrees)
          * \li \%T   theta() (radians)
          * \li \%p   phi() (degrees)
          * \li \%P   phi() (radians)
          * \li \%r   radius() meters
          * \li \%R   radius()/1000 kilometers
          * \li \%h   height() meters
          * \li \%H   height()/1000 kilometers
          *
          * @param fmt format to use for this time.
          * @return a string containing this Position in the
          *   representation specified by \c fmt.
          * @throw StringUtils::StringException
          */
      std::string printf(const char *fmt) const;

         /** Format this time into a string.
          * @see printf(const char*)
          * @throw StringUtils::StringException
          */
      std::string printf(const std::string& fmt) const
      { return printf(fmt.c_str()); }

         /** Returns the string that operator<<() would print.
          * @throw StringUtils::StringException
          */
      std::string asString() const;

         // ----------- Part 10: functions: fundamental conversions -----------
         //
         /** Fundamental conversion from spherical to cartesian coordinates.
          * @param[in] tpr theta, phi (degrees), radius
          * @param[out] xyz X,Y,Z in units of radius
          * Algorithm references: standard geometry.
          */
      static void convertSphericalToCartesian(const Triple& tpr,
                                              Triple& xyz)
         noexcept;

         /** Fundamental routine to convert cartesian to spherical coordinates.
          * The zero vector is converted to (90,0,0).
          * @param[in] xyz X,Y,Z
          * @param[out] tpr theta, phi (degrees), radius (units of input)
          * Algorithm references: standard geometry.
          */
      static void convertCartesianToSpherical(const Triple& xyz,
                                              Triple& tpr)
         noexcept;


         /** Fundamental routine to convert ECEF (cartesian) to
          * geodetic coordinates, (Ellipsoid specified by semi-major
          * axis and eccentricity squared).  The zero vector is
          * converted to (90,0,-R(earth)).
          * @param[in] xyz X,Y,Z in meters
          * @param[out] llh geodetic lat(deg N), lon(deg E),
          *                             height above ellipsoid (meters)
          * @param[in] A Earth semi-major axis
          * @param[in] eccSq square of Earth eccentricity
          * Algorithm references: Leick, "GPS Satellite Surveying," 2nd edition.
          */
      static void convertCartesianToGeodetic(const Triple& xyz,
                                             Triple& llh,
                                             const double A,
                                             const double eccSq)
         noexcept;

         /** Fundamental routine to convert geodetic to ECEF
          * (cartesian) coordinates, (Ellipsoid specified by
          * semi-major axis and eccentricity squared).
          * @param[in] llh geodetic lat(deg N), lon(deg E),
          *                             height above ellipsoid (meters)
          * @param[in] A Earth semi-major axis
          * @param[out] xyz: X,Y,Z in meters
          * @param[in] eccSq square of Earth eccentricity
          * Algorithm references: Leick, "GPS Satellite Surveying," 2nd edition.
          */
      static void convertGeodeticToCartesian(const Triple& llh,
                                             Triple& xyz,
                                             const double A,
                                             const double eccSq)
         noexcept;


         /** Fundamental routine to convert cartesian (ECEF) to geocentric
          * The zero vector is converted to (0,0,0).
          * @param[in] xyz X,Y,Z
          * @param[out] llr geocentric lat(deg N), lon(deg E),
          *                              radius (units of input)
          */
      static void convertCartesianToGeocentric(const Triple& xyz,
                                               Triple& llr)
         noexcept;

         /** Fundamental routine to convert geocentric to cartesian (ECEF)
          * @param[in] llr geocentric lat(deg N),lon(deg E),radius
          * @param[out] xyz X,Y,Z (units of radius)
          */
      static void convertGeocentricToCartesian(const Triple& llr,
                                               Triple& xyz)
         noexcept;


         /** Fundamental routine to convert geocentric to geodetic
          * @param[in] llr geocentric lat(deg N),lon(deg E),radius (meters)
          * @param[out] geodeticllh geodetic latitude (deg N),
          *            longitude (deg E), and height above ellipsoid (meters)
          * @param[in] A Earth semi-major axis
          * @param[in] eccSq square of Earth eccentricity
          */
      static void convertGeocentricToGeodetic(const Triple& llr,
                                              Triple& geodeticllh,
                                              const double A,
                                              const double eccSq)
         noexcept;

         /** Fundamental routine to convert geodetic to geocentric
          * @param[in] geodeticllh geodetic latitude (deg N),
          *            longitude (deg E), and height above ellipsoid (meters)
          * @param[out] llr geocentric lat (deg N),lon (deg E),radius (meters)
          * @param[in] A Earth semi-major axis
          * @param[in] eccSq square of Earth eccentricity
          */
      static void convertGeodeticToGeocentric(const Triple& geodeticllh,
                                              Triple& llr,
                                              const double A,
                                              const double eccSq)
         noexcept;

         // ----------- Part 11: operator<< and other useful functions --------
         //
         /**
          * Stream output for Position objects.
          * @param s stream to append formatted Position to.
          * @param p Position to append to stream \c s.
          * @return reference to \c s.
          */
      friend std::ostream& operator<<(std::ostream& s,
                                      const Position& p);

         /**
          * Compute the range in meters between two Positions.
          * Input Positions are not modified.
          * @param A,B Positions between which to find the range
          * @return the range (in meters)
          * @throw GeometryException if ellipsoid values differ.
          *        or if transformTo(Cartesian) fails
          */
      friend double range(const Position& A,
                          const Position& B);

         /**
          * Compute the radius of the ellipsoidal Earth, given the
          * geodetic latitude.
          * @param geolat geodetic latitude in degrees
          * @return the Earth radius (in meters)
          */
      static double radiusEarth(const double geolat,
                                const double A,
                                const double eccSq)
         noexcept;

         /**
          * A member function that calls the non-member radiusEarth() for
          * this Position.
          * @return the Earth radius (in meters)
          */
      double radiusEarth() const
         noexcept
      {
         Position p(*this);
         p.transformTo(Position::Geodetic);
         return Position::radiusEarth(p.theArray[0], p.AEarth, p.eccSquared);
      }

         /**
          * A member function that computes the elevation of the input
          * (Target) position as seen from this Position.
          * @param Target the Position which is observed to have the
          *        computed elevation, as seen from this Position.
          * @return the elevation in degrees
          * @throw GeometryException
          */
      double elevation(const Position& Target) const;

         /**
          * A member function that computes the elevation of the input
          * (Target) position as seen from this Position, using a Geodetic
          * (ellipsoidal) system.
          * @param Target the Position which is observed to have the
          *        computed elevation, as seen from this Position.
          * @return the elevation in degrees
          * @throw GeometryException
          */
      double elevationGeodetic(const Position& Target) const;

         /**
          * A member function that computes the azimuth of the input
          * (Target) position as seen from this Position.
          * @param Target the Position which is observed to have the
          *        computed azimuth, as seen from this Position.
          * @return the azimuth in degrees
          * @throw GeometryException
          */
      double azimuth(const Position& Target) const;

         /**
          * A member function that computes the azimuth of the input
          * (Target) position as seen from this Position, using a Geodetic
          * (ellipsoidal) system.
          * @param Target the Position which is observed to have the
          *        computed azimuth, as seen from this Position.
          * @return the azimuth in degrees
          * @throw GeometryException
          */
      double azimuthGeodetic(const Position& Target) const;

         /**
          * A member function that computes the position at which a
          * signal, which is received at this Position and there is
          * observed at the (input) azimuth and elevation angles,
          * crosses a model ionosphere that is taken to be a thin
          * shell at constant (input) height.
          *
          * This function will not transform this Position, and it
          * will return a Position in the same system; the algorithm
          * itself is done in the geocentric coordinate system.
          * @param elev elevation angle in degrees of the signal at reception
          * @param azim azimuth angle in degrees of the signal at reception
          * @param ionoht height of the ionosphere, in meters
          * @return Position IPP the position of the ionospheric pierce point,
          *     in the same coordinate system as *this; *this is not modified.
          */
      Position getIonosphericPiercePoint(const double elev,
                                         const double azim,
                                         const double ionoht) const
         noexcept;

         /**
          * A member function that computes the radius of curvature of the
          * meridian (Rm) corresponding to this Position.
          * @return radius of curvature of the meridian (in meters)
          */
      double getCurvMeridian() const
         noexcept;

         /**
          * A member function that computes the radius of curvature in the
          * prime vertical (Rn) corresponding to this Position.
          * @return radius of curvature in the prime vertical (in meters)
          */
      double getCurvPrimeVertical() const
         noexcept;

         /** Compute the zenith angle &zeta; from this position to
          * another.  With the center of the earth C, this position
          * being P1, and the second position being P2, This is the
          * angle between CP1 and P1P2.
          * This position (P1) is expected to be a surface observer,
          * while the target position (P2) is expected to be that of a
          * satellite in orbit.
          * @pre In order to correctly compute the radius from the
          *   center of the earth, the ellipsoid model for both this
          *   object and target should be set via setEllipsoidModel
          *   prior to calling this method.  Specifically, if one is
          *   using this method in the context of Galileo ionospheric
          *   modeling, one should have set the ellipsoid to an
          *   instance of GalileoIonoEllipsoid.
          * @note This method will create a geodetic version of the
          *   two objects if they are not already in Geodetic
          *   coordinates.
          * @param[in] target The orbital satellite position.
          * @param[out] delta The angle between CP1 and CP2.
          * @return the zenith angle between this and target.
          */
      Angle getZenithAngle(const Position& target, AngleReduced& delta) const;

         /** Compute the zenith angle &zeta; between
          * P1=(phi1,lambda1,r1) and P2=(phi2,lambda2,r2).  With the
          * center of Earth C, this is the angle between CP1 and P1P2.
          * This position (P1) is expected to be a surface observer,
          * while the target position (P2) is expected to be that of a
          * satellite in orbit.
          * @param[in] phi1 The geodetic latitude of the surface observer.
          * @param[in] lambda1 The longitude of the surface observer.
          * @param[in] phi2 The geodetic latitude of the target satellite.
          * @param[in] lambda2 The longitude of the target satellite.
          * @param[in] r1 The distance from Earth center of the
          *   surface observer (units must be consistent with r2).
          * @param[in] r2 The distance from Earth center of the
          *   surface observer (units must be consistent with r1).
          * @param[out] delta The computed delta angle, being the
          *   Earth angle on the great circle connecting receiver
          *   antenna P1 and the satellite P2.  This is used to
          *   compute the zenith angle, but is also used to compute
          *   the ray perigee, so it is output as an extra variable
          *   here.
          * @return the zenith angle between this and target.
          */
      static Angle getZenithAngle(const Angle& phi1, const Angle& lambda1,
                                  const Angle& phi2, const Angle& lambda2,
                                  double r1, double r2,
                                  AngleReduced& delta);

         /** Compute the ray-perigee position for the ray between this
          * position and another.
          * This position (P1) is expected to be a surface observer,
          * while the target position (P2) is expected to be that of a
          * satellite in orbit.
          * @pre In order to correctly compute the radius from the
          *   center of the earth, the ellipsoid model for both this
          *   object and target should be set via setEllipsoidModel
          *   prior to calling this method.  Specifically, if one is
          *   using this method in the context of Galileo ionospheric
          *   modeling, one should have set the ellipsoid to an
          *   instance of GalileoIonoEllipsoid.
          * @note This method will create a geodetic version of the
          *   two objects if they are not already in Geodetic
          *   coordinates.
          * @param[in] target The orbital satellite position.
          * @return the ray-perigee geodetic position on the ellipsoid surface.
          */
      Position getRayPerigee(const Position& target) const;

         /** Compute the coordinates at a given distance along a path
          * between two positions.
          * @param[in] dist The distance in METERS along the path
          *   between this and target.
          * @param[in] target The target position of the ray (e.g. ray
          *   perigee coordinates).
          * @return The position along the ray.
          */
      Position getRayPosition(double dist, const Position& target) const;

         /** This is a bit of a kludge to deal with the fact that
          * Position doesn't store the actual ellipsoid model but
          * rather a couple of terms from it.  It works by copying
          * those terms from another object into this one.
          * @todo Modify Position to store a
          *   shared_ptr<EllipsoidModel> instead of AEarth, eccSquared
          *   etc.
          * @param[in] src The Position object from which to copy
          *   ellipsoid model parameters.
          * @post AEarth=src.AEarth and eccSquared=src.eccSquared. */
      void copyEllipsoidModelFrom(const Position& src)
      {
         AEarth = src.AEarth;
         eccSquared = src.eccSquared;
      }

         /** Return the Earth semi-major axis (in meters) currently
          * used for coordinate conversion */
      double getAEarth() const
      { return AEarth; }

         // ----------- Part 12: private functions and member data ------------
         //
   private:

         /** Initialization function, used by the constructors.
          * @param a coordinate [ X(m), or latitude (degrees N) ]
          * @param b coordinate [ Y(m), or longitude (degrees E) ]
          * @param c coordinate [ Z, height above ellipsoid or radius, in m ]
          * @param s CoordinateSystem, defaults to Cartesian
          * @param ell pointer to a EllipsoidModel, default NULL (WGS84)
          * @throw GeometryException on invalid input.
          */
      void initialize(const double a,
                      const double b,
                      const double c,
                      CoordinateSystem s = Cartesian,
                      const EllipsoidModel *ell = nullptr,
                      ReferenceFrame frame = ReferenceFrame::Unknown);

         /* Values of the coordinates, defined for each system as follows;
          *    Cartesian  : X,Y,Z in meters
          *    Geocentric : Latitude(degrees N), Longitude(degrees E),
          *                    Radius (meters)
          *    Geodetic   : Latitude(degrees N), Longitude(degrees E),
          *                    Height above ellipsoid (meters)
          *    Spherical  : theta (degrees) - angle from the z axis
          *                 phi (degrees) - angle in xy plane from x axis
          *                    toward y axis (same as longitude)
          *                 radius (meters?) - distance from origin
          */
         // use std::valarray<double> theArray;  -- inherit from Triple

         /// semi-major axis of Earth (meters)
      double AEarth;

         /// square of ellipsoid eccentricity
      double eccSquared;

         /// see #CoordinateSystem
      CoordinateSystem system;

         /// tolerance used in comparisons
      double tolerance;

      ReferenceFrame refFrame;

   };   // end class Position

      //@}

}  // namespace gnsstk

#endif   // GNSSTK_POSITION_HPP
