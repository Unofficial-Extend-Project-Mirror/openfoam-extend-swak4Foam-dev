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

#include "swakVersion.H"

#endif

// ************************************************************************* //
