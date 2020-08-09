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
    2012-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017-2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakCloudMacroHeader_H
#define SwakCloudMacroHeader_H

#include "swak.H"

#ifdef FOAM_HAS_FLUIDTHERMO_CLOUD

#include "fluidThermoCloud.H"

namespace Foam {
    typedef fluidThermoCloud swakFluidThermoCloudType;
}

#else

#include "basicThermoCloud.H"

namespace Foam {
    typedef basicThermoCloud swakFluidThermoCloudType;
}
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_REACTINGCLOUD_TEMPLATED
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_KINEMATICCLOUD_OLD_STYLE
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_THERMOCLOUD_OLD_STYLE
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_BASICKINEMATICCLOUD_HAS_NO_KINEMATIC_TYPE
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_MAKEINJECTOR_TEMPLATE_HAS_3_PARAMETERS
#endif

#ifndef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_INJECT_PREPAREFORNEXT_RETURNS_BOOL
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_INJECT_TOINJECT_IS_CONST
#endif

#ifndef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_INJECT_SETPOSITION_HAS_TET_PARAMETERS
#endif

#ifndef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_INJECT_CONSTRUCTOR_HAS_MODELNAME
#endif

#ifdef FOAM_VERSION4SWAK_IS_EXTEND
#define FOAM_REACTINGCLOUD_OLD_STYLE
#endif


#if FOAM_VERSION4SWAK > VERSION_NR2(2,2)
#define FOAM_PHASECHANGEMODEL_HAS_TVAP
#endif

#if (FOAM_VERSION4SWAK > VERSION_NR2(2,0)) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_HAS_COLLIDING_CLOUD
#endif

#if (FOAM_VERSION4SWAK > VERSION_NR2(2,2)) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_HAS_MPPIC_CLOUD
#endif

#if (FOAM_VERSION4SWAK > VERSION_NR(2,3,1)) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_KINEMATIC_PARCEL_HAS_MINPARCELMASS
#endif

#if !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_KINEMATIC_PARCEL_HAS_DTARGET
#endif

#if !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_KINEMATIC_PARCEL_HAS_AGE
#endif

#if (FOAM_VERSION4SWAK > VERSION_NR2(3,1)) && defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_CLOUDTEMPLATE_IS_CLOUD_H
#endif

#if (OPENFOAM_COM >= 1606)
#define FOAM_KINEMATIC_PARCEL_ACTIVE_SET_THROUGH_METHOD
#endif

#if (OPENFOAM_COM >= 1712) \
    || (defined(FOAM_VERSION4SWAK_IS_ORG) && FOAM_VERSION4SWAK>=VERSION_NR(5,0,0))
#define FOAM_BARYCENTRIC_PARTICLES
#endif

#if (OPENFOAM_COM >= 1712)                                          \
    || (defined(FOAM_VERSION4SWAK_IS_ORG) && FOAM_VERSION4SWAK>=VERSION_NR(5,0,0))
#define FOAM_IOPOSITION_NEEDS_STREAM_IN_READDATA
#endif

#if (OPENFOAM_COM >= 1712)                                          \
    || (defined(FOAM_VERSION4SWAK_IS_ORG) && FOAM_VERSION4SWAK>=VERSION_NR(6,0,0))
#define FOAM_REACTING_PARCEL_HAS_NO_PC_METHOD
#endif

#if defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_IOPOSITION_IS_TEMPLATE_OF_PARTICLE
#endif

#if (OPENFOAM_COM >= 2004)
#define FOAM_PARTICLE_STREAM_CONSTRUCTOR_WITH_NEW_FORMAT
#endif

#if (OPENFOAM_COM >= 2004)
#define FOAM_CLOUD_FUNCTION_OBJECT_PRE_POST_EVOLVE_WITH_TD
#endif

#include "DebugOStream.H"

#endif

// ************************************************************************* //
