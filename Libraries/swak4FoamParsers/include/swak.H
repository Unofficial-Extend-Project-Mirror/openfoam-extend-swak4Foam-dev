/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class


Description
    Define versions and flags for swak to distinguish different features
    via #ifdef

SourceFiles

Contributors/Copyright:
    2012-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakMacroHeader_H
#define SwakMacroHeader_H

#include "foamVersion4swak.H"

#define VERSION_NR(major,minor,patch) (     \
    10000 * major + \
    100   * minor + \
    patch )

#define VERSION_NR2(major,minor) (     \
    10000 * major + \
    100   * minor + \
    99 )

#define FOAM_VERSION4SWAK VERSION_NR(FOAM_VERSION4SWAK_MAJOR,FOAM_VERSION4SWAK_MINOR,FOAM_VERSION4SWAK_PATCH_NUM)

#if FOAM_VERSION4SWAK > VERSION_NR2(1,6)
#define FOAM_HAS_SORTED_TOC
#else
#define FOAM_OLDTIME_PROBLEM
#endif

#if FOAM_VERSION4SWAK >= VERSION_NR(2,0,0)
#error "This swak4Foam-version is only used for the 1.x-versions of OpenFOAM. For higher versions there is a special branch"
#endif

// in 1.6-ext the operation s1 & s2 of two symmetrical tensor fields does not yield a symmetric tensor
#ifdef FOAM_DEV
#define FOAM_SYMMTENSOR_WORKAROUND
#endif

// in 1.6-ext nextRelease a different Macro is used for the creation of certain patch types
#ifdef FOAM_DEV
#define FOAM_USE_MAKE_TEMPLATE_PATCH_TYPE
#endif

// Certain OpenFOAM-versions don't have all tensor operations defined
#if FOAM_VERSION4SWAK < VERSION_NR(1,7,0)
#define FOAM_INCOMPLETE_OPERATORS
#endif

// Certain OpenFOAM-versions don't have this method in fvMesh
#if FOAM_VERSION4SWAK < VERSION_NR(1,7,0)
#define FOAM_FV_MESH_HAS_NO_SOLVERDICT
#endif

// Additional tensor types in nextRelease
#ifdef FOAM_DEV
#define FOAM_DEV_ADDITIONAL_TENSOR_TYPES
#endif

// The kineamticPArcel has no property active in 1.6-ext
#ifdef FOAM_DEV
#define FOAM_KINEMTATIC_HAS_NO_ACTIVE_PROPERTY
#endif

#include "swakVersion.H"

#include "DebugOStream.H"

#endif

// ************************************************************************* //
