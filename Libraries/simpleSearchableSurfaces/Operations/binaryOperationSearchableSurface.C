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

#include "binaryOperationSearchableSurface.H"
#include "SortableList.H"

#include <cassert>

#define USE_BOTH_HITS

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(binaryOperationSearchableSurface, 1);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::binaryOperationSearchableSurface::binaryOperationSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    searchableSurface(io),
    a_(
        searchableSurface::New
        (
            word(dict.subDict("a").lookup("type")),
            *this,
            dict.subDict("a")
        )
    ),
    b_(
        searchableSurface::New
        (
            word(dict.subDict("b").lookup("type")),
            *this,
            dict.subDict("b")
        )
    ),
    nrARegions_(
        a().regions().size()
    ),
    nrBRegions_(
        b().regions().size()
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::binaryOperationSearchableSurface::~binaryOperationSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList& Foam::binaryOperationSearchableSurface::regions() const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::regions " << name() << endl;
    }

    if(regions_.size() == 0 ) {
        regions_.setSize(nrARegions_+nrBRegions_);
        for(label i=0;i<nrARegions_;i++) {
            regions_[i]="A_"+a().regions()[i];
        }
        for(label i=0;i<nrBRegions_;i++) {
            regions_[i+nrARegions_]="B_"+b().regions()[i];
        }
    }
    return regions_;
}

Foam::pointField Foam::binaryOperationSearchableSurface::coordinates() const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::coordinates " << name() << endl;
    }

    pointField aCoords(a().coordinates());
    pointField bCoords(b().coordinates());

    pointField result(aCoords.size()+bCoords.size());

    forAll(aCoords,i) {
        result[i]=aCoords[i];
    }
    forAll(bCoords,i) {
        result[i+aCoords.size()]=bCoords[i];
    }

    return result;
}

// void Foam::binaryOperationSearchableSurface::findNearest
// (
//     const pointField& samples,
//     const scalarField& nearestDistSqr,
//     List<pointIndexHit>& info
// ) const
// {
// }


void Foam::binaryOperationSearchableSurface::findLine
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::findLine " << name() << endl;
    }

    List<List<pointIndexHit> > infoAll;
    findLineAll(start,end,infoAll);
    info.setSize(start.size());

    forAll(info,i) {
        if(infoAll[i].size()>0) {
            info[i]=infoAll[i][0];
        }
    }
}


void Foam::binaryOperationSearchableSurface::findLineAny
(
    const pointField& start,
    const pointField& end,
    List<pointIndexHit>& info
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::findLineAny " << name() << endl;
    }

    findLine(start,end,info);
}


void Foam::binaryOperationSearchableSurface::findLineAll
(
    const pointField& start,
    const pointField& end,
    List<List<pointIndexHit> >& info
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::findLineAll " << name() << endl;
    }
    List<List<pointIndexHit> > infoA;
    List<List<pointIndexHit> > infoB;

    a().findLineAll(start,end,infoA);
    b().findLineAll(start,end,infoB);

    info.setSize(start.size());

    forAll(info,i) {
        if(
            infoA[i].size()==0
            &&
            infoB[i].size()==0
        ) {
            info[i].setSize(0);
            continue;
        }

        this->filter(infoA[i],infoB[i],info[i]);

       // it's only ugly bubble-sort but OK for the small number of points
        for(label j=0;j<(info[i].size()-1);j++) {
            for(label k=j+1;k<info[i].size();k++) {
                if
                    (
                        mag(start[i]-info[i][k].rawPoint())
                        <
                        mag(start[i]-info[i][j].rawPoint())
                    ) {
                    pointIndexHit tmp=info[i][j];
                    info[i][j]=info[i][k];
                    info[i][k]=tmp;
                }
            }
        }

        scalar identicalThreshhold=1e-10;

        bool clipIdentical=false;
        for(label j=1;j<info[i].size();j++) {
            if(
                mag(
                    info[i][j-1].rawPoint()
                    -
                    info[i][j].rawPoint()
                )<identicalThreshhold
            ) {
                clipIdentical=true;
                break;
            }
        }
        if(clipIdentical) {
            //            Info << "Identical points in " << info[i] << endl;
            DynamicList<pointIndexHit> cleaned;
            cleaned.append(info[i][0]);
            for(label j=1;j<info[i].size();j++) {
                if(
                    mag(
                        cleaned[cleaned.size()-1].rawPoint()
                        -
                        info[i][j].rawPoint()
                    )>identicalThreshhold
                ){
                    cleaned.append(info[i][j]);
                }
            }
            info[i]=cleaned;
            //            Info << "Cleaned: " << info[i] << endl;
        }
    }

    if(debug) {
        label cntA=0,cntB=0,cnt=0;
        forAll(infoA,j) {
            cntA+=infoA[j].size();
            cntB+=infoB[j].size();
            cnt +=info[j].size();
        }
        Info << "Found A: " << cntA << " B: " << cntB
            << " Total: " << cnt << endl;
    }
 }


void Foam::binaryOperationSearchableSurface::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::getRegion " << name() << endl;
    }

    List<hitWhom> who;
    whose(info,who);

    List<pointIndexHit> infoA;
    List<pointIndexHit> infoB;
    splitHits(
        info,
        who,
        infoA,
        infoB
    );

    labelList regionA;
    labelList regionB;
    a().getRegion(infoA,regionA);
    b().getRegion(infoB,regionB);

    region.setSize(who.size());

    label cntA=0,cntB=0;
    forAll(who,i) {
        if(who[i]==BOTH || who[i]==HITSA) {
            region[i]=regionA[cntA];
            cntA++;
#ifdef USE_BOTH_HITS
            if(who[i]==BOTH) {
                cntB++;
            }
#endif
        } else if(who[i]==HITSB) {
            if(regionB[cntB]>=0) {
                region[i]=regionB[cntB]+nrARegions_;
            } else {
                region[i]=regionB[cntB];
            }
            cntB++;
        }
    }

    if(cntA!=regionA.size() || cntB!=regionB.size()) {
        FatalErrorIn("binaryOperationSearchableSurface::getRegion")
            << "Something went horribly wrong. The sizes "
                << cntA << "!=" << regionA.size() << " and/or "
                << cntB << "!=" << regionB.size() << "\n"
                << "We're lucky that the program got here\n"
                << endl
                << abort(FatalError);
    }

    assert(region.size()==info.size());

    if(debug) {
        HashTable<label,label> cnts;
        forAll(region,i) {
            label reg=region[i];
            if(!cnts.found(reg)) {
                cnts.insert(reg,1);
            } else {
                cnts[reg]++;
            }
        }
        Info << "Counts: " << cnts << endl;
    }
}


void Foam::binaryOperationSearchableSurface::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::getNormal " << name() << endl;
    }

    List<hitWhom> who;
    whose(info,who);

    List<pointIndexHit> infoA;
    List<pointIndexHit> infoB;
    splitHits(
        info,
        who,
        infoA,
        infoB
    );

    vectorField normalA;
    vectorField normalB;
    a().getNormal(infoA,normalA);
    b().getNormal(infoB,normalB);

    normal.setSize(info.size());

    label cntA=0,cntB=0;
    forAll(who,i) {
        if(who[i]==BOTH || who[i]==HITSA) {
            normal[i]=normalA[cntA];
            if(revertNormalA(info[i])) {
                normal[i]*=-1;
            }
            cntA++;
            if(who[i]==BOTH) {
                cntB++;
            }
        } else {
            normal[i]=normalB[cntB];
            if(revertNormalB(info[i])) {
                normal[i]*=-1;
            }
            cntB++;
        }
    }
    if(cntA!=normalA.size() || cntB!=normalB.size()) {
        FatalErrorIn("binaryOperationSearchableSurface::getNormal")
            << "Something went horribly wrong. The sizes "
                << cntA << "!=" << normalA.size() << " and/or "
                << cntB << "!=" << normalB.size() << "\n"
                << "We're lucky that the program got here\n"
                << endl
                << abort(FatalError);
    }
}

void  Foam::binaryOperationSearchableSurface::whose
(
    const List<pointIndexHit>& hits,
    List<hitWhom> &whom
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::whose " << name() << endl;
    }

    pointField samples(hits.size());
    scalarField distance(hits.size(),1e5);

    forAll(hits,i) {
        samples[i]=hits[i].rawPoint();
    }

    List<pointIndexHit> nearestA;
    List<pointIndexHit> nearestB;

    a().findNearest(samples,distance,nearestA);
    b().findNearest(samples,distance,nearestB);

    whom.setSize(hits.size(),NONE);

    //    Info << nearestA << nearestB << endl;

    forAll(whom,i) {
        scalar distA=HUGE;
        scalar distB=HUGE;
        if(nearestA[i].hit()) {
            distA=mag(samples[i]-nearestA[i].hitPoint());
        }
        if(nearestB[i].hit()) {
            distB=mag(samples[i]-nearestB[i].hitPoint());
        }
        if(
            distA>1e-5
            &&
            distB>1e-5
        ) {
            whom[i]=NONE;
        } else if(
            distA<SMALL
            &&
            distB<SMALL
        ) {
            whom[i]=BOTH;
        } else {
            whom[i]=( distA <= distB ? HITSA : HITSB);
        }
    }
    if(debug) {
        label cntA=0,cntB=0,cntBoth=0,cntNone=0;
        forAll(whom,i) {
            switch(whom[i]) {
                case HITSA:
                    cntA++; break;
                case HITSB:
                    cntB++; break;
                case BOTH:
                    cntBoth++; break;
                case NONE:
                    cntNone++; break;
            }
        }
        Info << whom.size() << " points. A: " << cntA
            << " B: " << cntB << " Both: " << cntBoth
            << " None: " << cntNone << endl;
    }
}

void  Foam::binaryOperationSearchableSurface::splitHits
(
    const List<pointIndexHit>& hits,
    const List<hitWhom> &isA,
    List<pointIndexHit>& hitsA,
    List<pointIndexHit>& hitsB
) const
{
    if(debug) {
        Info << "Foam::binaryOperationSearchableSurface::splitHits " << name() << endl;
    }

    label nrA=0;
    label nrB=0;
    forAll(isA,i) {
        if(isA[i]==BOTH || isA[i]==HITSA){
            nrA++;
        }
#ifdef USE_BOTH_HITS
        if(isA[i]==BOTH || isA[i]==HITSB){
#else
        if(isA[i]==HITSB){
#endif
            nrB++;
        }
    }

    hitsA.setSize(nrA);
    hitsB.setSize(nrB);

    label cntA=0;
    label cntB=0;

    forAll(hits,i) {
        if(isA[i]==BOTH || isA[i]==HITSA){
            hitsA[cntA]=hits[i];
            cntA++;
        }
#ifdef USE_BOTH_HITS
        if(isA[i]==BOTH || isA[i]==HITSB){
#else
        if(isA[i]==HITSB){
#endif
            hitsB[cntB]=hits[i];
            cntB++;
        }
    }
    if( cntA!=nrA || cntB!=nrB ) {
        FatalErrorIn("Foam::binaryOperationSearchableSurface::splitHits")
            << "Counts differ: " << cntA << "/" << nrA << " " << cntB << "/" << nrB
                << "\n" << endl
                << abort(FatalError);
    }
    if(debug) {
        Info << "From " << hits.size() << " to A: " << hitsA.size()
            << " to B: " << hitsB.size() << endl;
    }
}

void  Foam::binaryOperationSearchableSurface::inside
(
    const searchableSurface &s,
    const List<pointIndexHit>& hits,
    List<bool> &in
) const
{
    pointField samples(hits.size());

    forAll(hits,i) {
        samples[i]=hits[i].rawPoint();
    }

    List<volumeType> vol;

    s.getVolumeType(samples,vol);

    in.setSize(vol.size());

    forAll(vol,i) {
        //        in[i]=(vol[i]==INSIDE || vol[i]==UNKNOWN);
        in[i]=(vol[i]==INSIDE);
    }
}

// ************************************************************************* //
