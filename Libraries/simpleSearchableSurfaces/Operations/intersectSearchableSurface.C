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

#include "intersectSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"
#include "transform.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(intersectSearchableSurface, 0);
addToRunTimeSelectionTable(searchableSurface, intersectSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::intersectSearchableSurface::intersectSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    binaryOperationSearchableSurface(io,dict)
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::intersectSearchableSurface::~intersectSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::intersectSearchableSurface::filter
(
    const List<pointIndexHit>& hitsA,
    const List<pointIndexHit>& hitsB,
    List<pointIndexHit>& result
) const
{
    List<bool> inA;
    List<bool> inB;

    insideA(hitsB,inA);
    insideB(hitsA,inB);
    
    label nr=0;
    forAll(inA,i) {
        if(inA[i]) {
            nr++;
        }
    }
    forAll(inB,i) {
        if(inB[i]) {
            nr++;
        }
    }

    result.setSize(nr);
    label cnt=0;
    forAll(hitsA,i) {
        if(inB[i]) {
            result[cnt]=hitsA[i];
            cnt++;
        }
    }
    forAll(hitsB,i) {
        if(inA[i]) {
            result[cnt]=hitsB[i];
            cnt++;
        }
    }
    if(cnt!=nr) {
        FatalErrorIn("Foam::intersectSearchableSurface::filter")
            << "Something went horribly wrong. Number " << cnt
                << "different from expected number " << nr
                << endl
                << abort(FatalError);
    }
}

void Foam::intersectSearchableSurface::findNearest
(
    const pointField& sample,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& result
) const
{
    List<pointIndexHit> hitA;
    List<pointIndexHit> hitB;
    a().findNearest(sample,nearestDistSqr,hitA);
    b().findNearest(sample,nearestDistSqr,hitB);

    List<bool> inA;
    List<bool> inB;
    insideA(hitB,inA);
    insideB(hitA,inB);

    result.setSize(sample.size());

    forAll(result,i) {
        if(inA[i] && !inB[i]) {
            result[i]=hitB[i];
        } else if(!inA[i] && inB[i]) {
            result[i]=hitA[i];
        } else {
            // not sure
            if
                (
                    mag(sample[i]-hitA[i].rawPoint()) 
                    < 
                    mag(sample[i]-hitB[i].rawPoint())
                ) {
                result[i]=hitA[i];
            } else {
                result[i]=hitB[i];
            }
        }
    }
}
    
void Foam::intersectSearchableSurface::getVolumeType
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
        if( inA[i]==INSIDE && inB[i]==INSIDE ) {
            volType[i]=INSIDE;
        } else {
            volType[i]=OUTSIDE;
        }
    }
}

// ************************************************************************* //
