/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class


Description
    Define types that were renamed in swak4Foam 2.2

SourceFiles

Contributors/Copyright:
    2012-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

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

#ifndef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_RADIATION_HAS_SOOTMODEL
#define FOAM_RADIATION_HAS_PUBLIC_ABSORPTIONMODEL
#else
#define FOAM_BASIC_THERMO_HAS_NO_NEW
#endif

#if (OPENFOAM_COM >= 1706)                                          \
    || (defined(FOAM_VERSION4SWAK_IS_ORG) && FOAM_VERSION4SWAK>=VERSION_NR(5,0,0))
#define FOAM_CHEMISTRYMODEL_HAS_NO_SOURCE_TERM
#endif

#if (OPENFOAM_COM >= 1803)                                          \
    || (defined(FOAM_VERSION4SWAK_IS_ORG) && FOAM_VERSION4SWAK>=VERSION_NR(6,0,0))
#define FOAM_NO_PSICHEMISTRY_MODEL
#endif


#include "DebugOStream.H"

#endif

// ************************************************************************* //
