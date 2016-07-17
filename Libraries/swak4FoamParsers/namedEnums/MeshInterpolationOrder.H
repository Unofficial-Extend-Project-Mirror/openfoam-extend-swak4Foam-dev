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
    Foam::MeshInterpolationOrder

Description

SourceFiles
    MeshInterpolationOrder.C

Contributors/Copyright:
    2012-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef MeshInterpolationOrder_H
#define MeshInterpolationOrder_H

#include "swak.H"

#include "meshToMesh.H"

#include "NamedEnum.H"

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
    static const NamedEnum<value, 3> names;

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
