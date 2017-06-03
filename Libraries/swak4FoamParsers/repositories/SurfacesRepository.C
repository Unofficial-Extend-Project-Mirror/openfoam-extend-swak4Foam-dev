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

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2016 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"

#include "SurfacesRepository.H"
#include "surfaceWriter.H"

namespace Foam {

#ifdef FOAM_SURFACEWRITER_NOT_A_TEMPLATE
typedef surfaceWriter scalarSurfaceWriter;
#else
typedef surfaceWriter<scalar> scalarSurfaceWriter;
#endif

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(SurfacesRepository, 0);

SurfacesRepository *SurfacesRepository::repositoryInstance(NULL);

SurfacesRepository::SurfacesRepository(const IOobject &o)
    :
    RepositoryBase(o)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SurfacesRepository::~SurfacesRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

SurfacesRepository &SurfacesRepository::getRepository(const objectRegistry &obr)
{
    SurfacesRepository*  ptr=repositoryInstance;

    if(debug) {
        Pout << "SurfacesRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Pout << "swak4Foam: Allocating new repository for sampledSurfaces\n";

        ptr=new SurfacesRepository(
            IOobject(
                "swakSurfaces",
                obr.time().timeName(),
                "surfaceRepository",
                obr.time(),
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            )
        );
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
        Pout << "SurfacesRepository: Lookuing up" << name << " (assuming it exists)" << endl;
    }

    sampledSurface &found=*(surfaces_[name]);

    if((&mesh)!=(&(found.mesh()))) {
        FatalErrorIn("SampledSurface &SurfacesRepository::getSurface")
            << "Found a mesh named " << name << " which is not for the mesh "
                << mesh.name() << "but for the mesh " << found.mesh().name()
                << endl
                << exit(FatalError);
    }

    return found;
}

bool SurfacesRepository::updateSurface(
    const word &name,
    const fvMesh &mesh
) {
    sampledSurface &surf=getSurface(name,mesh);
    bool expired=surf.expire();
    bool updated=surf.update();
    Pbug << "Surface " << name << " Expired: " << expired << " Updated: " << updated << endl;
    return expired && updated;
}


sampledSurface &SurfacesRepository::getSurface(
    const dictionary &dict,
    const fvMesh &mesh
)
{
    word name(dict.lookup("surfaceName"));

    if(debug) {
        Pout << "SurfacesRepository: getting surface " << name << endl;
    }

    if(surfaces_.found(name)) {
        if(debug) {
            Pout << "SurfacesRepository: " << name << " already exists" << endl;
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
            Pout << "SurfacesRepository: " << name << " does not exist" << endl;
        }
        surfaces_.insert(
            name,
            sampledSurface::New(
                name,
                mesh,
                dict.subDict("surface")
            ).ptr()
        );

        bool writeSurface=dict.lookupOrDefault<bool>("autoWriteSurface",false);
        bool writeSurfaceNow=dict.lookupOrDefault<bool>("writeSurfaceOnConstruction",false);
        if(
            writeSurface
            ||
            writeSurfaceNow
        ) {
            word format(dict.lookup("surfaceFormat"));

            // Just to check whether the format actually exists
            autoPtr<scalarSurfaceWriter> theWriter(
                scalarSurfaceWriter::New(format)
            );

            if(writeSurface) {
                formatNames_.insert(name,format);
            }
            if(writeSurfaceNow) {
                Info << "Writing surface " << name << endl;

                sampledSurface &surf=*surfaces_[name];
                surf.update();

                theWriter->write
                (
                    this->path(),
                    name+"_geometry_AtCreation",
#if OPENFOAM_PLUS >= 1612
                    meshedSurfRef(surf.points(), surf.faces())
#else
                    surf.points(), surf.faces()
#endif
                );
            }
        }

        return *surfaces_[name];
    }
}

bool SurfacesRepository::writeData(Ostream &f) const
{
    if(debug) {
        Info << "SurfacesRepository::write()" << endl;
    }

    f << surfaces_;

    typedef HashTable<word,word> wordWord;

    forAllConstIter(wordWord,formatNames_,it) {
        const word &name=it.key();
        const word &format=it();

        const sampledSurface &surf=*surfaces_[name];

        autoPtr<scalarSurfaceWriter> theWriter(
            scalarSurfaceWriter::New(format)
        );

        theWriter->write
        (
            f.name().path(),
            name+"_geometry",
#if OPENFOAM_PLUS >= 1612
            meshedSurfRef(surf.points(), surf.faces())
#else
            surf.points(), surf.faces()
#endif
        );
    }

    return true;
}

void SurfacesRepository::updateRepo()
{
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
