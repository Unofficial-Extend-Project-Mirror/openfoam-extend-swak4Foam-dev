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

Class
    Foam::scaleSearchableSurface

Description
    Searching on scaled surface

SourceFiles
    scaleSearchableSurface.C

Contributors/Copyright:
    2009, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef scaleSearchableSurface_H
#define scaleSearchableSurface_H

#include "transformationSearchableSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                           Class scaleSearchableSurface Declaration
\*---------------------------------------------------------------------------*/

class scaleSearchableSurface
:
    public transformationSearchableSurface
{
private:

    // Private Member Data

    vector scale_;


        //- Disallow default bitwise copy construct
        scaleSearchableSurface(const scaleSearchableSurface&);

        //- Disallow default bitwise assignment
        void operator=(const scaleSearchableSurface&);

protected:

    // Do the transformation for a point
    virtual point transform(const point &) const;

    // Do the inverse transformation for a point
    virtual point inverseTransform(const point &) const;

public:

    //- Runtime type information
    TypeName("scale");


    // Constructors

        //- Construct from components
        scaleSearchableSurface(const IOobject& io, const treeBoundBox& bb);

        //- Construct from dictionary (used by transformationSearchableSurface)
        scaleSearchableSurface
        (
            const IOobject& io,
            const dictionary& dict
        );

    // Destructor

        virtual ~scaleSearchableSurface();

    //- From a set of points and indices get the normal
    virtual void getNormal
    (
        const List<pointIndexHit>&,
        vectorField& normal
    ) const;

#ifdef FOAM_SEARCHABLE_SURF_NEEDS_BOUNDING_SPHERES
    virtual void boundingSpheres
    (
        pointField& centres,
        scalarField& radiusSqr
    ) const;
#endif

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
