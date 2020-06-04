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
    Foam::MeshInterpolationOrder

Description

SourceFiles
    MeshInterpolationOrder.C

Contributors/Copyright:
    2012-2014, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: MeshInterpolationOrder.H,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef MeshInterpolationOrder_H
#define MeshInterpolationOrder_H

#include "swak.H"

#include "meshToMesh.H"

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
#include "Enum.H"
#else
#include "NamedEnum.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

#ifndef FOAM_NEW_MESH2MESH

/*---------------------------------------------------------------------------*\
                           Class MeshInterpolationOrder Declaration
\*---------------------------------------------------------------------------*/

class MeshInterpolationOrder
{
    // Private data

public:

    typedef meshToMesh::order value;

    //- String representation of order enums
#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    static const Enum<value> names;
#else
    static const NamedEnum<value, 3> names;
#endif

};

typedef meshToMesh::order meshToMeshOrder;
#define meshToMeshInterpolationNames MeshInterpolationOrder::names

#else

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

typedef meshToMesh::interpolationMethod meshToMeshOrder;
#define meshToMeshInterpolationNames meshToMesh::interpolationMethodNames_

#endif

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
