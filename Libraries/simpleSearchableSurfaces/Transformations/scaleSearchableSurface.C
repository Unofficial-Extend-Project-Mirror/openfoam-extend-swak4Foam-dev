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
        normal[i]/=mag(normal[i]);
    }
}

// ************************************************************************* //
