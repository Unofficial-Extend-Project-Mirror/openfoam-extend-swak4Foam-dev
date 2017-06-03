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
    2009, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "unionSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"
#include "transform.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(unionSearchableSurface, 0);
addToRunTimeSelectionTable(searchableSurface, unionSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::unionSearchableSurface::unionSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    binaryOperationSearchableSurface(io,dict)
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::unionSearchableSurface::~unionSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::unionSearchableSurface::decidePoint(
    const hitWhom who,
    const bool inA,
    const bool inB
) const {
    if(
        who==BOTH
    ) {
        return true;
    } else if(
        (!inA && who==HITSB)
        ||
        (!inB && who==HITSA)
    ) {
        return true;
    }

    return false;
}

void Foam::unionSearchableSurface::getVolumeType
(
    const pointField& points,
    List<volumeType>& volType
) const
{
    if(debug) {
        Info << "Foam::unionSearchableSurface::getVolumeType" << endl;
    }

    List<volumeType> inA;
    List<volumeType> inB;

    a().getVolumeType(points,inA);
    b().getVolumeType(points,inB);

    volType.setSize(points.size());

    forAll(volType,i) {
        if( inA[i]==INSIDE || inB[i]==INSIDE ) {
            volType[i]=INSIDE;
        } else if( inA[i]==OUTSIDE && inB[i]==OUTSIDE ) {
            volType[i]=OUTSIDE;
        } else {
            volType[i]=UNKNOWN;
        }
        if(debug) {
            Info << "Point: " << points[i] << " A: " << inA[i]
                << " B: " << inB[i]  << " -> " << volType[i] << endl;
        }
    }
}

// ************************************************************************* //
