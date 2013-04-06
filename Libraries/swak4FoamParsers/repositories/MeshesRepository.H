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
    Foam::MeshesRepository

Description

SourceFiles
    MeshesRepositoryI.H
    MeshesRepository.C
    MeshesRepositoryIO.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef MeshesRepository_H
#define MeshesRepository_H

#include "PtrMap.H"
#include "fvMesh.H"
#include "meshToMesh.H"

#include "MeshInterpolationOrder.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class MeshesRepository Declaration
\*---------------------------------------------------------------------------*/

class MeshesRepository
{
    // Private data

    HashPtrTable<Time,word> times_;

    HashPtrTable<fvMesh,word> meshes_;

    HashPtrTable<meshToMesh,word> meshInterpolations_;

    HashTable<meshToMesh::order,word> interpolationOrder_;

    // Constructors

    //- Construct null
    MeshesRepository();

    //- The only instance of this repository
    static MeshesRepository *repositoryInstance;

public:
     //- Runtime type information
    TypeName("MeshesRepository");

    //- Destructor

    virtual ~MeshesRepository();

    //- Get the Singleton of this class
    static MeshesRepository &getRepository();

    //- add a mesh and construct a new time for it
    fvMesh &addMesh(
        const word &name,
        const fileName &caseName,
        const word &region=polyMesh::defaultRegion
    );

    //- add a mesh that is in the same case as another mesh
    fvMesh &addCoupledMesh(
        const word &name,
        const word &masterName,
        const word &region=polyMesh::defaultRegion
    );

    //- get a set using its name (mesh must exist)
    fvMesh &getMesh(
        const word &name
    );

    //- does the mesh exist
    bool hasMesh(
        const word &name
    );

    //- get a mesh to mesh interpolation from mesh name to the other mesg
    meshToMesh &getMeshToMesh(
        const word &name,
        const fvMesh &mesh
    );

    //- set the time that is used for a mesh
    scalar setTime(
        const word &name,
        const string &time,
        label timeIndex=0
    );

    //- set the time that is used for a mesh
    scalar setTime(
        const word &name,
        scalar time,
        label timeIndex=0
    );

    //- set interpolation order for a mesh
    void setInterpolationOrder(
        const word &name,
        meshToMesh::order val
    );

    //- get interpolation order
    meshToMesh::order getInterpolationOrder(
        const word &name
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
