/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2009, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
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

bool Foam::intersectSearchableSurface::decidePoint(
        const hitWhom who,
        const bool inA,
        const bool inB
) const
{
    if(
        who==BOTH
    ) {
        return true;
    } else if(
        (inA && who==HITSB)
        ||
        (inB && who==HITSA)
    ) {
        return true;
    }

    return false;
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
        if( inA[i]==VOLTYPE_INSIDE && inB[i]==VOLTYPE_INSIDE ) {
            volType[i]=VOLTYPE_INSIDE;
        } else if( inA[i]==VOLTYPE_UNKNOWN || inB[i]==VOLTYPE_UNKNOWN) {
            volType[i]=VOLTYPE_UNKNOWN;
        } else {
            volType[i]=VOLTYPE_OUTSIDE;
        }
    }
}

// ************************************************************************* //
