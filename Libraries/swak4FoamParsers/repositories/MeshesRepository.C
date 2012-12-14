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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "MeshesRepository.H"

#include "polyMesh.H"
#include "meshSearch.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(MeshesRepository, 0);

MeshesRepository *MeshesRepository::repositoryInstance(NULL);

MeshesRepository::MeshesRepository()
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

MeshesRepository::~MeshesRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

MeshesRepository &MeshesRepository::getRepository()
{
    MeshesRepository*  ptr=repositoryInstance;

    if(debug) {
        Pout << "MeshesRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Pout << "swak4Foam: Allocating new repository for sampledMeshes\n";

        ptr=new MeshesRepository();
    }

    repositoryInstance=ptr;

    return *repositoryInstance;
}

fvMesh &MeshesRepository::addMesh(
    const word &name,
    const fileName &caseN,
    const word &region
)
{
    fileName usedN=caseN;
    usedN.expand();

    if(debug) {
        Pout << "MeshesRepository: Adding" << name
            << " (assuming it does not exist)" << endl;
    }

    typedef  HashPtrTable<Time,word> tableIterator;

    forAllIter(
        tableIterator,
        times_,
        iter
    ) {
        const Time &time=(*iter());
        fileName meshPath=time.rootPath()/time.caseName();

        if(
            usedN==meshPath
            &&
            region==time.dbDir()
        ) {
            FatalErrorIn("MeshesRepository::addMesh")
                << "Mesh of case " << usedN << " region " << region
                    << " is already in the repository under the name "
                    << iter.key()
                    << endl
                    << exit(FatalError);

        }
    }

    if(meshes_.found(name)) {
        FatalErrorIn("MeshesRepository::addMesh")
            << "There is already an entry " << name
                << " in the repository"
                << endl
                << exit(FatalError);

    }

    times_.insert(
        name,
        new Time(
            Time::controlDictName,
            usedN.path(),
            fileName(usedN.name())
        )
    );
    meshes_.insert(
        name,
        new fvMesh(
            IOobject(
                region,
                (*times_[name]).timeName(),
                (*times_[name]),
                Foam::IOobject::MUST_READ
            )
        )
    );

    return *meshes_[name];
}

fvMesh &MeshesRepository::getMesh(
    const word &name
)
{
    return *meshes_[name];
}

bool MeshesRepository::hasMesh(
    const word &name
)
{
    return meshes_.found(name);
}

meshToMesh &MeshesRepository::getMeshToMesh(
    const word &name,
    const fvMesh &mesh
)
{
    if(!meshInterpolations_.found(name)) {
         meshInterpolations_.insert(
             name,
             new meshToMesh(
                 *meshes_[name],
                 mesh
             )
         );
    }

    return *meshInterpolations_[name];
}

scalar MeshesRepository::setTime(
    const word &name,
    const string &time
) {
    IStringStream tStream(time);

    return setTime(
        name,
        readScalar(tStream)
    );
}

scalar MeshesRepository::setTime(
    const word &name,
    scalar time
) {
    fvMesh &mesh=getMesh(name);

    instant iTime=mesh.time().findClosestTime(time);

    const_cast<Time&>(mesh.time()).setTime(
        iTime,
        0 // whatever
    );
    return mesh.time().value();
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
