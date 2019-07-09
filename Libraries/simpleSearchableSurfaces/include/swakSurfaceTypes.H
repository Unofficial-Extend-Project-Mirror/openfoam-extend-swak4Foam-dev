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
    Define types for searchableSurfaces that changed

SourceFiles

Contributors/Copyright:
    2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakSurfaceTypesMacroHeader_H
#define SwakSurfaceTypesMacroHeader_H

#include "swak.H"

#ifdef FOAM_VOLUMETYPE_IS_TYPE

#if (defined(FOAM_VERSION4SWAK_IS_ORG) && FOAM_VERSION4SWAK>=VERSION_NR(7,0,0))
#define FOAM_VOLUMETYPE_ENUM_LOWERCASE
#endif

#ifdef FOAM_VOLUMETYPE_ENUM_LOWERCASE
    #define VOLTYPE_INSIDE volumeType::inside
    #define VOLTYPE_OUTSIDE volumeType::outside
    #define VOLTYPE_UNKNOWN volumeType::unknown
    #define VOLTYPE_MIXED volumeType::mixed
#else
    #define VOLTYPE_INSIDE volumeType::INSIDE
    #define VOLTYPE_OUTSIDE volumeType::OUTSIDE
    #define VOLTYPE_UNKNOWN volumeType::UNKNOWN
    #define VOLTYPE_MIXED volumeType::MIXED
#endif

#endif

#if FOAM_VERSION4SWAK > VERSION_NR2(2,2) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_SEARCHABLE_SURF_NEEDS_BOUNDING_SPHERES
#endif


#endif

// ************************************************************************* //
