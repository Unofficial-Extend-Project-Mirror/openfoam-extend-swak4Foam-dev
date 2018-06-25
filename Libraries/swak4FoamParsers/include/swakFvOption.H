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

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakFvOptionMacroHeader_H
#define SwakFvOptionMacroHeader_H

#include "swak.H"

#ifdef FOAM_HAS_FVOPTIONS

#include "fvOption.H"
#include "makeFvOption.H"

#if (FOAM_VERSION4SWAK >= VERSION_NR(3,0,0)) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)

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

#if (FOAM_VERSION4SWAK >= VERSION_NR(2,3,1)) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)
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
