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
    2009, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
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

void Foam::unionSearchableSurface::filter
(
    const point &start,
    const List<pointIndexHit>& hitsA,
    const List<pointIndexHit>& hitsB,
    List<pointIndexHit>& result
) const
{
    if(debug) {
        Info << "Foam::unionSearchableSurface::filter" << endl;
    }

    List<bool> inA;
    List<bool> inB;
    List<hitWhom> whom;
    List<pointIndexHit> hits;
    collectInfo(
        start,
        hitsA,
        hitsB,
        hits,
        inA,
        inB,
        whom
    );

    DynamicList<pointIndexHit> h;
    forAll(hits,i) {
        if(
            whom[i]==BOTH
        ) {
            h.append(hits[i]);
        } else if(
            (!inA[i] && whom[i]==HITSB)
            ||
            (!inB[i] && whom[i]==HITSA)
        ) {
            h.append(hits[i]);
        }
    }
    result=h;
}

void Foam::unionSearchableSurface::findNearest
(
    const pointField& sample,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& result
) const
{
    if(debug) {
        Info << "Foam::unionSearchableSurface::findNearest" << endl;
    }

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
            result[i]=hitA[i];
        } else if(!inA[i] && inB[i]) {
            result[i]=hitB[i];
        } else {
            // not sure
            if(hitA[i].hit() == hitB[i].hit()) {
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
            } else {
                if(hitA[i].hit()) {
                    result[i]=hitA[i];
                } else {
                    result[i]=hitB[i];
                }
            }
            if(inA[i] && inB[i]) {
                result[i].setMiss();
            }
        }
    }
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
