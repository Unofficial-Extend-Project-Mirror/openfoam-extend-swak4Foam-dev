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

#include "wrapperSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"
#include "SortableList.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(wrapperSearchableSurface, 0);
    // addToRunTimeSelectionTable(searchableSurface, wrapperSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::wrapperSearchableSurface::wrapperSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    searchableSurface(io),
    delegate_(
        searchableSurface::New
        (
            word(dict.subDict("surface").lookup("type")),
            *this,
            dict.subDict("surface")
        )
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::wrapperSearchableSurface::~wrapperSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList& Foam::wrapperSearchableSurface::regions() const
{
    return delegate().regions();
}


void Foam::wrapperSearchableSurface::findNearest
(
    const pointField& samples,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& info
) const
{
    delegate().findNearest
        (
            samples,
            nearestDistSqr,
            info
        );
}


void Foam::wrapperSearchableSurface::findLine
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    delegate().findLine
        (
            start,
            end,
            info
        );
}


void Foam::wrapperSearchableSurface::findLineAny
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    delegate().findLineAny
        (
            start,
            end,
            info
        );
}


void Foam::wrapperSearchableSurface::findLineAll
(
    const pointField& start,
    const pointField& end,
    List<List<pointIndexHit> >& info
) const
{
    delegate().findLineAll
        (
            start,
            end,
            info
        );
}


void Foam::wrapperSearchableSurface::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    delegate().getRegion
        (
            info,
            region
        );
}


void Foam::wrapperSearchableSurface::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    delegate().getNormal
        (
            info,
            normal
        );
}


void Foam::wrapperSearchableSurface::getVolumeType
(
    const pointField& points,
    List<volumeType>& volType
) const
{
    delegate().getVolumeType
        (
            points,
            volType
        );
}


// ************************************************************************* //
