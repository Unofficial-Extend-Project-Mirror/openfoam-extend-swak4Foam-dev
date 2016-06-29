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

#include "transformationSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"
#include "SortableList.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(transformationSearchableSurface, 0);
    // addToRunTimeSelectionTable(searchableSurface, transformationSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::transformationSearchableSurface::transformationSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    searchableSurface(io),
    rename_(
        dict.subDict("surface").found("rename")
        ?
        new IOobject(
            word(dict.subDict("surface")["rename"]),
            static_cast<const searchableSurface&>(*this).instance(),
            io.local(),
            io.db(),
            io.readOpt(),
            io.writeOpt(),
            false
        )
        :
        NULL
    ),
    delegate_(
        searchableSurface::New
        (
            word(dict.subDict("surface").lookup("type")),
            (rename_.valid() ? rename_() : *this),
            dict.subDict("surface")
        )
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::transformationSearchableSurface::~transformationSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::pointIndexHit Foam::transformationSearchableSurface::transform
(
    const pointIndexHit &orig
) const
{
    return pointIndexHit
        (
            orig.hit(),
            transform(orig.rawPoint()),
            orig.index()
        );
}

Foam::pointIndexHit Foam::transformationSearchableSurface::inverseTransform
(
    const pointIndexHit &orig
) const
{
    return pointIndexHit
        (
            orig.hit(),
            inverseTransform(orig.rawPoint()),
            orig.index()
        );
}

const Foam::wordList& Foam::transformationSearchableSurface::regions() const
{
    return delegate().regions();
}

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
Foam::tmp<Foam::pointField>
#else
Foam::pointField
#endif
Foam::transformationSearchableSurface::coordinates() const
{
#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
    tmp<pointField> tResult(new pointField(delegate().coordinates()));
    pointField &result=tResult();
#else
    pointField result(delegate().coordinates());
#endif

    forAll(result,i) {
        result[i]=transform(result[i]);
    }

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
    return tResult;
#else
    return result;
#endif
}

// Foam::pointIndexHit Foam::transformationSearchableSurface::findNearest
// (
//     const point& sample,
//     const scalar nearestDistSqr
// ) const
// {
//     pointIndexHit result=delegate().findNearest(
//         inverseTransform(sample),
//         nearestDistSqr
//     );

//     return transform(result);
// }


// Foam::pointIndexHit Foam::transformationSearchableSurface::findNearestOnEdge
// (
//     const point& sample,
//     const scalar nearestDistSqr
// ) const
// {
//     pointIndexHit result=delegate().findNearestOnEdge(
//         inverseTransform(sample),
//         nearestDistSqr
//     );

//     return transform(result);
// }


// Foam::pointIndexHit Foam::transformationSearchableSurface::findNearest
// (
//     const linePointRef& ln,
//     treeBoundBox& tightest,
//     point& linePoint
// ) const
// {
//     notImplemented
//     (
//         "transformationSearchableSurface::findNearest"
//         "(const linePointRef&, treeBoundBox&, point&)"
//     );
//     return pointIndexHit();
// }


// Foam::pointIndexHit Foam::transformationSearchableSurface::findLine
// (
//     const point& start,
//     const point& end
// ) const
// {
//     pointIndexHit result=delegate().findLine(
//         inverseTransform(start),
//         inverseTransform(end)
//     );

//     return transform(result);
// }


// Foam::pointIndexHit Foam::transformationSearchableSurface::findLineAny
// (
//     const point& start,
//     const point& end
// ) const
// {
//     pointIndexHit result=delegate().findLineAny(
//         inverseTransform(start),
//         inverseTransform(end)
//     );

//     return transform(result);
// }


void Foam::transformationSearchableSurface::findNearest
(
    const pointField& samples,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& info
) const
{
    pointField iSamples(samples.size());
    List<pointIndexHit> iInfo;

    forAll(samples,i) {
        iSamples[i]=inverseTransform(samples[i]);
    }

    delegate().findNearest
        (
            iSamples,
            nearestDistSqr,
            iInfo
        );

    info.setSize(iInfo.size());
    forAll(info,i) {
        info[i]=transform(iInfo[i]);
    }
}


void Foam::transformationSearchableSurface::findLine
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    pointField iStart(start.size());
    pointField iEnd(end.size());
    List<pointIndexHit> iInfo;

    forAll(start,i) {
        iStart[i]=inverseTransform(start[i]);
        iEnd[i]=inverseTransform(end[i]);
    }

    delegate().findLine
        (
            iStart,
            iEnd,
            iInfo
        );

    info.setSize(iInfo.size());
    forAll(info,i) {
        info[i]=transform(iInfo[i]);
    }
}


void Foam::transformationSearchableSurface::findLineAny
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    pointField iStart(start.size());
    pointField iEnd(end.size());
    List<pointIndexHit> iInfo;

    forAll(start,i) {
        iStart[i]=inverseTransform(start[i]);
        iEnd[i]=inverseTransform(end[i]);
    }

    delegate().findLineAny
        (
            iStart,
            iEnd,
            iInfo
        );

    info.setSize(iInfo.size());
    forAll(info,i) {
        info[i]=transform(iInfo[i]);
    }
}


void Foam::transformationSearchableSurface::findLineAll
(
    const pointField& start,
    const pointField& end,
    List<List<pointIndexHit> >& info
) const
{
    pointField iStart(start.size());
    pointField iEnd(end.size());
    List<List<pointIndexHit> > iInfo;

    forAll(start,i) {
        iStart[i]=inverseTransform(start[i]);
        iEnd[i]=inverseTransform(end[i]);
    }

    delegate().findLineAll
        (
            iStart,
            iEnd,
            iInfo
        );

    info.setSize(iInfo.size());
    forAll(info,i) {
        info[i].setSize(iInfo[i].size());
        forAll(info[i],j) {
            info[i][j]=transform(iInfo[i][j]);
        }
    }
}


void Foam::transformationSearchableSurface::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    List<pointIndexHit> iInfo(info.size());
    forAll(info,i) {
        iInfo[i]=inverseTransform(info[i]);
    }

    delegate().getRegion
        (
            iInfo,
            region
        );
}


void Foam::transformationSearchableSurface::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    List<pointIndexHit> iInfo(info.size());
    forAll(info,i) {
        iInfo[i]=inverseTransform(info[i]);
    }

    delegate().getNormal
        (
            iInfo,
            normal
        );
    forAll(normal,i) {
        //        Info << normal[i] << " at " << iInfo[i].rawPoint() << endl;
        //        Info << info[i].rawPoint() << transform(iInfo[i].rawPoint()) << endl;
        vector toPt=iInfo[i].rawPoint()+normal[i];
        normal[i]=transform(toPt)-info[i].rawPoint();
        //        Info << " -> " << normal[i] << " " << toPt << " " << transform(toPt) << endl;
    }
}


void Foam::transformationSearchableSurface::getVolumeType
(
    const pointField& points,
    List<volumeType>& volType
) const
{
    pointField iPoints(points.size());
    forAll(points,i) {
        iPoints[i]=inverseTransform(points[i]);
    }

    delegate().getVolumeType
        (
            iPoints,
            volType
        );
}


#ifdef FOAM_SEARCHABLE_SURF_HAS_POINTS
Foam::tmp<Foam::pointField> Foam::transformationSearchableSurface::points() const
{
    pointField result(delegate().points());

    forAll(result,i) {
        result[i]=transform(result[i]);
    }

    return result;
}
#endif

bool Foam::transformationSearchableSurface::overlaps(const boundBox& bb) const
{
    notImplemented
        (
            "Foam::transformationSearchableSurface::overlaps(const boundBox&) const"
        );

    return false;
}


// ************************************************************************* //
