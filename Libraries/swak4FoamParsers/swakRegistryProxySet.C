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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "swakRegistryProxySet.H"
#include "dictionary.H"
#include "volFields.H"
#include "volPointInterpolation.H"
#include "addToRunTimeSelectionTable.H"
#include "fvMesh.H"

#include "SetsRepository.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(swakRegistryProxySet, 0);
    addToRunTimeSelectionTable(sampledSet, swakRegistryProxySet, word);
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// void Foam::swakRegistryProxySet::createGeometry()
// {
//     if (debug)
//     {
//         Info<< "swakRegistryProxySet::createGeometry() - doing nothing" 
//             << endl;
//     }
// }

Foam::sampledSet &Foam::swakRegistryProxySet::realSet()
{
    return SetsRepository::getRepository().getSet(
        setName_,
        static_cast<const fvMesh&>(mesh())
    );
}

const Foam::sampledSet &Foam::swakRegistryProxySet::realSet() const
{
    return SetsRepository::getRepository().getSet(
        setName_,
        static_cast<const fvMesh&>(mesh())
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::swakRegistryProxySet::swakRegistryProxySet
(
    const word& name,
    const polyMesh& mesh,
    meshSearch& search,
    const dictionary& dict
)
:
    sampledSet(
        name, 
        mesh, 
        search,
        dict
    ),
    setName_(dict.lookup("setName"))
{
    setSamples(
        realSet(),
        realSet().cells(),
        realSet().faces(),
        realSet().segments(),
        realSet().curveDist()        
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::swakRegistryProxySet::~swakRegistryProxySet()
{}

// Foam::point Foam::swakRegistryProxySet::getRefPoint (const List< point > &pl) const
// {
//     return realSet().getRefPoint(pl);
// }

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


// ************************************************************************* //
