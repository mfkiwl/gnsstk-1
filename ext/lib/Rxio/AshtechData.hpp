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
 * @file AshtechData.hpp
 * gnsstk::AshtechData - base cass for Ashtech formatted data.
 */

#ifndef ASHTECHDATA_HPP
#define ASHTECHDATA_HPP

#include <map>
#include <iostream>

#include "gnsstk_export.h"
#include "Exception.hpp"
#include "StringUtils.hpp"
#include "BinUtils.hpp"

#include "FFData.hpp"
#include "FFStream.hpp"
#include "DataStatus.hpp"

namespace gnsstk
{

   class AshtechStream;

   class AshtechData : public FFData, public CRCDataStatus
   {
   public:
      AshtechData() noexcept : ascii(false), checksum(0) {}
      // Returns whether or not this AshtechData is valid.
      bool isValid() const {return good();}

      //! This class is "data" so this function always returns "true".
      virtual bool isData() const {return true;}

      /** Encode this object to a string.
       * @return this object as a string.
       */
      virtual std::string encode() const noexcept { return std::string(); }

      /** Decode this object from a string.
       * @param str the string to read from.
       */
      virtual void decode(const std::string& str)
      {std::cout<<"AshtechData::decode()"<<std::endl;}

      /// Simple accessors for various static thangs.
      virtual std::string getName() const {return "hdr";}

      /// Returns true when the provided id is valid for this message
      virtual bool checkId(std::string hdrId) const {return false;}

      /** Dump some debugging information to the given ostream.
       * @param out the ostream to write to
       */
      void dump(std::ostream& out) const noexcept;

      GNSSTK_EXPORT static int debugLevel;
      GNSSTK_EXPORT static bool hexDump;

      std::string id;
      bool ascii;

      // Note that this doesn't appear in the RID message. Also there is reason
      // to believe it isn't trustworthy except in the PBN and MPC messages.
      uint16_t checksum;

      GNSSTK_EXPORT static const std::string preamble;
      GNSSTK_EXPORT static const std::string trailer;

   protected:
         /**
          * @throw FFStreamError
          */
      virtual void reallyPutRecord(FFStream& ffs) const
      {
         gnsstk::FFStreamError e("Writing of AshtecData is not supported.");
         GNSSTK_THROW(e);
      }

         /**
          * @throw std::exception
          * @throw FFStreamError
          * @throw EndOfFile
          */
      virtual void reallyGetRecord(FFStream& ffs);

         /**
          * @throw FFStreamError
          * @throw EndOfFile
          */
      virtual void readHeader(AshtechStream& stream);

         /**
          * @throw FFStreamError
          * @throw EndOfFile
          */
      virtual void readBody(AshtechStream& stream);

   }; // class AshtechData
} // namespace gnsstk

#endif
