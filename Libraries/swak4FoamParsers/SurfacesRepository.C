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

#include "SurfacesRepository.H"

namespace Foam {
 
// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(SurfacesRepository, 0);

SurfacesRepository *SurfacesRepository::repositoryInstance(NULL);

SurfacesRepository::SurfacesRepository()
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SurfacesRepository::~SurfacesRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

SurfacesRepository &SurfacesRepository::getRepository()
{
    SurfacesRepository*  ptr=repositoryInstance;
    
    if(debug) {
        Info << "SurfacesRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Info << "swak4Foam: Allocating new repository for sampledSurfaces\n";

        ptr=new SurfacesRepository();
    }
    
    repositoryInstance=ptr;

    return *repositoryInstance;
}

sampledSurface &SurfacesRepository::getSurface(
    const word &name,
    const fvMesh &mesh
)
{
    if(debug) {
        Info << "SurfacesRepository: Lookuing up" << name << " (assuming it exists)" << endl;
    }

    sampledSurface &found=*(surfaces_[name]);
        
    if((&mesh)!=(&(found.mesh()))) {
        FatalErrorIn("SampledSurface &SurfacesRepository::getSurface")
            << "Found a mesh named " << name << " which is not for the mesh "
                << mesh.name() << "but for the mesh " << found.mesh().name()
                << endl
                << abort(FatalError);
    }

    return found;
}

sampledSurface &SurfacesRepository::getSurface(
    const dictionary &dict,
    const fvMesh &mesh
)
{
    word name(dict.lookup("surfaceName"));

    if(debug) {
        Info << "SurfacesRepository: getting surface " << name << endl;
    }

    if(surfaces_.found(name)) {
        if(debug) {
            Info << "SurfacesRepository: " << name << " already exists" << endl;
        }
        
        if(dict.found("surface")) {
            WarningIn("SampledSurface &SurfacesRepository::getSurface")
                << "Already got a surface named " << name 
                    << ". There is a specification for the surface here "
                    << "which is ignored. It is: " << endl
                    << dict.subDict("surface") << endl;
        }
        
        return getSurface(name,mesh);
    } else {
        if(debug) {
            Info << "SurfacesRepository: " << name << " does not exist" << endl;
        }
        surfaces_.insert(
            name,
            sampledSurface::New(
                name,
                mesh,
                dict.subDict("surface")
            ).ptr()
        );

        return *surfaces_[name];
    }
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
