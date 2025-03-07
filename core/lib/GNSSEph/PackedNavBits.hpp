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
 * @file PackedNavBits.hpp
 * Engineering units navigation message abstraction.
 */

#ifndef GNSSTK_PACKEDNAVBITS_HPP
#define GNSSTK_PACKEDNAVBITS_HPP

#include <bitset>
#include <memory>
#include <vector>
#include <cstddef>
#include "gnsstkplatform.h" //#include <stdint.h>
#include <string>

#include "ObsID.hpp"
#include "NavID.hpp"
#include "SatID.hpp"
#include "CommonTime.hpp"
#include "Exception.hpp"

namespace gnsstk
{
      /// @ingroup GNSSEph
      //@{

      // forward declaration
   class PackedNavBits;
      /// Managed pointer for passing PackedNavBits around.
   typedef std::shared_ptr<PackedNavBits> PackedNavBitsPtr;

   class PackedNavBits
   {
   public:
         /** Indicate whether parity/CRC/whatever checking has been
          * performed and whether that check passed or failed on this
          * subframe/message. */
      enum ParityStatus
      {
         psUnknown, ///< Parity/CRC check has not been performed.
         psPassed,  ///< Parity/CRC check was performed and passed.
         psFailed   ///< Parity/CRC check was performed and failed.
      };

         /// empty constructor
      PackedNavBits();

         /// explicit constructor
      PackedNavBits(const SatID& satSysArg,
                    const ObsID& obsIDArg,
                    const CommonTime& transmitTimeArg);

         /// explicit constructor
      PackedNavBits(const SatID& satSysArg,
                    const ObsID& obsIDArg,
                    const std::string rxString,
                    const CommonTime& transmitTimeArg);

         /// explicit constructor
      PackedNavBits(const SatID& satSysArg,
                    const ObsID& obsIDArg,
                    const NavID& navIDArg,
                    const std::string rxString,
                    const CommonTime& transmitTimeArg);

         /// explicit constructor
      PackedNavBits(const SatID& satSysArg,
                    const ObsID& obsIDArg,
                    const NavID& navIDArg,
                    const std::string rxString,
                    const CommonTime& transmitTimeArg,
                    unsigned numBits,
                    bool fillValue);

      PackedNavBits(const PackedNavBits& right);             // Copy constructor
         //PackedNavBits& operator=(const PackedNavBits& right); // Copy assignment

      PackedNavBits* clone() const;

      void setSatID(const SatID& satSysArg);
      void setObsID(const ObsID& obsIDArg);
      void setNavID(const NavID& navIDArg);
      void setRxID(const std::string rxString);
      void setTime(const CommonTime& transmitTimeArg);
      void clearBits();

         /* Returnst the satellite system ID for a particular SV */
      SatID getsatSys() const;

         /* Returns Observation type, Carrier, and Tracking Code */
      ObsID getobsID() const;

         /* Return navigation message ID */
      NavID getNavID() const;

         /* Returns string defining the receiver that collected the data.
            NOTE: This was a late addition to PackedNavBits and may not
            be present in all applications */
      std::string getRxID() const;

         /* Returns time of transmission from SV */
      CommonTime getTransmitTime() const;

         /* Returns the number of bits */
      size_t getNumBits() const;

         /* Output the contents of this class to the given stream. */
      void dump(std::ostream& s = std::cout) const noexcept;

         /***    UNPACKING FUNCTIONS *********************************/
         /* Unpack an unsigned long integer */
      unsigned long asUnsignedLong(const int startBit,
                                   const int numBits,
                                   const int scale ) const;

         /* Unpack a signed long integer */
      long asLong(const int startBit,
                  const int numBits,
                  const int scale ) const;

         /* Unpack an unsigned double */
      double asUnsignedDouble( const int startBit,
                               const int numBits,
                               const int power2) const;

         /* Unpack a signed double */
      double asSignedDouble( const int startBit,
                             const int numBits,
                             const int power2) const;

         /* Unpack a double with units of semicircles */
      double asDoubleSemiCircles( const int startBit,
                                  const int numBits,
                                  const int power2) const;

         /* Unpack a string */
      std::string asString(const int startBit,
                           const int numChars) const;

         // The following three methods were added to support
         // GLONASS sign/magnitude real values.
         //
         // Since GLONASS has no disjoint fields (at least not
         // up through ICD Edition 5.1) there are no methods
         // for unpacking disjoint-field sign/mag quantities.
         /* Unpack a sign/mag long */
      long asSignMagLong(const int startBit,
                         const int numBits,
                         const int scale) const;

         /* Unpack a sign/mag double */
      double asSignMagDouble( const int startBit,
                              const int numBits,
                              const int power2) const;

         /* Unpack a sign/mag double with units of semi-circles */
      double asSignMagDoubleSemiCircles( const int startBit,
                                         const int numBits,
                                         const int power2) const;

         /* Unpack mehthods that join multiple disjoint
            navigation message areas as a single field
            NOTE: startBit1 is associated with the most significant section
            startBit2 is associated with the least significant section
         */
         /* Unpack a split unsigned long integer */
      unsigned long asUnsignedLong(const unsigned startBits[],
                                   const unsigned numBits[],
                                   const unsigned len,
                                   const int scale ) const;

         /** Unpack an unsigned long integer split into two pieces.
          * @warning Be careful about what order you specify the parameters in.
          * @note This prototype obviates constructing an array before calling.
          * @param[in] startBit1 The 0-indexed first bit of the MSBs.
          * @param[in] numBits1 The number of MSBs.
          * @param[in] startBit2 The 0-indexed first bit of the LSBs.
          * @param[in] numBits2 The number of LSBs.
          * @param[in] scale A number to multiply the bits by before returning.
          * @return The decoded value.
          */
      unsigned long asUnsignedLong(const unsigned startBit1,
                                   const unsigned numBits1,
                                   const unsigned startBit2,
                                   const unsigned numBits2,
                                   const int scale ) const;

         /* Unpack a signed long integer */
      long asLong(const unsigned startBits[],
                  const unsigned numBits[],
                  const unsigned len,
                  const int scale ) const;

         /** Unpack an signed long integer split into two pieces.
          * @warning Be careful about what order you specify the parameters in.
          * @note This prototype obviates constructing an array before calling.
          * @param[in] startBit1 The 0-indexed first bit of the MSBs.
          * @param[in] numBits1 The number of MSBs.
          * @param[in] startBit2 The 0-indexed first bit of the LSBs.
          * @param[in] numBits2 The number of LSBs.
          * @param[in] scale A number to multiply the bits by before returning.
          * @return The decoded value.
          */
      long asLong(const unsigned startBit1,
                  const unsigned numBits1,
                  const unsigned startBit2,
                  const unsigned numBits2,
                  const int scale ) const;

         /* Unpack a split unsigned double */
      double asUnsignedDouble( const unsigned startBits[],
                               const unsigned numBits[],
                               const unsigned len,
                               const int power2) const;

         /** Unpack a positive-only floating point number split into two pieces.
          * @warning Be careful about what order you specify the parameters in.
          * @note This prototype obviates constructing an array before calling.
          * @param[in] startBit1 The 0-indexed first bit of the MSBs.
          * @param[in] numBits1 The number of MSBs.
          * @param[in] startBit2 The 0-indexed first bit of the LSBs.
          * @param[in] numBits2 The number of LSBs.
          * @param[in] power2 The result is multiplied by 2^(power2)
          *   before returning.
          * @return The decoded value.
          */
      double asUnsignedDouble(const unsigned startBit1,
                              const unsigned numBits1,
                              const unsigned startBit2,
                              const unsigned numBits2,
                              const int power2) const;

         /* Unpack a split signed double */
      double asSignedDouble( const unsigned startBits[],
                             const unsigned numBits[],
                             const unsigned len,
                             const int power2) const;

         /** Unpack a floating point number split into an arbitrary
          * number of pieces.  The startBits, numBits and whichSF
          * parameters must all have the same size, however in the
          * interest of performance, no checking is done to make sure
          * that is the case.  Additionally, the values in whichSF
          * must refer to valid indices of the bits vector, which also
          * is not validated.  The elements of startBits, numBits and
          * whichSF must correspond to the same segment for a given
          * vector index, and must be in order of MSB to LSB.
          * @note This prototype is intended to be used when the
          *   encoded value spans multiple subframes and thus multiple
          *   PackedNavBits objects.
          * @param[in] startBits The 0-indexed first bit of each segment.
          * @param[in] numBits The number of bits for each segment.
          * @param[in] whichSF The index into bits that pertains to the segment.
          * @param[in] bits The set of PackedNavBits objects used to
          *   construct the value being decoded.
          * @param[in] power2 The result is multiplied by 2^(power2)
          *   before returning.
          * @return The decoded value.
          */
      static double asUnsignedDouble(const std::vector<unsigned>& startBits,
                                     const std::vector<unsigned>& numBits,
                                     const std::vector<unsigned>& whichSF,
                                     const std::vector<PackedNavBitsPtr>& bits,
                                     const int power2);

         /** Unpack a floating point number split into an arbitrary
          * number of pieces.  The startBits, numBits and whichSF
          * parameters must all have the same size, however in the
          * interest of performance, no checking is done to make sure
          * that is the case.  Additionally, the values in whichSF
          * must refer to valid indices of the bits vector, which also
          * is not validated.  The elements of startBits, numBits and
          * whichSF must correspond to the same segment for a given
          * vector index, and must be in order of MSB to LSB.
          * @note This prototype is intended to be used when the
          *   encoded value spans multiple subframes and thus multiple
          *   PackedNavBits objects.
          * @param[in] startBits The 0-indexed first bit of each segment.
          * @param[in] numBits The number of bits for each segment.
          * @param[in] whichSF The index into bits that pertains to the segment.
          * @param[in] bits The set of PackedNavBits objects used to
          *   construct the value being decoded.
          * @param[in] power2 The result is multiplied by 2^(power2)
          *   before returning.
          * @return The decoded value.
          */
      static double asSignedDouble(const std::vector<unsigned>& startBits,
                                   const std::vector<unsigned>& numBits,
                                   const std::vector<unsigned>& whichSF,
                                   const std::vector<PackedNavBitsPtr>& bits,
                                   const int power2);

         /** Unpack a floating point number split into two pieces.
          * @warning Be careful about what order you specify the parameters in.
          * @note This prototype obviates constructing an array before calling.
          * @param[in] startBit1 The 0-indexed first bit of the MSBs.
          * @param[in] numBits1 The number of MSBs.
          * @param[in] startBit2 The 0-indexed first bit of the LSBs.
          * @param[in] numBits2 The number of LSBs.
          * @param[in] power2 The result is multiplied by 2^(power2)
          *   before returning.
          * @return The decoded value.
          */
      double asSignedDouble(const unsigned startBit1,
                            const unsigned numBits1,
                            const unsigned startBit2,
                            const unsigned numBits2,
                            const int power2) const;

         /* Unpack a split double with units of semicircles */
      double asDoubleSemiCircles( const unsigned startBits[],
                                  const unsigned numBits[],
                                  const unsigned len,
                                  const int power2) const;

         /** Unpack a floating point number split into two pieces,
          * converting from semi-circles to radians.
          * @warning Be careful about what order you specify the parameters in.
          * @note This prototype obviates constructing an array before calling.
          * @param[in] startBit1 The 0-indexed first bit of the MSBs.
          * @param[in] numBits1 The number of MSBs.
          * @param[in] startBit2 The 0-indexed first bit of the LSBs.
          * @param[in] numBits2 The number of LSBs.
          * @param[in] power2 The result is multiplied by 2^(power2)
          *   before returning.
          * @return The decoded value.
          */
      double asDoubleSemiCircles(const unsigned startBit1,
                                 const unsigned numBits1,
                                 const unsigned startBit2,
                                 const unsigned numBits2,
                                 const int power2) const;

      bool asBool( const unsigned bitNum) const;

         /***    PACKING FUNCTIONS *********************************/
         /** Pack an unsigned long integer
          * @throw InvalidParameter
          */
      void addUnsignedLong( const unsigned long value,
                            const int numBits,
                            const int scale );

         /** Pack a signed long integer
          * @throw InvalidParameter
          */
      void addLong( const long value,
                    const int numBits,
                    const int scale );

         /** Pack an unsigned double
          * @throw InvalidParameter
          */
      void addUnsignedDouble( const double value,
                              const int numBits,
                              const int power2);

         /** Pack a signed double
          * @throw InvalidParameter
          */
      void addSignedDouble( const double value,
                            const int numBits,
                            const int power2);

         /** Pack a double with units of semicircles
          * @throw InvalidParameter
          */
      void addDoubleSemiCircles( const double radians,
                                 const int numBits,
                                 const int power2);

         /**
          * Pack a string.
          * Characters in String limited to those defined in
          * IS-GPS-200 Section 20.3.3.5.1.8
          * If numChars < length of String only, chars 1..numChars
          * will be added.
          * If numChars > length of String, blanks will be added at the end.
          * @param[in] numChars represents number of chars (8 bits
          *   each) to add to PackedBits.
          * @throw InvalidParameter
          */
      void addString(const std::string String,
                     const int numChars);

         /**
          * Pack a vector of bytes.
          * @param[in] data The vector of bytes to append to the
          *   packed nav bits.
          * @param[in] numBits The actual number of bits to add.  Bits
          *   are added starting with the MSB of data[0].
          * @note The contents of data are assumed to be left-aligned
          *   and right padded, meaning that if you have have a vector
          *   of 8 bytes and you want to add 60 bits, data[7] is
          *   expected to contain the bits in the 4 MSBs and not in
          *   the 4 LSBs.
          * @throw InvalidParameter if numBits is > 8*data.size().
          */
      void addDataVec(const std::vector<uint8_t>& data, unsigned numBits);

         /** Pack a bitset.  This is done by converting it first to a
          * string of 0 and 1 characters which are then treated as an
          * array that is appended to the end of the bits vector.  Not
          * entirely sure if this is faster than iterating.
          * @param[in] newbits The bitset containing the data to
          *   append to the PackedNavBits data. */
      template <size_t N>
      void addBitset(const std::bitset<N>& newbits)
      {
         std::string binbits(newbits.to_string((char)0,(char)1));
         bits.insert(bits.end(), &binbits[0], &binbits[binbits.length()]);
         bits_used += binbits.length();
      }

         /**
          * @throw InvalidParameter
          */
      void addPackedNavBits( const PackedNavBits &pnb);

         /*
          * Output the packed bits as a set of 32 bit
          * hex values, four per line, without any
          * additional information.
          * Returns the number of bits in the object.
          */
      int outputPackedBits(std::ostream& s = std::cout,
		           const short numPerLine=4,
                           const char delimiter = ' ',
                           const short numBitsPerWord=32 ) const;

         /*
          * The equality operator insists that ALL the metadata
          * and the complete bit patterns must match.
          * However, there are frequently occaisions when only
          * a subset of the metadata need be checked, and sometimes
          * only certain set of bits.  Therefore, operator==( ) is
          * supplemented by matchBits( ) and matchMetaData( )
          */
      bool operator==(const PackedNavBits& right) const;

         /*
          * There are frequently cases in which we want to know
          * if a pair of PackedNavBits objects are from the same
          * SV, but we might want to allow for different receivers
          * and/or different ObsIDs.  Therefore, matchMetaData( )
          * allows specification of the particular metadata items
          * that are to be checked using a bit-flag system.
          */
      static const unsigned int mmTIME = 0x0001;  // Check transmitTime
      static const unsigned int mmSAT  = 0x0002;  // Check SatID
      static const unsigned int mmOBS  = 0x0004;  // Check ObsID
      static const unsigned int mmRX   = 0x0008;  // Check Receiver ID
      static const unsigned int mmNAV  = 0x0010;  // Check NavID
      static const unsigned int mmALL  = 0xFFFF;  // Check ALL metadata
      static const unsigned int mmNONE = 0x0000;  // NO metadata checks
      bool matchMetaData(const PackedNavBits& right,
                         const unsigned flagBits=mmALL) const;
         /*
          * Return true if all bits between start and end are identical
          * between this object and right.  Default is to compare all
          * bits.
          *
          * This method allows comparison of the "unchanging" data in
          * nav messages while avoiding the time tags.
          */
      bool matchBits(const PackedNavBits& right,
                     const short startBit=0,
                     const short endBit=-1) const;

         /*
          *  This is the most flexible of the matching methods.
          *  A default of match(right) will yield the same
          *  result as operator==( ).
          *  However, the arguments provide the means to
          *  specifically check bits sequences and/or
          *  selectively check the metadata.
          */
      bool match(const PackedNavBits& right,
                 const short startBit=0,
                 const short endBit=-1,
                 const unsigned flagBits=mmALL) const;
         /*
          * This version was the original equality checker.  As
          * first implemented, it checks ONLY SAT and OBS for
          * equality.  Therefore, it is maintained with that
          * default functionality.  That is to say, when
          * checkOverhead==true, the result is the same as a call
          * to matchBits(right,startBit,endBit, (mmSAT|mmOBS)).
          *
          * For clarity, it is suggested that new code use
          *  operator==(),
          *  matchMetaData(), and/or
          *  matchBits( ) using explicit flags.
          *
          * This version was REMOVED because of ambiguity
          * in the signature.
          *
          * The checkOverhead option allows the user to ignore
          * the associated metadata.  E.g. ObsID, SatID.
          *
          bool matchBits(const PackedNavBits& right,
          const short startBit=0,
          const short endBit=-1,
          const bool checkOverhead) const;
         */

         /**
          * The less than operator is defined in order to support use
          *   with the NavFilter classes.  The idea is to provide a
          *   "sort" for bits contained in the class.  Matching strings
          *   will fail both  a < b and b < a; however, in the process
          *   all matching strings can be sorted into sets and the
          *   "winner" determined.
          */
      bool operator<(const PackedNavBits& right) const;

         /**
          *  Bitwise invert contents of this object.
          */
      void invert( );

         /**
          *  Bit wise copy from another PackecNavBits.
          *  None of the meta-data (transmit time, SatID, ObsID)
          *  will be changed.
          *  This method is intended for use between two
          *  PackedNavBits objecst that are ALREADY the
          *  same size (in bits).
          *  Yes, we could define a copy that would account
          *  for the difference, but the pre-existing model
          *  for PNB is that the bits_used variable records
          *  the # of bits used as items are added to the end
          *  of the bit array.   I didn't want copyBits( )
          *  to confuse that model by modifying bits_used.
          * @throw InvalidParameter if called using two
          *   objects that are NOT the same size.
          */
      void copyBits(const PackedNavBits& src,
                    const short startBit=0,
                    const short endBit=-1);

         /**
          * This method is not typically used in production; however it
          * is used in test support.  It assumes the PNB object is already
          * created and is already sized to hold at least (startBit+numBits)
          * bits.  If this is not true, an exception is thrown.
          * It overwrites the data that is already present with
          * the provided value / scale.  If value / scale is too large to
          * fit in numBits, then an exception is thrown.
          * @throw InvalidParameter
          */
      void insertUnsignedLong(const unsigned long value,
                              const int startBit,
                              const int numBits,
                              const int scale=1 );
         /**
          *  Reset number of bits
          */
      void reset_num_bits(const int new_bits_used=0);

         /* Resize the vector holding the packed data. */
      void trimsize();

         /**
          * Raw bit input
          * This function is intended as a test-support function.
          * It assumes a string of the form
          *    ###  0xABCDABCD 0xABCDABCD 0xABCDABCD
          * where
          *    ### is the number of bits to expect in the remainder
          *        of the line.
          *    0xABCDABCD are each 32-bit unsigned hex numbers, left
          *        justified.  The number of bits needs to match or
          *        exceed ###
          * The function returns if the read is succeessful.
          * Otherwise,the function throws an exception
          * @throw InvalidParameter
          */
      void rawBitInput(const std::string inString );

      void setXmitCoerced(bool tf=true) {xMitCoerced=tf;}
      bool isXmitCoerced() const {return xMitCoerced;}

      const std::vector<bool>& getBits() const
      { return bits; }

         /** Indicate the status of parity/CRC checking.  Must be
          * explicitly set after construction, no parity checking is
          * supported in this class. */
      ParityStatus parityStatus;

   private:
      SatID satSys;            /**< System ID (based on RINEX defintions */
      ObsID obsID;             /**< Defines carrier and code tracked */
      NavID navID;             /**< Defines the navigation message tracked */
      std::string rxID;        /**< Defines the receiver that collected the data */
      CommonTime transmitTime; /**< Time nav message is transmitted */
      std::vector<bool> bits;  /**< Holds the packed data */
      int bits_used;

      bool xMitCoerced;        /**< Used to indicate that the transmit
                                  time is NOT directly derived from
                                  the SOW in the message */

         /** Unpack the bits
          * @throw InvalidParameter
          */
      uint64_t asUint64_t(const int startBit, const int numBits ) const;

         /** Pack the bits */
      void addUint64_t( const uint64_t value, const int numBits );

         /** Extend the sign bit for signed values */
      int64_t SignExtend( const int startBit, const int numBits ) const;

         /** Scales doubles by their corresponding scale factor */
      double ScaleValue( const double value, const int power2) const;

   }; // class PackedNavBits

      //@}
   std::ostream& operator<<(std::ostream& s, const PackedNavBits& pnb);

} // namespace

#endif
