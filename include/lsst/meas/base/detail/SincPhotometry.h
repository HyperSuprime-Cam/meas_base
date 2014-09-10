// -*- LSST-C++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
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
 
#if !defined(LSST_MEAS_BASE_ALGORITHMS_DETAIL_SINCPHOTOMETRY_H)
#define LSST_MEAS_BASE_ALGORITHMS_DETAIL_SINCPHOTOMETRY_H 1
/**
 * @file SincPhotometry.h
 * @brief Compute Aperture using sinc interpolation
 * @ingroup meas/base/algorithms
 * @author Steve Bickerton (adapted from RHL's Shape class)
 */
namespace lsst {
namespace meas {
namespace base {
namespace detail {

template<typename PixelT>
typename lsst::afw::image::Image<PixelT>::Ptr calcImageRealSpace(double const rad1, double const rad2,
                                                                 double const taper=0.1);

template<typename PixelT>
typename lsst::afw::image::Image<PixelT>::Ptr calcImageKSpaceReal(double const rad1, double const rad2);

template<typename PixelT>
typename lsst::afw::image::Image<PixelT>::Ptr calcImageKSpaceCplx(double const rad1, double const rad2,
                                                                  double const posAng, double const ell);

}}}} // end of lsst::meas::base::detail
#endif
