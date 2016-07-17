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

#ifndef SwakThermoMacroHeader_H
#define SwakThermoMacroHeader_H

#include "swak.H"

#ifdef FOAM_HAS_FLUIDTHERMO

#include "psiThermo.H"
#include "rhoThermo.H"

namespace Foam {
    typedef fluidThermo swakFluidThermoType;
    typedef psiThermo swakPsiThermoType;
    typedef rhoThermo swakRhoThermoType;
}

#else

#include "basicPsiThermo.H"
#include "basicRhoThermo.H"

namespace Foam {
    typedef basicThermo swakFluidThermoType;
    typedef basicPsiThermo swakPsiThermoType;
    typedef basicRhoThermo swakRhoThermoType;
}

#endif


#include "DebugOStream.H"

#endif

// ************************************************************************* //
