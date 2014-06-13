// -*- lsst-c++ -*-
/*
 * LSST Data Management System
 * Copyright 2008-2013 LSST Corporation.
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */

#ifndef LSST_MEAS_BASE_TestFlux_h_INCLUDED
#define LSST_MEAS_BASE_TestFlux_h_INCLUDED

/**
 *  @file lsst/meas/base/TestFlux.h
 *  This is a class which knows how to calculate fluxes using the GAUSSIAN photometry algorithm
 */
#include <string>
#include "lsst/pex/config.h"
#include "lsst/afw/image/Exposure.h"
#include "lsst/meas/base/Inputs.h"
#include "lsst/meas/base/Results.h"
#include "lsst/meas/base/algorithms/SdssShapeImpl.h"

namespace lsst { namespace meas { namespace base {

    typedef struct {
        Flux flux;
        FluxErrElement fluxSigma;
    } TestFluxComponent;

/**
 *  @brief A C++ control class to handle TestFluxAlgorithm's configuration
 */
class TestFluxControl {
public:
    LSST_CONTROL_FIELD(fixed, bool,
                       "if true, use existing shape and centroid measurements instead of fitting");
    LSST_CONTROL_FIELD(background, double, "FIXME! NEVER DOCUMENTED!");
    LSST_CONTROL_FIELD(shiftmax, double, "FIXME! NEVER DOCUMENTED!");
    LSST_CONTROL_FIELD(centroid, std::string, "name of centroid field to use if fixed is true");
    LSST_CONTROL_FIELD(shape, std::string, "name of shape field to use if fixed is true");
    LSST_CONTROL_FIELD(shapeFlag, std::string, "suffix of shape field flag to check if fixed is true");
    LSST_CONTROL_FIELD(maxIter, int, "Maximum number of iterations");
    LSST_CONTROL_FIELD(tol1, float, "Convergence tolerance for e1,e2");
    LSST_CONTROL_FIELD(tol2, float, "Convergence tolerance for FWHM");
    LSST_CONTROL_FIELD(usePixelWeights, bool, "Whether to use per-pixel inverse variance as weights");
    LSST_CONTROL_FIELD(badMaskPlanes, std::vector<std::string>,
                       "Mask planes that indicate pixels that should be excluded from the fit");

    /**
     *  @brief Default constructor
     *
     *  All control classes should define a default constructor that sets all fields to their default values.
     */
    TestFluxControl() :
        fixed(false), background(0.0), shiftmax(10.0),
        centroid("shape.sdss.centroid"), shape("shape.sdss"), shapeFlag(".flags"),
        maxIter(algorithms::SDSS_SHAPE_MAX_ITER),
        tol1(algorithms::SDSS_SHAPE_TOL1), tol2(algorithms::SDSS_SHAPE_TOL2)
    {}
};


/**
 *  @brief A measurement algorithm that estimates flux using the TestFlux algorithm
 */
class TestFluxAlgorithm {
public:

    /**
     *  @brief Flag bits to be used with the 'flags' data member of the Result object.
     *
     *  Inspect getFlagDefinitions() for more detailed explanations of each flag.
     *
     *  Note that we've included a final N_FLAGS value that isn't a valid flag; this is a common C++
     *  idiom for automatically counting the number of enum values, and it's required for Algorithms
     *  as the N_FLAGS value is used by the Result and ResultMapper objects.
     */
    enum FlagBits {
        NO_PSF=0,
        NO_GOOD_PIXELS,
        EDGE,
        NO_FIXED,
        N_FLAGS
    };

    /**
 *      *  @brief Return an array of (name, doc) tuples that describes the flags and sets the names used
 *           *         in catalog schemas.
 *                */
    static boost::array<FlagDef,N_FLAGS> const & getFlagDefinitions() {
        static boost::array<FlagDef,N_FLAGS> const flagDefs = {{
                {"noPsf", "No Psf object attached to the Exposure object being measured"},
                {"noGoodPixels", "No usable pixels in fit region"},
                {"edge", "Could not use full PSF model image in fit because of proximity to exposure border"},
                {"noFixed", "Fixed option has been deprecated"}
            }};
        return flagDefs;
    }


    //typedef Result1<TestFluxAlgorithm, TestFluxComponent> Result; 

    typedef struct Result : FlagsComponent<TestFluxAlgorithm>
    {
        //bool getFlag(FlagBits bit) const { return _flags[bit]; }

        //void setFlag(FlagBits bit, bool value=true) { _flags[bit] = value; }

        //void unsetFlag(FlagBits bit) { _flags[bit] = false; }

        double flux;
        double fluxSigma;
        std::bitset<N_FLAGS> _flags;
    } Result;
   

    static boost::array<std::string, 2> const & getFieldNames() {
        static boost::array<std::string, 2> const fieldNames = {{
            "flux",
            "fluxSigma"
        }};
        return fieldNames;
    }
  
    static boost::array<std::string, 2> const & getFieldTypes() {
        static boost::array<std::string, 2> const fieldTypes = {{
            "D",
            "D",
        }};
        return fieldTypes;
    }
  
    static boost::array<std::string, 4> const & getFlagNames() {
        static boost::array<std::string, 4> const flagNames = {{
            "flag_noPsf",
            "flag_noGoodPixels",
            "flag_edge",
            "flag_noFixed"
        }};
        return flagNames;
    }
  
    static unsigned int getFieldCount() {
        return getFieldNames().size() + N_FLAGS + 1;
    }

    static std::string const & getFieldName(unsigned int index) {
        static std::string const & flagName = std::string("flag");
        unsigned int flagOffset = getFieldNames().size();
        if (index < flagOffset) return getFieldNames()[index];
        if (index == flagOffset) return flagName;
        return getFlagNames()[index-flagOffset-1];
    }
    
    static std::string const & getFieldType(unsigned int index) {
        unsigned int flagOffset = getFieldNames().size();
        if (index < flagOffset) return getFieldTypes()[index];
        static std::string const & flagType = std::string("Flag");
        return flagType;
    }
    
    /// A typedef to the Control object for this algorithm, defined above.
    /// The control object contains the configuration parameters for this algorithm.
    typedef TestFluxControl Control;

    typedef FootprintCentroidInput Input; // type passed to apply in addition to Exposure.

    /**
     *  @brief Measure the flux of a source using the TestFlux algorithm.
     */
    template <typename T>
    static Result apply(
        afw::image::Exposure<T> const & exposure,
        afw::geom::Point2D const & position,
        Control const & ctrl=Control()
    );

    /**
     *  @brief Apply the TestFlux to a single source using the Plugin API.
     *
     *  This is the version that will be called by both the SFM framework and the forced measurement
     *  framework, in single-object mode.  It will delegate to the other overload of apply().  Note that
     *  we can use the same implementation for both single-frame and forced measurement, because we require
     *  exactly the same inputs in both cases.  This is true for the vast majority of algorithms, but not
     *  all (extended source photometry is the notable exception).
     */
    template <typename T>
    static Result apply(
        afw::image::Exposure<T> const & exposure,
        Input const & inputs,
        Control const & ctrl=Control()
    );

};

}}} // namespace lsst::meas::base

#endif // !LSST_MEAS_BASE_TestFlux_h_INCLUDED
