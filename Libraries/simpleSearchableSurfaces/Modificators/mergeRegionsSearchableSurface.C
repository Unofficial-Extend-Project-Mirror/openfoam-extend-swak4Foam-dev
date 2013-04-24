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

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "mergeRegionsSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(mergeRegionsSearchableSurface, 0);
    addToRunTimeSelectionTable(searchableSurface, mergeRegionsSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::mergeRegionsSearchableSurface::mergeRegionsSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    wrapperSearchableSurface(io,dict)
{
    word name(dict.lookup("newName"));
    wordList children(dict.lookup("regions"));
    const wordList &oldNames=delegate().regions();
    const pointField oldCoords(delegate().coordinates());

    label size=oldNames.size()-children.size()+1;

    if(size<1) {
        FatalErrorIn("Foam::mergeRegionsSearchableSurface::mergeRegionsSearchableSurface")
            << "To few regions " << oldNames << " to merge to "
                << children << endl
                << abort(FatalError);
    }

    regions_.setSize(size);
    coordinates_.setSize(size);
    toRegion_.setSize(oldNames.size());

    regions_[0]=name;
    label cnt=1;
    bool hasZeroCoord=false;

    forAll(oldNames,i) {
        const word &n=oldNames[i];
        bool found=false;
        forAll(children,j) {
            if(children[j]==n) {
                found=true;
                break;
            }
        }
        if(found) {
            toRegion_[i]=0;
            if(!hasZeroCoord) {
                coordinates_[0]=oldCoords[i];
                hasZeroCoord=true;
            }
        } else {
            if(cnt>=size) {
                FatalErrorIn("Foam::mergeRegionsSearchableSurface::mergeRegionsSearchableSurface")
                    << "Too few matched regions in " << children
                        << " (old: " << oldNames << ")\n" << endl
                        << abort(FatalError);
            }
            regions_[cnt]=n;
            coordinates_[cnt]=oldCoords[i];
            toRegion_[i]=cnt;
            cnt++;
        }
    }

    if(cnt!=size) {
        FatalErrorIn("Foam::mergeRegionsSearchableSurface::mergeRegionsSearchableSurface")
            << "Mismatch with " << children
                << " (old: " << oldNames << ")\n" << endl
                << abort(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::mergeRegionsSearchableSurface::~mergeRegionsSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


const Foam::wordList& Foam::mergeRegionsSearchableSurface::regions() const
{
    return regions_;
}

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
Foam::tmp<Foam::pointField>
#else
Foam::pointField
#endif
Foam::mergeRegionsSearchableSurface::coordinates() const
{
#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
    return tmp<pointField>(new pointField(coordinates_));
#else
    return coordinates_;
#endif
}


void Foam::mergeRegionsSearchableSurface::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    delegate().getRegion(info,region);

    forAll(region,i) {
        region[i]=toRegion_[region[i]];
    }
}




// ************************************************************************* //
