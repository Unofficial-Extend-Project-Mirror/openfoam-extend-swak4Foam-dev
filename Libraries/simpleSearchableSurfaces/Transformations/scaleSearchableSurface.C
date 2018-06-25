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
    2009, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "scaleSearchableSurface.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(scaleSearchableSurface, 0);
addToRunTimeSelectionTable(searchableSurface, scaleSearchableSurface, dict);

}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::scaleSearchableSurface::scaleSearchableSurface
(
    const IOobject& io,
    const dictionary& dict
)
:
    transformationSearchableSurface(io,dict),
    scale_(dict.lookup("scale"))
{
    scalar mini=min(mag(scale_.x()),min(mag(scale_.y()),mag(scale_.z())));
    if(mini<SMALL) {
        FatalErrorIn("Foam::scaleSearchableSurface::scaleSearchableSurface")
            << "Scaling vector " << scale_ << " has a component that is almost zero\n"
                << " -> Division by zero while inverse operation"
                <<endl
                <<abort(FatalError);
    }

#ifdef FOAM_SEARCHABLE_SURF_HAS_BOUND_METHOD
    pointField pts(2);
    pts[0]=transform(delegate().bounds().min());
    pts[1]=transform(delegate().bounds().max());

    bounds()=boundBox(pts);
#endif
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::scaleSearchableSurface::~scaleSearchableSurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::point Foam::scaleSearchableSurface::transform(const point &p) const
{
    return point
        (
            p.x()*scale_.x(),
            p.y()*scale_.y(),
            p.z()*scale_.z()
        );
}

Foam::point Foam::scaleSearchableSurface::inverseTransform(const point &p) const
{
    return point
        (
            p.x()/scale_.x(),
            p.y()/scale_.y(),
            p.z()/scale_.z()
        );
}

void Foam::scaleSearchableSurface::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    vectorField iNormal;

    transformationSearchableSurface::getNormal
        (
            info,
            iNormal
        );

    normal.setSize(iNormal.size());

    forAll(normal,i) {
        normal[i]=inverseTransform(iNormal[i]);
        scalar len=mag(normal[i]);
        if(len>SMALL) {
            normal[i]/=len;
        }
    }
}

#ifdef FOAM_SEARCHABLE_SURF_NEEDS_BOUNDING_SPHERES
void Foam::scaleSearchableSurface::boundingSpheres
(
    pointField& centres,
    scalarField& radiusSqr
) const
{
    delegate().boundingSpheres(
        centres,
        radiusSqr
    );
    scalar maxScale=mag(scale_.x());
    if(mag(scale_.y())>maxScale) {
        maxScale=mag(scale_.y());
    }
    if(mag(scale_.z())>maxScale) {
        maxScale=mag(scale_.z());
    }

    forAll(centres,i) {
        radiusSqr[i]=radiusSqr[i]*maxScale*maxScale;
        centres[i]=transform(centres[i]);
    }
}
#endif

// ************************************************************************* //
