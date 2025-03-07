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

/// @file Epoch.hpp

#ifndef GNSSTK_EPOCH_HPP
#define GNSSTK_EPOCH_HPP

#include "MathBase.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"

#include "GPSZcount.hpp"
#include "CommonTime.hpp"
#include "TimeTag.hpp"
#include "SystemTime.hpp"

#include "JulianDate.hpp"
#include "MJD.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "GPSWeekZcount.hpp"
#include "GPSWeekSecond.hpp"
#include "BDSWeekSecond.hpp"
#include "GALWeekSecond.hpp"
#include "QZSWeekSecond.hpp"

namespace gnsstk
{
      /** @defgroup TimeHandling Time Representation */
      //@{

      /**
       * @todo Fix these comments.
       *
       * A time representation class for all common time formats, including
       * GPS. There is a seamless conversion between dates, times, and both,
       * as well as the ability to input and output the stored day-time in
       * formatted strings (printf() and scanf()).
       *
       * Internally, the representation of day and time uses a CommonTime
       * object. @see CommonTime
       *
       * In addition, the representation includes a tolerance value which is
       * used in CommonTime comparisons. It defaults to the value of the static
       * gnsstk::Epoch::EPOCH_TOLERANCE, but this can be modified with the
       * static method setEpochTolerance().  Several different default
       * tolerances have been defined and are in the Epoch-Specific
       * Definitions section. The tolerance can also be changed on a per-object
       * basis with the setTolerance() member function.  All comparisons are
       * done using the tolerance as a range for the comparison.
       * So, for example, operator==() returns true if the times are within
       * 'tolerance' seconds. Once set for each object, the tolerance is
       * appropriately "carried forward" to new objects through the copy
       * operator (operator=), the copy constructor, and elsewhere.
       *
       * The internal representation is manipulated using four fundamental
       * routines, two that convert between 'jday' (the integer
       * representation of JD) and calendar date: year/month/day-of-month,
       * and two that convert between seconds-of-day and hour/minute/second.
       * These routines can be found in the TimeConverters.hpp file.
       * The range of validity of the jday--calendar routines is approximately
       * 4317 B.C. to 4317 A.D.; these limits are incorporated into constants
       * Epoch::BEGINNING_OF_TIME and Epoch::END_OF_TIME.
       *
       * * All Epoch objects that lie outside these limits are disallowed. *
       *
       * This representation separates day and time-of-day cleanly.
       * Because day and time are logically separated, it is possible to use
       * Epoch for day only, or for time only. Thus, for example, one
       * could instantiate a Epoch object and only manipulate the date,
       * without reference to time-of-day; or vice-versa. [However in this
       * regard note that the default constructor for Epoch sets the
       * data, not to zero, but to the current (system) time.]
       *
       * When constructing Epoch objects from GPS time values -- such as
       * GPS week and seconds of weeks, or GPS week and z count -- there
       * may be ambiguity associated with the GPS week. Many receivers
       * and receiver processing software store the GPS week as it appears
       * in the NAV message, as a 10-bit number. This leads to a 1024 week
       * ambiguity when 10-bit GPS weeks are used to specify a Epoch.
       * In general, Epoch uses the system time to disambiguate which
       * 1024 week period to use. This is a good assumption except when
       * processing binary data from before GPS week rollover, which
       * occured on August 22, 1999.
       *
       */
   class Epoch
   {
   public:
         /**
          * @ingroup exceptionclass
          * Epoch basic exception class.
          * @todo Do we need this, or can we get by with InvaildRequest
          * and/or InvalidParameter?
          */
      NEW_EXCEPTION_CLASS(EpochException, gnsstk::Exception);

         /**
          * @ingroup exceptionclass
          * Epoch formatting ("printing") error exception class.
          */
      NEW_EXCEPTION_CLASS(FormatException, gnsstk::Exception);

         /**
          * @name Epoch-Specific Definitions
          * All of these tolerances are 1/2 of the tolerance they specify.
          * So one nsec tolerance is actually 1/2 an ns added to the time
          * in units of days.
          */
         //@{
         /// One nanosecond tolerance.
      static const double ONE_NSEC_TOLERANCE;
         /// One microsecond tolerance.
      static const double ONE_USEC_TOLERANCE;
         /// One millisecond tolerance.
      static const double ONE_MSEC_TOLERANCE;
         /// One second tolerance.
      static const double ONE_SEC_TOLERANCE;
         /// One minute tolerance.
      static const double ONE_MIN_TOLERANCE;
         /// One hour tolerance.
      static const double ONE_HOUR_TOLERANCE;

         /// Default tolerance for time equality in days.
      static double EPOCH_TOLERANCE;

         /// Earliest representable Epoch.
      static const Epoch BEGINNING_OF_TIME;
         /// Latest Representable Epoch.
      static const Epoch END_OF_TIME;

         /// This is how an Epoch is printed by default.
      static std::string PRINT_FORMAT;
         //@}

         /// @name Tolerance Functions
         //@{
         /// Changes the EPOCH_TOLERANCE for all Epoch objects
      static double setEpochTolerance(double tol)
         noexcept
      { return EPOCH_TOLERANCE = tol; }

         /// Returns the current EPOCH_TOLERANCE.
      static double getEpochTolerance()
         noexcept
      { return EPOCH_TOLERANCE; }

         /**
          * Sets the tolerance for output and comparisons on this object only.
          * See the constants in this file (e.g. ONE_NSEC_TOLERANCE)
          * for some easy to use tolerance values.
          * @param tol Tolerance in days to be used by comparison operators.
          * @sa Epoch-Specific Definitions
          */
      Epoch& setTolerance(double tol)
         noexcept;

         /**
          * Return the tolerance value currently in use by this object.
          * @return the current tolerance value (in seconds, of course)
          */
      double getTolerance() const noexcept
      { return tolerance; }
         //@}

         /// @name Constructors and Destructor
         //@{
         /**
          * Default Constructor.
          * Initializes to current system time or to the given TimeTag.
          * TimeTag-covered constructors:
          * year, month, day, hour, minute, second (CivilTime)
          * long double mjd (MJD)
          * double mjd (MJD)
          * year, doy, sod (YDSTime)
          * Unix struct timeval (UnixTime)
          * GPS full week and second (GPSWeekSecond)
          * @throw EpochException
          */
      Epoch(const TimeTag& tt = SystemTime());

         /**
          * CommonTime Constructor.
          * Set the time using the given CommonTime object.
          */
      Epoch(const CommonTime& ct)
         noexcept;

         /**
          * TimeTag + Year Constructor.
          * Set the current time using the given year as a hint.
          * For example, when one only knows the 10-bit GPS week, one could
          * could use a "hint" year to figure out which Epoch the week was in.
          * TimeTag + year -covered constructors:
          * gps 10-bit week and second and year (GPSEpochWeekSecond + year)
          * gps week and zcount and year (GPSWeekZcount + year)
          * @throw Epochexception
          */
      Epoch(const WeekSecond& tt,
            short year);

         /** GPSZcount Constructor.
          * Set the current time using the given GPSZcount.
          */
      Epoch(const GPSZcount& gzc)
         noexcept;

         // Other Constructors:
         // gps 29-bit zcount w/ epoch determined by current system time
         //     (GPSZcount29 + SystemTime)

         /// Destructor.
      ~Epoch()
         noexcept
      {}
         //@}

         /// @name Assignment and Copy
         //@{
         /// Copy constructor.
      Epoch(const Epoch &right)
         noexcept;

         /// Assignment operator.
      Epoch& operator=(const Epoch& right)
         noexcept;
         //@}

         /// @name Arithmetic
         //@{
         /**
          * Epoch difference function.
          * @param right Epoch to subtract from this one.
          * @return difference in seconds.
          */
      double operator-(const Epoch& right) const
         noexcept;

         /**
          * Add seconds to this time.
          * @param sec Number of seconds to increase this time by.
          * @return The new time incremented by \c sec.
          * @throw EpochException
          */
      Epoch operator+(double sec) const;

         /**
          * Subtract seconds from this time.
          * @param sec Number of seconds to decrease this time by.
          * @return The new time decremented by \c sec.
          * @throw EpochException
          */
      Epoch operator-(double sec) const;

         /**
          * Add seconds to this time.
          * @param sec Number of seconds to increase this time by.
          * @throw EpochException on over/under-flow
          */
      Epoch& operator+=(double sec);

         /**
          * Subtract seconds from this time.
          * @param sec Number of seconds to decrease this time by.
          * @throw EpochException on over/under-flow
          */
      Epoch& operator-=(double sec);

         /**
          * Add (double) seconds to this time.
          * @param seconds Number of seconds to increase this time by.
          * @throw EpochException on over/under-flow
          */
      Epoch& addSeconds(double seconds);

         /**
          * Add (integer) seconds to this time.
          * @param seconds Number of seconds to increase this time by.
          * @throw EpochException on over/under-flow
          */
      Epoch& addSeconds(long seconds);

         /**
          * Add (integer) milliseconds to this time.
          * @param msec Number of milliseconds to increase this time by.
          * @throw EpochException on over/under-flow
          */
      Epoch& addMilliSeconds(long msec);

         /**
          * Add (integer) microseconds to this time.
          * @param usec Number of microseconds to increase this time by.
          * @throw EpochException on over/under-flow
          */
      Epoch& addMicroSeconds(long usec);
         //@}

         /// @name Comparisons
         //@{
      bool operator==(const Epoch &right) const
         noexcept;
      bool operator!=(const Epoch &right) const
         noexcept;
      bool operator<(const Epoch &right) const
         noexcept;
      bool operator>(const Epoch &right) const
         noexcept;
      bool operator<=(const Epoch &right) const
         noexcept;
      bool operator>=(const Epoch &right) const
         noexcept;
         //@}

         /// @name Accessor Methods (get and set)
         //@{
         /// Get the specified TimeTag.
         /// This function converts the internal store into the requested
         /// TimeTag type.
         /// @throw EpochException
      template <class TimeTagType>
      TimeTagType get() const;

         /// Get Julian Date JD
         /// @warning For some compilers, this result may have diminished
         ///  accuracy.
         /// @throw EpochException
      inline long double JD() const;

         /// Get Modified Julian Date MJD
         /// @warning For some compilers, this result may have diminished
         ///  accuracy.
         /// @throw EpochException
      inline long double MJD() const;

         /// Get year.
         /// @throw EpochException
      inline short year() const;

         /// Get month of year.
         /// @throw EpochException
      inline short month() const;

         /// Get day of month.
         /// @throw EpochException
      inline short day() const;

         /// Get day of week
         /// @throw EpochException
      inline short dow() const;

         /// Get hour of day.
         /// @throw EpochException
      inline short hour() const;

         /// Get minutes of hour.
         /// @throw EpochException
      inline short minute() const;

         /// Get seconds of minute.
         /// @throw EpochException
      inline double second() const;

         /// Get seconds of day.
         /// @throw EpochException
      inline double sod() const;

         /// Get 10-bit GPS week
         /// @throw EpochException
      inline short GPSModWeek() const;

         /// Get 10-bit GPS week, deprecated, use GPSModWeek()
         /// @throw EpochException
      inline short GPSweek10() const;

         /// Get normal (19 bit) zcount.
         /// @throw EpochException
      inline long GPSzcount() const;

         /// Same as GPSzcount() but without rounding to nearest zcount.
         /// @throw EpochException
      inline long GPSzcountFloor() const;

         /**
          * Get time as 32 bit Z count.
          * The 13 MSBs are week modulo 1024, 19 LSBs are seconds of
          * week in Zcounts.
          * @throw EpochException
          */
      inline unsigned long GPSzcount32() const;

         /// Same as fullZcount() but without rounding to nearest zcount.
         /// @throw EpochException
      inline unsigned long GPSzcount32Floor() const;

         /// Get GPS second of week.
         /// @throw EpochException
      inline double GPSsow() const;

         /// Get full (>10 bits) week
         /// @throw EpochException
      inline short GPSweek() const;

         /// Get BDS second of week.
         /// @throw EpochException
      inline double BDSsow() const;

         /// Get full BDS week
         /// @throw EpochException
      inline short BDSweek() const;

         /// Get mod (short) BDS week
         /// @throw EpochException
      inline short BDSModWeek() const;

         /// Get QZS second of week.
         /// @throw EpochException
      inline double QZSsow() const;

         /// Get full QZS week
         /// @throw EpochException
      inline short QZSweek() const;

         /// Get mod (short) QZS week
         /// @throw EpochException
      inline short QZSModWeek() const;

         /// Get GAL second of week.
         /// @throw EpochException
      inline double GALsow() const;

         /// Get full GAL week
         /// @throw EpochException
      inline short GALweek() const;

         /// Get mod (short) GAL week
         /// @throw EpochException
      inline short GALModWeek() const;

         /// Get day of year.
         /// @throw EpochException
      inline short doy() const;

         /// Get object time in UNIX timeval structure.
         /// @throw EpochException
      inline struct timeval unixTime() const;

         /// Convert this object to a GPSZcount object.
         /// @throw EpochException
      operator GPSZcount() const;

         /// Convert this object to a CommonTime object.
      operator CommonTime() const
         noexcept;

         /// @todo Could we get away with just CommonTime sets? The TimeTags
         /// can convert themselves to CommonTime objects.  That's what we
         /// do internally anyway...

         /** Set the object using a TimeTag object.
          * @param tt the TimeTag to which to set this object (Defaults
          * to SystemTime()).
          * @return a reference to this object.
          * @throw EpochException
          */
      Epoch& set(const TimeTag& tt = SystemTime());

         /** Set the object using a TimeTag and a year as a hint.
          * @param tt the TimeTag to which to set this object.
          * @param year the "hint" year
          * @return a reference to this object.
          * @throw EpochException
          */
      Epoch& set(const WeekSecond& tt,
                 short year);

         /**
          * Set the object using the give CommonTime.
          * @param c the CommonTime object to set to
          * @return a reference to this object.
          */
      Epoch& set(const CommonTime& c)
         noexcept;

         /**
          * Set the object using a GPSZcount object.
          * @throw EpochException
          */
      Epoch& set(const GPSZcount& z);

         /**
          * Set the object's time using a CommonTime object. This operation
          * leaves the object's date unchanged.
          * @note as TimeTags can be implicitly converted to CommonTime
          * objects, this method works for them as well.
          * @throw EpochException
          */
      Epoch& setTime(const CommonTime& ct);

         /**
          * Set the object's date using a CommonTime object. This operation
          * leaves the object's time unchanged.
          * @note as TimeTags can be implicitly converted to CommonTime
          * objects, this method works for them as well.
          * @throw EpochException
          */
      Epoch& setDate(const CommonTime& ct);

         /**
          * Set the object time to the current local time.
          * @todo What is this?
          * @throw EpochException
          */
      Epoch& setLocalTime();

         // other sets:
         // ymdhms
         // week and sow (if week is 10-bit, set epoch from system time)
         // week and zcount (if week is 10-bit, set epoch from system time)
         // week, zcount, year (if week is 10-bit, set epoch from given year)
         // week, sow, year  (if week is 10-bit, set epoch from given year)
         // gps 29-bit zcount (epoch determined by system time)
         // gps full week and sow
         // gps full week and zcount
         // year, doy, sod
         // long double mjd
         // double mjd
         // Unix struct timeval
         // ANSI time
         // YMD, (year/doy) w/ time unchanged
         // HMS, sod w/ date unchanged
         //@}

         /// @name Printing and Scanning Methods
         //@{
         /// @todo Someone figure out how to make the table below show up
         /// nice in doxygen.
         /**
          * Similar to scanf, this function takes a string and a
          * format describing string in order to read in
          * values.  The parameters it can take are listed below and
          * described above with the printf() function.
          *
          * The specification must resolve to a day at a minimum
          * level. The following table lists combinations that give valid
          * times. Anything more or other combinations will give
          * unknown (read as: "bad") results so don't try it.  Anything
          * less will throw an exception.  If nothing changes the time
          * of day, it will default to midnight.  Also, the year
          * defaults to the current year if a year isn't specified
          * or can't be determined.
          *
          * @code
          *  1 of...           and 1 of....         optional...
          *  %C
          *  %G                %w %g %Z                %Y %y %E    (GPS)
          *  %e                %w %g                   %Y %y %R    (BDS)
          *  %l                %w %g                   %Y %y %T    (GAL)
          *  %i                %w %g                   %Y %y %V    (QZS)
          *  %F                %w %g %Z                            (GPS)
          *  %D                %w %g                               (BDS)
          *  %L                %w %g                               (GAL)
          *  %I                %w %g                               (QZS)
          *  %m %B %b          %a %A %d             %Y %y %H %M %S
          *  %Q
          *  %j                                      %Y %y %s
          * @endcode
          *
          * So
          * @code
          * time.setToString("Aug 1, 2000 20:20:20", "%b %d, %Y %H:%M:%S")
          * @endcode
          * works but
          * @code
          * time.setToString("Aug 2000", "%b %Y")
          * @endcode
          * doesn't work (incomplete specification because it doesn't specify
          * a day).
          *
          * Don't worry about counting whitespace - this function will
          * take care of that.  Just make sure that the extra stuff in
          * the format string (ie '.' ',') are in the same relative
          * location as they are in the actual string.  (see in the
          * example above))
          *
          * @param str string to get date/time from.
          * @param fmt format to use to parse \c str.
          * @throw EpochException if \c fmt is an incomplete specification
          * @throw FormatException if unable to scan \c str.
          * @throw StringException if an error occurs manipulating the
          * \c str or \c fmt strings.
          * @return a reference to this object.
          */
      Epoch& scanf(const std::string& str,
                   const std::string& fmt);

         // if you can see this, ignore the \'s below, as they are for
         // the nasty html-ifying of doxygen.  Browsers try and
         // interpret the % and they get all messed up.
         /**
          * Format this time into a string.
          *
          * @note
          * Whenever a format is added or removed from the Epoch
          * class, it more than likely should also be added or removed
          * from the FileSpec class.  Additionally, the format
          * character must not conflict with any of the existing
          * format characters in Epoch or FileSpec.
          *
          * Generate and return a string containing a formatted
          * date, formatted by the specification \c fmt.
          *
          * \li \%Y   year()
          * \li \%y   year() % 100
          * \li \%m   month()
          * \li \%d   day()
          * \li \%H   hour()
          * \li \%M   minute()
          * \li \%S   (short)second()
          * \li \%f   second()
          * \li \%E   GPS getEpoch()
          * \li \%G   GPS getModWeek()
          * \li \%F   GPS getWeek()
          * \li \%g   GPS/BDS/GAL/QZS sow() or getSOW()
          * \li \%Z   GPSzcount()
          * \li \%z   GPSzcountFloor()
          * \li \%R   BDS getEpoch()
          * \li \%D   BDS getWeek()
          * \li \%e   BDS getModWeek()
          * \li \%T   GAL getEpoch()
          * \li \%L   GAL getWeek()
          * \li \%l   GAL getModWeek()
          * \li \%V   QZS getEpoch()
          * \li \%I   QZS getWeek()
          * \li \%i   QZS getModWeek()
          * \li \%s   DOYsecond()
          * \li \%Q   MJDdate()
          * \li \%w   dayOfWeek() or GPSday()
          * \li \%b   MonthAbbrevName[month()]
          * \li \%B   MonthName[month()]
          * \li \%a   DayOfWeekAbbrevName[dayOfWeek()]
          * \li \%A   DayOfWeekName[dayOfWeek()]
          * \li \%j   DOYday() or DOY()
          * \li \%U   unixTime().tv_sec
          * \li \%u   unixTime().tv_usec
          * \li \%C   fullZcount()
          * \li \%c   fullZcountFloor()
          *
          * @warning See above note.
          *
          * @param fmt format to use for this time.
          * @return a string containing this time in the
          * representation specified by \c fmt.
          * @throw StringUtils::StringException
          */
      std::string printf(const std::string& fmt = PRINT_FORMAT) const;
         //@}

   private:
         /// @name Private Methods and Data Members
         //@{
         /// This is the core of the Epoch.
         /// @see CommonTime.hpp
      CommonTime core;

         /// double tolerance used in comparisons (seconds)
      double tolerance;

         //@}
   };   // end class Epoch

      /**
       * Stream output for Epoch objects.  Typically used for debugging.
       * @param s stream to append formatted Epoch to.
       * @param t Epoch to append to stream \c s.
       * @return reference to \c s.
       */
   std::ostream& operator<<( std::ostream& s,
                             const Epoch& t );
      //@}


   template <class TimeTagType>
   TimeTagType Epoch::get() const
   {
      try { return TimeTagType(core); }
      catch( Exception& e)
      {
         EpochException ee(e);
         GNSSTK_THROW(ee);
      }
   }

      /// Get Julian Date JD
      /// @warning For some compilers, this result may have diminished
      /// accuracy.
   long double Epoch::JD() const
   {
      return get<JulianDate>().jd;
   }

      /// Get Modified Julian Date MJD
      /// @warning For some compilers, this result may have diminished
      /// accuracy.
   long double Epoch::MJD() const
   {
      return get<gnsstk::MJD>().mjd;    // gnsstk to distinguish from Epoch::MJD
   }

      /// Get year.
   short Epoch::year() const
   {
      return static_cast<short>(get<CivilTime>().year);
   }

      /// Get month of year.
   short Epoch::month() const
   {
      return static_cast<short>(get<CivilTime>().month);
   }

      /// Get day of month.
   short Epoch::day() const
   {
      return static_cast<short>(get<CivilTime>().day);
   }

      /// Get day of week
   short Epoch::dow() const
   {
      return (((static_cast<long>(JD()) % 7) + 1) % 7) ;
   }

      /// Get hour of day.
   short Epoch::hour() const
   {
      return static_cast<short>(get<CivilTime>().hour);
   }

      /// Get minutes of hour.
   short Epoch::minute() const
   {
      return static_cast<short>(get<CivilTime>().minute);
   }

      /// Get seconds of minute.
   double Epoch::second() const
   {
      return get<CivilTime>().second;
   }

      /// Get seconds of day.
   double Epoch::sod() const
   {
      return get<YDSTime>().sod;
   }

      /// Get 10-bit GPS week. Deprecated, used GPSModWeek()
   short Epoch::GPSweek10() const
   {
      return GPSModWeek();
   }

      /// Get 10-bit GPS week
   short Epoch::GPSModWeek() const
   {
      return static_cast<short>(get<GPSWeekSecond>().getModWeek());
   }

      /// Get normal (19 bit) zcount.
   long Epoch::GPSzcount() const
   {
      return get<GPSWeekZcount>().zcount;
   }

      /// Same as GPSzcount() but without rounding to nearest zcount.
   long Epoch::GPSzcountFloor() const
   {
      Epoch e = *this + .75; // add half a zcount
      return e.get<GPSWeekZcount>().zcount;
   }

      /// Get time as 32 bit Z count.
      /// The 13 MSBs are week modulo 1024, 19 LSBs are seconds of
      /// week in Zcounts.
   unsigned long Epoch::GPSzcount32() const
   {
      return get<GPSWeekZcount>().getZcount32();
   }

      /// Same as fullZcount() but without rounding to nearest zcount.
   unsigned long Epoch::GPSzcount32Floor() const
   {
      Epoch e = *this + .75; // add half a zcount
      return e.get<GPSWeekZcount>().getZcount32();
   }

      /// Get GPS second of week.
   double Epoch::GPSsow() const
   {
      return get<GPSWeekSecond>().sow;
   }

      /// Get full (>10 bits) week
   short Epoch::GPSweek() const
   {
      return static_cast<short>(get<GPSWeekSecond>().week);
   }

      /// Get BDS second of week.
   double Epoch::BDSsow() const
   {
      return get<BDSWeekSecond>().sow;
   }

      /// Get full BDS week
   short Epoch::BDSweek() const
   {
      return static_cast<short>(get<BDSWeekSecond>().week);
   }

      /// Get mod (short) BDS week
   short Epoch::BDSModWeek() const
   {
      return static_cast<short>(get<BDSWeekSecond>().getModWeek());
   }

      /// Get QZS second of week.
   double Epoch::QZSsow() const
   {
      return get<QZSWeekSecond>().sow;
   }

      /// Get full QZS week
   short Epoch::QZSweek() const
   {
      return static_cast<short>(get<QZSWeekSecond>().week);
   }

      /// Get mod (short) QZS week
   short Epoch::QZSModWeek() const
   {
      return static_cast<short>(get<QZSWeekSecond>().getModWeek());
   }

      /// Get GAL second of week.
   double Epoch::GALsow() const
   {
      return get<GALWeekSecond>().sow;
   }

      /// Get full GAL week
   short Epoch::GALweek() const
   {
      return static_cast<short>(get<GALWeekSecond>().week);
   }

      /// Get mod (short) GAL week
   short Epoch::GALModWeek() const
   {
      return static_cast<short>(get<GALWeekSecond>().getModWeek());
   }

      /// Get day of year.
   short Epoch::doy() const
   {
      return static_cast<short>(get<YDSTime>().doy);
   }

      /// Get object time in UNIX timeval structure.
   struct timeval Epoch::unixTime() const
   {
      return get<UnixTime>().tv;
   }

}  // namespace gnsstk

#endif   // GNSSTK_EPOCH_HPP
