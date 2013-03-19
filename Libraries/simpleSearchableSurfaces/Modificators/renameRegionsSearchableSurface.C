/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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

#include "renameRegionsSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"
#include "Pair.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(renameRegionsSearchableSurface, 0);
    addToRunTimeSelectionTable(searchableSurface, renameRegionsSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::renameRegionsSearchableSurface::renameRegionsSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    wrapperSearchableSurface(io,dict)
{
    regions_=delegate().regions();
    List<Pair<word> > replace(dict.lookup("replacementPairs"));

    forAll(replace,i) {
        forAll(regions_,j) {
            regions_[j].replaceAll
                (
                    replace[i].first(),
                    replace[i].second()
                );
        }
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::renameRegionsSearchableSurface::~renameRegionsSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


const Foam::wordList& Foam::renameRegionsSearchableSurface::regions() const
{
    return regions_;
}



// ************************************************************************* //
