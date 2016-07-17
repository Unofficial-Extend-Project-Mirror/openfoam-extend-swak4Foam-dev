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

#include "unitCylinderSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(unitCylinderSearchableSurface, 0);
addToRunTimeSelectionTable(searchableSurface, unitCylinderSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::pointIndexHit Foam::unitCylinderSearchableSurface::findNearest
(
    const point& sample,
    const scalar nearestDistSqr
) const
{
    pointIndexHit info(false, sample, -1);

    List<point> pts(3);
    List<bool>  hit(3,false);

    point &topPt=pts[0];
    topPt=top_.nearestPoint(sample);
    scalar topRadius=sqrt(topPt.x()*topPt.x()+topPt.y()*topPt.y());
    if(topRadius>1) {
        topPt.x()/=topRadius;
        topPt.y()/=topRadius;
    } else {
        hit[0]=false;
    }
    point &bottomPt=pts[1];
    bottomPt=bottom_.nearestPoint(sample);
    scalar bottomRadius=sqrt(bottomPt.x()*bottomPt.x()+bottomPt.y()*bottomPt.y());
    if(bottomRadius>1) {
        bottomPt.x()/=bottomRadius;
        bottomPt.y()/=bottomRadius;
    } else {
        hit[1]=false;
    }

    scalar r=max(
        sqrt(sample.x()*sample.x()+sample.y()*sample.y()),
        SMALL
    );

    point &hullPoint=pts[2];
    hullPoint=point(sample.x()/r,sample.y()/r,sample.z());
    if(hullPoint.z()>1) {
        hullPoint.z()=1;
    } else if(hullPoint.z()<-1) {
        hullPoint.z()=-1;
    } else {
        hit[2]=true;
    }

    scalar minDist=HUGE;
    label minIndex=-1;
    forAll(pts,i) {
        if(mag(sample-pts[i])<minDist) {
            minDist=mag(sample-pts[i]);
            minIndex=i;
        }
    }

    if (nearestDistSqr > minDist)
    {
        info.rawPoint() = pts[minIndex];
        info.setHit();
        info.setIndex(minIndex);
    }

    return info;
}


void Foam::unitCylinderSearchableSurface::findLineAll
(
    const point& start,
    const point& end,
    List<pointIndexHit>& hit
) const
{
    List<scalar> f(4,HUGE);

    vector dir=end-start;

    f[0]=max(min(top_.normalIntersect(start,dir),HUGE),-HUGE);
    point pt0=start+f[0]*dir;
    pt0.z()=0;
    if(magSqr(pt0)>1) {
        f[0]=HUGE;
    }

    f[1]=max(min(bottom_.normalIntersect(start,dir),HUGE),-HUGE);
    point pt1=start+f[1]*dir;
    pt1.z()=0;
    if(magSqr(pt1)>1) {
        f[1]=HUGE;
    }

    {
        scalar a=min(dir.x()*dir.x()+dir.y()*dir.y(),SMALL);
        scalar b=2*(start.x()*dir.x()+start.y()*dir.y());
        scalar c=start.x()*start.x()+start.y()*start.y()-1;
        scalar D=b*b-4*a*c;
        if(D>SMALL) {
            f[2]=(-b-sqrt(D))/(2*a);
            f[3]=(-b+sqrt(D))/(2*a);
        } else if(D>0) {
            f[2]=(-b)/(2*a);
        }
        scalar z2=start.z()+f[2]*dir.z();
        if(z2<-1 || z2>1) {
            f[2]=HUGE;
        }
        scalar z3=start.z()+f[3]*dir.z();
        if(z3<-1 || z3>1) {
            f[3]=HUGE;
        }
    }

    label nr=0;
    forAll(f,i) {
        if(0<=f[i] && f[i]<=1) {
            nr++;
        }
    }

    hit.setSize(nr);
    label cnt=0;
    forAll(f,i) {
        if(0<=f[i] && f[i]<=1) {
            hit[cnt].rawPoint()=start+f[i]*dir;
            hit[cnt].setHit();
            hit[cnt].setIndex( i<3 ? i : 2 );
            cnt++;
        }
    }

    if(cnt!=nr) {
        FatalErrorIn("Foam::unitCylinderSearchableSurface::findLineAll")
            << "Expected number of hits " << nr << " does not match the actual "
                << cnt << endl
                << abort(FatalError);
    }

    for(label i=0;i<nr-1;i++) {
        for(label j=i+1;j<nr;j++) {
            if(mag(start-hit[i].rawPoint())>mag(start-hit[j].rawPoint())) {
                pointIndexHit tmp=hit[i];
                hit[i]=hit[j];
                hit[j]=tmp;
            }
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::unitCylinderSearchableSurface::unitCylinderSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    searchableSurface(io),
    top_(point(0,0,1),vector(0,0,1)),
    bottom_(point(0,0,-1),vector(0,0,-1))
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::unitCylinderSearchableSurface::~unitCylinderSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList& Foam::unitCylinderSearchableSurface::regions() const
{
    if (regions_.size() == 0)
    {
        regions_.setSize(3);
        regions_[0] = "top";
        regions_[1] = "bottom";
        regions_[2] = "hull";
    }
    return regions_;
}

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
Foam::tmp<Foam::pointField>
#else
Foam::pointField
#endif
Foam::unitCylinderSearchableSurface::coordinates() const
{
    pointField result(3);

    result[0]=top_.refPoint();
    result[1]=bottom_.refPoint();
    result[2]=0.5*(top_.refPoint()+bottom_.refPoint());

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
    return tmp<pointField>(new pointField(result));
#else
    return result;
#endif
}

bool Foam::unitCylinderSearchableSurface::overlaps(const boundBox& bb) const
{
    notImplemented
        (
            "Foam::unitCylinderSearchableSurface::overlaps(const boundBox&) const"
        );

    return false;
}


Foam::tmp<Foam::pointField> Foam::unitCylinderSearchableSurface::points() const
{
    pointField result(2);

    result[0]=top_.refPoint();
    result[1]=bottom_.refPoint();

    return result;
}

void Foam::unitCylinderSearchableSurface::findNearest
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


void Foam::unitCylinderSearchableSurface::findLine
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    info.setSize(start.size());

    forAll(start, i)
    {
        List<pointIndexHit> hits;

        findLineAll(start[i], end[i], hits);
        if(hits.size()>0) {
            info[i]=hits[0];
        }
    }
}


void Foam::unitCylinderSearchableSurface::findLineAny
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    info.setSize(start.size());

    forAll(start, i)
    {
        List<pointIndexHit> hits;

        findLineAll(start[i], end[i], hits);
        if(hits.size()>0) {
            info[i]=hits[0];
        }
    }
}


void Foam::unitCylinderSearchableSurface::findLineAll
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
        findLineAll(start[i], end[i], info[i]);
    }
}


void Foam::unitCylinderSearchableSurface::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    region.setSize(info.size());
    forAll(info,i) {
        region[i] = info[i].index();
    }
}


void Foam::unitCylinderSearchableSurface::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    normal.setSize(info.size());

    forAll(info,i) {
        switch(info[i].index()) {
            case 0:
                normal[i]=vector(0,0,1);
                break;
            case 1:
                normal[i]=vector(0,0,-1);
                break;
            default:
                vector tmp=info[i].rawPoint();
                tmp.z()=0;
                normal[i]=tmp/max(mag(tmp),SMALL);
        }
    }
}


void Foam::unitCylinderSearchableSurface::getVolumeType
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

        if(
            pt.z()>1
            ||
            pt.z()<-1
            ||
            (pt.x()*pt.x() + pt.y()*pt.y()) > 1
        )
        {
            volType[pointI] = OUTSIDE;
        }
        else
        {
            volType[pointI] = INSIDE;
        }
    }
}

#ifdef FOAM_SEARCHABLE_SURF_NEEDS_BOUNDING_SPHERES
void Foam::unitCylinderSearchableSurface::boundingSpheres
(
    pointField& centres,
    scalarField& radiusSqr
) const
{
    centres.setSize(1);
    centres[0] = 0.5*(top_.refPoint()+bottom_.refPoint());

    radiusSqr.setSize(1);
    radiusSqr[0] = 1*1 + 1*1 + SMALL;
}
#endif


// ************************************************************************* //
