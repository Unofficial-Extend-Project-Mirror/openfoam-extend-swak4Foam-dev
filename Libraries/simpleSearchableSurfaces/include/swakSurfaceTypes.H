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
    Define types for searchableSurfaces that changed

SourceFiles

Contributors/Copyright:
    2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakSurfaceTypesMacroHeader_H
#define SwakSurfaceTypesMacroHeader_H

#include "swak.H"

#ifdef FOAM_VOLUMETYPE_IS_TYPE

#define INSIDE volumeType::INSIDE
#define OUTSIDE volumeType::OUTSIDE
#define UNKNOWN volumeType::UNKNOWN
#define MIXED volumeType::MIXED

#endif

#if FOAM_VERSION4SWAK > VERSION_NR2(2,2) && !defined(FOAM_VERSION4SWAK_IS_EXTEND)
#define FOAM_SEARCHABLE_SURF_NEEDS_BOUNDING_SPHERES
#endif

#endif

// ************************************************************************* //
