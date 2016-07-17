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
    \\  /    A nd           | Copyright held by original author
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
    2009, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "exclusiveSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"
#include "transform.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(exclusiveSearchableSurface, 0);
addToRunTimeSelectionTable(searchableSurface, exclusiveSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::exclusiveSearchableSurface::exclusiveSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    binaryOperationSearchableSurface(io,dict)
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::exclusiveSearchableSurface::~exclusiveSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::exclusiveSearchableSurface::decidePoint(
    const hitWhom who,
    const bool inA,
    const bool inB
) const
{
    if(
        who!=BOTH
    ) {
        return true;
    }
    return false;
}

void Foam::exclusiveSearchableSurface::getVolumeType
(
    const pointField& points,
    List<volumeType>& volType
) const
{
    List<volumeType> inA;
    List<volumeType> inB;

    a().getVolumeType(points,inA);
    b().getVolumeType(points,inB);

    volType.setSize(points.size());

    forAll(volType,i) {
        if
            (
                ( inA[i]==INSIDE && inB[i]==OUTSIDE )
                ||
                ( inA[i]==OUTSIDE && inB[i]==INSIDE )
            ) {
            volType[i]=INSIDE;
        } else if( inA[i]==UNKNOWN || inB[i]==UNKNOWN) {
            volType[i]=UNKNOWN;
        } else {
            volType[i]=OUTSIDE;
        }
    }
}

bool Foam::exclusiveSearchableSurface::revertNormalA(const pointIndexHit& h) const
{
    pointField pt(1,h.rawPoint());
    List<volumeType> inside;

    b().getVolumeType(pt,inside);

    return inside[0]==INSIDE;
}

bool Foam::exclusiveSearchableSurface::revertNormalB(const pointIndexHit& h) const
{
    pointField pt(1,h.rawPoint());
    List<volumeType> inside;

    a().getVolumeType(pt,inside);

    return inside[0]==INSIDE;
}

// ************************************************************************* //
