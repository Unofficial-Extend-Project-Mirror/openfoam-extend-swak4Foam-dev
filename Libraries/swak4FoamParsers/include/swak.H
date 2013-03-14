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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakMacroHeader_H
#define SwakMacroHeader_H

#include "foamVersion4swak.H"
#if (FOAM_VERSION4SWAK_MAJOR < 2)
#error "This swak4Foam-version is only used for the 2.x-versions of OpenFOAM. For lower versions there is a special branch"
#endif

#if (FOAM_VERSION4SWAK_MAJOR > 1) || (FOAM_VERSION4SWAK_MINOR > 6)
// Just to make sure that stuff that comes from 1.7 works correctly
#define FOAM_HAS_SORTED_TOC
#else
#define FOAM_OLDTIME_PROBLEM
#endif

// in 1.6-ext the operation s1 & s2 of two symmetrical tensor fields does not yield a symmetric tensor
#if defined(FOAM_DEV) || FOAM_VERSION4SWAK_MAJOR>1
#define FOAM_SYMMTENSOR_WORKAROUND
#endif

// Certain OpenFOAM-versions don't have all tensor operations defined
#if FOAM_VERSION4SWAK_MAJOR<2 &&  FOAM_VERSION4SWAK_MINOR<7
#define FOAM_INCOMPLETE_OPERATORS
#endif

#if FOAM_VERSION4SWAK_MAJOR<1 || FOAM_VERSION4SWAK_MINOR<2
#define FOAM_DELTATCHEM_NOT_DIMENSIONED
#endif

// Versions before 2.1
#if FOAM_VERSION4SWAK_MAJOR<2 || (FOAM_VERSION4SWAK_MINOR<1 && FOAM_VERSION4SWAK_MAJOR==2)

// what is later called mappedPatch is called directMappedPatch
#define FOAM_MAPPED_IS_DIRECTMAPPED

#else

// has basicSources
#define FOAM_HAS_BASICSOURCES

#endif

// this changed starting with 2.1
#if FOAM_VERSION4SWAK_MAJOR>2 || (FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR==2)

// this Foam-Version now has ABI-patches
#define FOAM_HAS_ABI_PATCHES

#endif

// this changed starting with 2.2
#if FOAM_VERSION4SWAK_MAJOR>2 || (FOAM_VERSION4SWAK_MINOR>=2 && FOAM_VERSION4SWAK_MAJOR==2)

// lookupClass returns a non-const list
#define FOAM_LOOKUPCLASS_NO_CONST

// meshSearch parameter for sampledSet-constructor is now const
#define FOAM_MESHSEARCH_CONST_SAMPLEDSET

// interpolate-method for meshToMesh needs a reduce-operation
#define FOAM_MESHTOMESH_INTERPOLATE_REDUCE

// functionObject now needs at least dummy-implementations of updateMesh and movePoints
#define FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH

// new revised fluidThermo
#define FOAM_HAS_FLUIDTHERMO

// RR from the chemistry does no longer return a complete field
#define FOAM_RR_ONLY_DIMENSIONED_FIELD

// there is no mixed-BC for point patches
#define FOAM_NO_MIXED_POINT_PATCH

// this version has fvOptions
#define FOAM_HAS_FVOPTIONS

// movePoints now gets a polyMesh
#define FOAM_MOVEPOINTS_GETS_POLYMESH

#endif // version starting with 2.2

#include "swakVersion.H"

#endif

// ************************************************************************* //
