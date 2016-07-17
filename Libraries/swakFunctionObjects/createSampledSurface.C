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
    \\  /    A nd           | Copyright  held by original author
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
    2010, 2013-2014, 2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "createSampledSurface.H"

#include "SurfacesRepository.H"

namespace Foam {
    defineTypeNameAndDebug(createSampledSurface,0);
}

Foam::createSampledSurface::createSampledSurface
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    active_(true),
    obr_(obr),
    surfaceName_("noSurfaceHereIn_"+name)
{
    if (!isA<fvMesh>(obr))
    {
        active_=false;
        WarningIn("createSampledSurface::createSampledSurface")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    execute();
}

Foam::createSampledSurface::~createSampledSurface()
{}

void Foam::createSampledSurface::timeSet()
{
    // Do nothing
}

void Foam::createSampledSurface::read(const dictionary& dict)
{
    if(active_) {
        surfaceName_=word(dict.lookup("surfaceName"));

        SurfacesRepository::getRepository(obr_).getSurface(
            dict,
            dynamic_cast<const fvMesh &>(obr_)
        );
    }
}

void Foam::createSampledSurface::execute()
{
}


void Foam::createSampledSurface::end()
{
}

void Foam::createSampledSurface::write()
{
}

void Foam::createSampledSurface::clearData()
{
}

//- Update for changes of mesh
void Foam::createSampledSurface::updateMesh(const mapPolyMesh&)
{
    redoSurface();
}

//- Update for changes of mesh
#ifdef FOAM_MOVEPOINTS_GETS_POLYMESH
void Foam::createSampledSurface::movePoints(const polyMesh&)
#else
void Foam::createSampledSurface::movePoints(const pointField&)
#endif
{
    redoSurface();
}

void Foam::createSampledSurface::redoSurface()
{
    Info << "Forcing regeneration of surface " << surfaceName_ << endl;

    bool status=SurfacesRepository::getRepository(obr_).updateSurface(
        surfaceName_,
        dynamic_cast<const fvMesh &>(obr_)
    );
    Info << "Forcing status for " << surfaceName_ << ": " << status << endl;
}

// ************************************************************************* //
