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

#include "SetsRepository.H"

#include "polyMesh.H"
#include "meshSearch.H"

namespace Foam {
 
// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(SetsRepository, 0);

SetsRepository *SetsRepository::repositoryInstance(NULL);

SetsRepository::SetsRepository()
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SetsRepository::~SetsRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

SetsRepository &SetsRepository::getRepository()
{
    SetsRepository*  ptr=repositoryInstance;
    
    if(debug) {
        Info << "SetsRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Info << "swak4Foam: Allocating new repository for sampledSets\n";

        ptr=new SetsRepository();
    }
    
    repositoryInstance=ptr;

    return *repositoryInstance;
}

sampledSet &SetsRepository::getSet(
    const word &name,
    const polyMesh &mesh
)
{
    if(debug) {
        Info << "SetsRepository: Lookuing up" << name << " (assuming it exists)" << endl;
    }

    sampledSet &found=*(sets_[name]);
        
    if((&mesh)!=(&(found.mesh()))) {
        FatalErrorIn("SampledSet &SetsRepository::getSet")
            << "Found a mesh named " << name << " which is not for the mesh "
                << mesh.name() << "but for the mesh " << found.mesh().name()
                << endl
                << abort(FatalError);
    }

    return found;
}

sampledSet &SetsRepository::getSet(
    const dictionary &dict,
    const polyMesh &mesh
)
{
    word name(dict.lookup("setName"));

    if(debug) {
        Info << "SetsRepository: getting set " << name << endl;
    }

    if(sets_.found(name)) {
        if(debug) {
            Info << "SetsRepository: " << name << " already exists" << endl;
        }
        
        if(dict.found("set")) {
            WarningIn("SampledSet &SetsRepository::getSet")
                << "Already got a set named " << name 
                    << ". There is a specification for the set here "
                    << "which is ignored. It is: " << endl
                    << dict.subDict("set") << endl;
        }
        
        return getSet(name,mesh);
    } else {
        if(debug) {
            Info << "SetsRepository: " << name << " does not exist" << endl;
        }
        sets_.insert(
            name,
            sampledSet::New(
                name,
                mesh,
                getSearch(mesh),
                dict.subDict("set")
            ).ptr()
        );

        if(debug) {
            Info << "Created set " << name << " :" << endl;
            sets_[name]->write(Info);
            Info << endl;
        }
        return *sets_[name];
    }
}

meshSearch &SetsRepository::getSearch(
    const polyMesh &mesh
)
{
    word name(mesh.name());

    if(debug) {
        Info << "SetsRepository: getting meshSearch " << name << endl;
    }

    if(meshSearches_.found(name)) {
        if(debug) {
            Info << "SetsRepository: meshSearch for mesh " << name << " already exists" << endl;
        }
        
        meshSearch &found=*(meshSearches_[name]);
        
        if((&mesh)!=(&(found.mesh()))) {
        FatalErrorIn("SampledSet &SetsRepository::getSearch")
            << "Found a mesh named " << name << " which is not for the mesh "
                << mesh.name() << "but for the mesh " << found.mesh().name()
                << endl
                << abort(FatalError);
        }

        return found;
    } else {
        if(debug) {
            Info << "SetsRepository: meshSearch for mesh " << name << " does not exist" << endl;
        }
        meshSearches_.insert(
            name,
            new meshSearch(mesh)
        );

        return *meshSearches_[name];
    }
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
