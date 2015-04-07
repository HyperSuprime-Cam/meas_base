// -*- lsst-c++ -*-
/*
 * LSST Data Management System
 * Copyright 2008-2015 AURA/LSST.
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

%module testLib

%{
#include "SillyCentroid.h"
#include "lsst/afw/math.h"
#include "lsst/afw/detection.h"
#include "lsst/pex/logging.h"
#define PY_ARRAY_UNIQUE_SYMBOL LSST_MEAS_BASE_TEST_NUMPY_ARRAY_API
#include "numpy/arrayobject.h"
%}

%include "lsst/p_lsstSwig.i"

%import "lsst/afw/table/tableLib.i"
%import "lsst/afw/image/imageLib.i"
%import "lsst/meas/base/baseLib.i"
%import "lsst/pex/config.h"

%convertConfig(test::foo::bar, SillyTransform)
%feature("notabstract") test::foo::bar::SillyCentroidAlgorithm;
%include "SillyCentroid.h"
