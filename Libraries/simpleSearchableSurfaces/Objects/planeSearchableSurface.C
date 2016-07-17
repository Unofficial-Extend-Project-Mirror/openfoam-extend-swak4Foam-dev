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
    2009, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "planeSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(planeSearchableSurface, 0);
addToRunTimeSelectionTable(searchableSurface, planeSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::pointIndexHit Foam::planeSearchableSurface::findNearest
(
    const point& sample,
    const scalar nearestDistSqr
) const
{
    pointIndexHit info(false, sample, -1);

    point pt=plane_.nearestPoint(sample);

    if (nearestDistSqr > magSqr(pt-sample))
    {
        info.rawPoint() = pt;
        info.setHit();
        info.setIndex(0);
    }

    return info;
}


void Foam::planeSearchableSurface::findLineAll
(
    const point& start,
    const point& end,
    pointIndexHit& hit
) const
{
    hit.setMiss();

    vector dir=end-start;

    scalar f=plane_.normalIntersect(start,dir);
    if(0<=f && f<=1) {
        hit.rawPoint()=start+f*dir;
        hit.setHit();
        hit.setIndex(0);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::planeSearchableSurface::planeSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    searchableSurface(io),
    plane_(dict.subDict("plane"))
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::planeSearchableSurface::~planeSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList& Foam::planeSearchableSurface::regions() const
{
    if (regions_.size() == 0)
    {
        regions_.setSize(1);
        regions_[0] = "region0";
    }
    return regions_;
}



void Foam::planeSearchableSurface::findNearest
(
    const pointField& samples,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& info
) const
{
    info.setSize(samples.size());

    forAll(samples, i)
    {
        info[i] = findNearest(samples[i], nearestDistSqr[i]);
    }
}


void Foam::planeSearchableSurface::findLine
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    info.setSize(start.size());

    forAll(start, i)
    {
        findLineAll(start[i], end[i], info[i]);
    }
}


void Foam::planeSearchableSurface::findLineAny
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    info.setSize(start.size());

    forAll(start, i)
    {
        findLineAll(start[i], end[i], info[i]);
    }
}


void Foam::planeSearchableSurface::findLineAll
(
    const pointField& start,
    const pointField& end,
    List<List<pointIndexHit> >& info
) const
{
    info.setSize(start.size());

    forAll(start, i)
    {
        info[i].setSize(1);
        findLineAll(start[i], end[i], info[i][0]);
    }
}


void Foam::planeSearchableSurface::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    region.setSize(info.size());
    region = 0;
}


void Foam::planeSearchableSurface::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    normal.setSize(info.size());
    normal = plane_.normal();
}


void Foam::planeSearchableSurface::getVolumeType
(
    const pointField& points,
    List<volumeType>& volType
) const
{
    volType.setSize(points.size());
    volType = INSIDE;

    forAll(points, pointI)
    {
        const point& pt = points[pointI];

        if ( ((pt - plane_.refPoint()) & plane_.normal()) < 0 )
        {
            volType[pointI] = INSIDE;
        }
        else
        {
            volType[pointI] = OUTSIDE;
        }
    }
}

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
Foam::tmp<Foam::pointField>
#else
Foam::pointField
#endif
Foam::planeSearchableSurface::coordinates() const
{
#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
    return tmp<pointField>(new pointField(1,plane_.refPoint()));
#else
    return pointField(1,plane_.refPoint());
#endif
}

Foam::tmp<Foam::pointField> Foam::planeSearchableSurface::points() const
{
    return coordinates();
}

bool Foam::planeSearchableSurface::overlaps(const boundBox& bb) const
{
    notImplemented
        (
            "Foam::planeSearchableSurface::overlaps(const boundBox&) const"
        );

    return false;
}

#ifdef FOAM_SEARCHABLE_SURF_NEEDS_BOUNDING_SPHERES
void Foam::planeSearchableSurface::boundingSpheres
(
    pointField& centres,
    scalarField& radiusSqr
) const
{
    centres.setSize(1);
    centres[0] = plane_.refPoint();

    radiusSqr.setSize(1);
    radiusSqr[0] = Foam::sqr(GREAT);
}
#endif

// ************************************************************************* //
