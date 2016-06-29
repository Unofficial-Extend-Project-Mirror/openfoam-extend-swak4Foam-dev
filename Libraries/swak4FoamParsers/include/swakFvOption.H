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
    Define types that were renamed in OpenFOAM 2.2

SourceFiles

Contributors/Copyright:
    2012-2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakFvOptionMacroHeader_H
#define SwakFvOptionMacroHeader_H

#include "swak.H"

#ifdef FOAM_HAS_FVOPTIONS

#include "fvOption.H"
#include "makeFvOption.H"

#if (FOAM_VERSION4SWAK >= VERSION_NR(3,0,0)) && !defined(FOAM_DEV)

#include "cellSetOption.H"

namespace Foam {
    typedef fv::cellSetOption swakFvOptionType;
}

#define FOAM_FVOPTION_MAKE_NOT_IN_NAMESPACE

#else



namespace Foam {
    typedef fv::option swakFvOptionType;
}

#endif

#define makeSwakFvOption makeFvOption

#if (FOAM_VERSION4SWAK >= VERSION_NR(2,3,1)) && !defined(FOAM_DEV)
#define FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
#endif

#else

#include "basicSource.H"
#include "makeBasicSource.H"

namespace Foam {
    typedef basicSource swakFvOptionType;
}

#define makeSwakFvOption makeBasicSource

#endif

#endif

// ************************************************************************* //
