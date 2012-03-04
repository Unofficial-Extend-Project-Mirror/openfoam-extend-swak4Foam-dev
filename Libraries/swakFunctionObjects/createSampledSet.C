//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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

\*---------------------------------------------------------------------------*/

#include "createSampledSet.H"

#include "SetsRepository.H"

#include "polyMesh.H"

namespace Foam {
    defineTypeNameAndDebug(createSampledSet,0);
}

Foam::createSampledSet::createSampledSet
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    active_(true),
    obr_(obr)
{
    if (!isA<polyMesh>(obr))
    {
        active_=false;
        WarningIn("createSampledSet::createSampledSet")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    execute();
}

Foam::createSampledSet::~createSampledSet()
{}

void Foam::createSampledSet::read(const dictionary& dict)
{
    if(active_) {
        SetsRepository::getRepository().getSet(
            dict,
            dynamic_cast<const polyMesh &>(obr_)
        );
    }
}

void Foam::createSampledSet::execute()
{
}


void Foam::createSampledSet::end()
{
}

void Foam::createSampledSet::write()
{
}

void Foam::createSampledSet::clearData()
{
}

// ************************************************************************* //
