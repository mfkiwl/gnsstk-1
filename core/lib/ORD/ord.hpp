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

/*
 * ord.hpp - ORD Header File
 *
 * This file should expose all of the 'atomic' entry points for calculating the Observed Range Deviation.
 *
 * Still to do:
 *   - Rename raw_range_x() methods to more appropriate names.
 *   - Replace raw_range index with proper enumeration.
 */

#ifndef CORE_LIB_ORD_ORD_HPP_
#define CORE_LIB_ORD_ORD_HPP_

#include <vector>

#include "IonoModelStore.hpp"
#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Position.hpp"
#include "NavLibrary.hpp"
#include "TropModel.hpp"

namespace gnsstk {
namespace ord {

/// Given a set of frequency and pseudorange pairs, attempts to compensate
/// for ionospheric effects. Vectors are used because they carry a size
/// parameter, another collection type may be more appropriate.
/// @param frequencies Signal frequencies.
/// @param pseudoranges Pseudorange values for frequencies.
/// @return Corrected pseudorange in meters
double IonosphereFreeRange(const std::vector<double>& frequencies,
        const std::vector<double>& pseudoranges);

/// Given an ionosphere model, and locations of receiver and satellite,
/// range correction due to ionospheric effects.
/// @param ionoModel Class that encapsulates ionospheric models
/// @param time The time of interest.
/// @param frequency Frequency of interest - see note above.
/// @param rxLoc The location of the receiver.
/// @param svXvt The location of the satellite at time of interest.
/// @return Range correction (delta) in meters
double IonosphereModelCorrection(const gnsstk::IonoModelStore& ionoModel,
        const gnsstk::CommonTime& time, CarrierBand band,
        const gnsstk::Position& rxLoc, const gnsstk::Xvt& svXvt);

/// Given a satellite id, a time, and an ephemeris store, retrieves the
/// satellite location/velocity in xvt instance. This is a relatively thin
/// wrapper for NavLibrary.getXvt() to bring the method into the same
/// namespace as the other range calculations.
/// @param sat_id Identifier for the satellite
/// @param time The time of interest.
/// @param ephemeris The ephemeris to query against.
/// @return Xvt instance containing satellite location/velocity
gnsstk::Xvt getSvXvt(const gnsstk::SatID& sat_id, const gnsstk::CommonTime& time,
        NavLibrary& ephemeris);

/// Calculate the raw range at RECEIVE time per RECEIVER clock.
/// @see CorrectedEphemerisRange::ComputeAtReceiveTime()
/// @param rx_loc The location of the receiver.
/// @param sat_id Identifier for the satellite
/// @param time The nominal receive time.
/// @param ephemeris The ephemeris to query against.
/// @param sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange1(const gnsstk::Position& rx_loc, const gnsstk::SatID& sat_id,
        const gnsstk::CommonTime& time,
        NavLibrary& ephemeris, gnsstk::Xvt& sv_xvt);

/// Calculate the raw range at TRANSMIT time per the RECEIVER clock.
/// @see CorrectedEphemerisRange::ComputeAtTransmitTime(const CommonTime&, const double&, const Position&, const SatID, NavLibrary&, NavSearchOrder, SVHealth, NavValidityType)
/// @param pseudorange Pseudorange in meters to seed the calculation.
/// @param rx_loc The location of the receiver.
/// @param sat_id Identifier for the satellite
/// @param time The nominal receive time.
/// @param ephemeris The ephemeris to query against.
/// @param sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange2(double pseudorange, const gnsstk::Position& rx_loc,
        const gnsstk::SatID& sat_id, const gnsstk::CommonTime& time,
        NavLibrary& ephemeris, gnsstk::Xvt& sv_xvt);

/// Calculate the raw range at TRANSMIT time per the SATELLITE clock
/// @see CorrectedEphemerisRange::ComputeAtTransmitSvTime()
/// @param pseudorange Pseudorange in meters to seed the calculation.
/// @param rx_loc The location of the receiver.
/// @param sat_id Identifier for the satellite
/// @param time The transmit time reported by satellite.
/// @param ephemeris The ephemeris to query against.
/// @param sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange3(double pseudorange, const gnsstk::Position& rx_loc,
        const gnsstk::SatID& sat_id, const gnsstk::CommonTime& time,
        NavLibrary& ephemeris, gnsstk::Xvt& sv_xvt);

/// Calculate the raw range at TRANSMIT time per RECEIVER clock, without
/// seeding the pseudorange.
/// @see CorrectedEphemerisRange::ComputeAtTransmitTime(const CommonTime& trNom, const Position&, const SatID, NavLibrary&, NavSearchOrder, SVHealth, NavValidityType)
/// @param rx_loc The location of the receiver.
/// @param sat_id Identifier for the satellite
/// @param time The nominal receive time.
/// @param ephemeris The ephemeris to query against.
/// @param sv_xvt Final SV Position/Velocity returned here.
/// @return Range in meters
double RawRange4(const gnsstk::Position& rx_loc, const gnsstk::SatID& sat_id,
        const gnsstk::CommonTime& time,
        NavLibrary& ephemeris, gnsstk::Xvt& sv_xvt);

/// Calculate the range delta due to clock bias.
/// Note: Most of the work is actually done by the Xvt object.
/// @param svXvt Satellite location/velocity
/// @returns Range correction (delta) in meters
double SvClockBiasCorrection(const gnsstk::Xvt& svXvt);

// Calculate the range delta due to relativistic effects
// Note: Most of the work is actually done by the Xvt object.
/// @param svXvt Satellite location/velocity
/// @returns Range correction (delta) in meters
double SvRelativityCorrection(gnsstk::Xvt& svXvt);

/// Given a troposphere model, and locations of receiver and satellite,
/// calculates tropospheric effects.
/// @param trop_model Class that encapsulates troposphere models
/// @param rx_loc The location of the receiver.
/// @param sv_xvt The location of the satellite at time of interest.
/// @return Range correction (delta) in meters
double TroposphereCorrection(const gnsstk::TropModel& trop_model,
        const gnsstk::Position& rx_loc, const gnsstk::Xvt& sv_xvt);

/// Example method that applies _all_ corrections to generate an Observed Range Deviation.
/// This is intended to be a sample showing how the above methods will be used.
/// The example is not fully developed, just a general sketch of a generic approach.
/// E.g., if dual-band, there should be no additional ionosphere correction applied.
/// Users should construct an ORD based on their data and use case.
/// Parameters:
/// @param[in] bands Signal bands (one or two enums, for single-band/dual-band).
/// @param[in] pseudoranges Pseudorange values, corresponding to frequency array (one or two).
/// @param[in] trop_model Class that encapsulates ionospheric models.
/// @param[in] rx_loc The location of the receiver.
/// @param[in] sat_id Identifier for the satellite.
/// @param[in] transmit_time The transmit time reported by satellite.
/// @param[in] receive_time The nominal receive time.
/// @param[in] iono_model Class that encapsulates ionospheric models.
/// @param[in] trop_model Class that encapsulates troposphere models.
/// @param[in] ephemeris The ephemeris to query against.
/// @param[in] range_method One of four raw range methods, depending on what data is available.
/// @returns Observed range deviation from 1st pseudorange.
/*
double calculate_ord(const std::vector<CarrierBand>& bands,
        const std::vector<double>& pseudoranges,
        const gnsstk::Position& rx_loc,
        const gnsstk::SatID& sat_id,
        const gnsstk::CommonTime& transmit_time,
        const gnsstk::CommonTime& receive_time,
        const gnsstk::IonoModelStore& iono_model,
        const gnsstk::TropModel& trop_model,
        NavLibrary& ephemeris,
        int range_method);
 */

}  // namespace ord
}  // namespace gnsstk

#endif  // CORE_LIB_ORD_ORD_HPP_
