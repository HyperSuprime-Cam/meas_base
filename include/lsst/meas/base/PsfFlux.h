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

#ifndef LSST_MEAS_BASE_PsfFlux_h_INCLUDED
#define LSST_MEAS_BASE_PsfFlux_h_INCLUDED

/**
 *  @file lsst/meas/base/PsfFlux.h
 *
 *  This file is one of two (the other is SdssShape.h) intended to serve as an tutorial example on
 *  how to implement new Algorithms.  PsfFluxAlgorithm is a particularly simple algorithm, while
 *  SdssShapeAlgorithm is more complex.
 *
 *  See @ref measBaseImplementingNew for a general overview of the steps required.
 */

#include "lsst/pex/config.h"
#include "lsst/afw/image/Exposure.h"
#include "lsst/meas/base/Inputs.h"
#include "lsst/meas/base/ResultMappers.h"

namespace lsst { namespace meas { namespace base {

/**
 *  @brief A C++ control class to handle PsfFluxAlgorithm's configuration
 *
 *  In C++, we define Control objects to handle configuration information.  Using the LSST_CONTROL_FIELD
 *  macro and lsst.pex.config.wrap.makeConfigClass, we can turn these into more full-featured Config classes
 *  in Python.  While the user will usually interact with the Config class, the plugin wrapper system will
 *  turn Config instances into Control instances when passing them to C++.
 *
 *  This should logically be an inner class, but Swig doesn't know how to parse those.
 */
class PsfFluxControl {
public:
    LSST_CONTROL_FIELD(usePixelWeights, bool, "Whether to use per-pixel inverse variance as weights");
    LSST_CONTROL_FIELD(badMaskPlanes, std::vector<std::string>,
                       "Mask planes that indicate pixels that should be excluded from the fit");

    /**
     *  @brief Default constructor
     *
     *  All control classes should define a default constructor that sets all fields to their default values.
     */
    PsfFluxControl() : usePixelWeights(false) {}
};


/**
 *  @brief A measurement algorithm that estimates flux using a linear least-squares fit with the Psf model
 *
 *  As an Algorithm class, all of PsfFluxAlgorithm's core functionality is availabe via static methods
 *  (in fact, there should be no reason to ever construct an instance).
 *
 *  Almost all of the implementation of PsfFluxAlgorithm is here and in PsfFluxAlgorithm.cc, but there
 *  are also a few key lines in the Swig .i file:
 *  @code
 *  %include "lsst/meas/base/PsfFlux.h"
 *  %template(apply) lsst::meas::base::PsfFluxAlgorithm::apply<float>;
 *  %template(apply) lsst::meas::base::PsfFluxAlgorithm::apply<double>;
 *  %wrapMeasurementAlgorithm1(lsst::meas::base, PsfFluxAlgorithm, PsfFluxControl, FootprintCentroidInput,
 *                             FluxComponent)
 *  @endcode
 *  and in the pure Python layer:
 *  @code
 *  WrappedSingleFramePlugin.generate(PsfFluxAlgorithm)
 *  @endcode
 *  The former ensure the Algorithm class is fully wrapped via Swig (including @c %%template instantiations
 *  of its @c Result and @c ResultMapper classes), and the latter actually generates the Config class and
 *  the Plugin classes and registers them.
 */
class PsfFluxAlgorithm {
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
        N_FLAGS
    };

    /**
     *  @brief Return an array of (name, doc) tuples that describes the flags and sets the names used
     *         in catalog schemas.
     *
     *  Each element of the returned array should correspond to one of the FlagBits enum values, but the
     *  names should follow conventions; FlagBits should be ALL_CAPS_WITH_UNDERSCORES, while FlagDef names
     *  should be camelCaseStartingWithLowercase.  @sa FlagsComponentMapper.
     */
    static boost::array<FlagDef,N_FLAGS> const & getFlagDefinitions();

    /// Just a typedef to the Control object defined above.
    typedef PsfFluxControl Control;

    /**
     *  This is the type returned by apply().  Because PsfFluxAlgorithm only measures a flux and its
     *  uncertainty, we can use one of the predefined components, FluxComponent, without any modification.
     */
    typedef Result1<PsfFluxAlgorithm,FluxComponent> Result;

    /**
     *  The ResultMapper typedef here exactly corresponds to the the Result typedef defined above: there's
     *  a corresponding ComponentMapper for each Component.
     */
    typedef Result::Mapper ResultMapper;

    /**
     *  In the actual overload of apply() used by the Plugin system, this is the only argument besides the
     *  Exposure being measured.  PsfFluxAlgorithm only needs a centroid, so we use FootprintCentroidInput.
     */
    typedef FootprintCentroidInput Input; // type passed to apply in addition to Exposure.

    /**
     *  @brief Create an object that transfers Result values to a record associated with the given schema
     *
     *  This is called by the Plugin wrapper system to create a ResultMapper.  It's responsible for calling
     *  the ResultMapper constructor, forwarding the schema and prefix arguments and providing the correct
     *  values for the uncertainty arguments.
     */
    static ResultMapper makeResultMapper(
        afw::table::Schema & schema,
        std::string const & prefix,
        Control const & ctrl=Control()
    );

    /**
     *  @brief Measure the flux of a source using the PsfFlux algorithm.
     *
     *  This is the overload of apply() that does all the work, and it's designed to be as easy to use
     *  as possible outside the Plugin framework (since the Plugin framework calls the other one).  The
     *  arguments are all the things we need, and nothing more: we don't even pass a Footprint, since
     *  we wouldn't actually use it, and if we didn't need to get a Psf from the Exposure, we'd use
     *  MaskedImage instead.
     */
    template <typename T>
    static Result apply(
        afw::image::Exposure<T> const & exposure,
        afw::geom::Point2D const & position,
        Control const & ctrl=Control()
    );

    /**
     *  @brief Apply the PsfFlux to a single source using the Plugin API.
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

#endif // !LSST_MEAS_BASE_PsfFlux_h_INCLUDED
