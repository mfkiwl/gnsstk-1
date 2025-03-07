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

#include "TimeTag.hpp"
#include "StringUtils.hpp"

namespace gnsstk
{
   void TimeTag::scanf( const std::string& str,
                        const std::string& fmt )
   {
      try
      {
            // Get the mapping of character (from fmt) to value (from str).
         IdToValue info;
         getInfo( str, fmt, info );

            // Attempt to set this object using the IdToValue object
         if( !setFromInfo( info ) )
         {
            gnsstk::InvalidRequest ir( "Incomplete time specification." );
            GNSSTK_THROW( ir );
         }
      }
      catch( gnsstk::StringUtils::StringException& se )
      {
         GNSSTK_RETHROW( se );
      }
   }

   void TimeTag::getInfo( const std::string& str,
                          const std::string& fmt,
                          IdToValue& info )
   {
         // Copy the arguments to strings we can modify.
      std::string f = fmt;
      std::string s = str;

      try
      {
         using namespace gnsstk::StringUtils;

            // Parse strings...  As we process each part, it's removed from both
            // strings so when we reach 0, we're done
         while( !s.empty() && !f.empty() )
         {
            // Remove everything in f and s up to the first % in f
            // (these parts of the strings must be identical or this will
            // break after it tries to remove it!)
            while ( !s.empty() &&
                    !f.empty() &&
                    ( f[0] != '%' ) )
            {
               // remove that character now and other whitespace
               s.erase(0,1);
               f.erase(0,1);
            }

            // check just in case we hit the end of either string...
            if ( s.empty() || f.empty() )
               break;

            // lose the '%' in f...
            f.erase( 0, 1 );

            std::string::size_type fieldLength = std::string::npos;
            char delimiter = 0;

            if (false)
               std::cout << "--------------" << std::endl
                         << "f:\"" << f << "\"" << std::endl
                         << "s:\"" << s << "\"" << std::endl;

            if( !isalpha( f[0] ) )
            {
               // If the format string is %03f, get '3' as the field length.
               // This is where we have a specified field length so we should
               // not throw away any more characters
               fieldLength = asInt( f );

               // remove everything else up to the next character
               // (in "%03f", that would be 'f')
               while ( !f.empty() && !isalpha( f[0] ) )
                  f.erase( 0, 1 );

               if ( f.empty() )
                  break;
            }
            else
            {
               // When there is no field width specified, there must be a
               // delimiter for the field.
               if ( f.size() > 1 )
               {
                  if ( f[1] != '%' )
                  {
                     delimiter = f[1];
                     stripLeading(s);
                     fieldLength = s.find( delimiter, 0 );
                  }
                  else
                  {
                     // if the there is no delimiter character and the
                     // next field is another part of the time to
                     // parse, assume the length of this field is 1
                     fieldLength = 1;
                  }
               }
            }

            // Copy the next string to be removed.
            std::string value( s.substr( 0, fieldLength ) );

            // based on char at f[0], we know what to do...
            info[ f[0] ] = value;

               // remove the part of str that we processed
            stripLeading( s, value, 1 );

            // remove the character we processed from fmt
            f.erase( 0, 1 );

            // And remove the delimiter if one was used
            if (delimiter != 0)
            {
               f.erase(0,1);
               s.erase(0,1);
            }
         } // end of while( (s.size() > 0) && (f.size() > 0) )
      }
      catch( gnsstk::StringUtils::StringException& se )
      {
         GNSSTK_RETHROW( se );
      }
         // make sure the format string was fully processed.
      if (!f.empty())
      {
         gnsstk::StringUtils::StringException
            exc("Failed to process time string");
         GNSSTK_THROW(exc);
      }
   }


   bool TimeTag ::
   changeTimeSystem(TimeSystem timeSys, TimeSystemConverter* conv)
   {
         // We have to add an offset and the only way to do that is to
         // change to CommonTime and back.
      CommonTime ct = convertToCommonTime();
      bool rv = ct.changeTimeSystem(timeSys, conv);
      if (rv)
         convertFromCommonTime(ct);
      return rv;
   }


   bool TimeTag ::
   changeTimeSystem(TimeSystem timeSys)
   {
         // We have to add an offset and the only way to do that is to
         // change to CommonTime and back.
      CommonTime ct = convertToCommonTime();
      bool rv = ct.changeTimeSystem(timeSys);
      if (rv)
         convertFromCommonTime(ct);
      return rv;
   }


   void TimeTag ::
   checkTimeSystem(TimeSystem ts1, TimeSystem ts2)
   {
      if ((ts1 != TimeSystem::Any) &&
          (ts2 != TimeSystem::Any) &&
          (ts1 != ts2))
      {
         InvalidRequest exc("TimeTag objects not in same time system,"
                            " cannot be compared");
         GNSSTK_THROW(exc);
      }
   }

} // namespace

std::ostream& operator<<( std::ostream& s,
                          const gnsstk::TimeTag& t )
{
   s << t.printf( t.getDefaultFormat() );
   return s;
}
