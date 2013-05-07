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

Class
    Foam::searchableSphereWithFix

Description
    Searching on sphere (fixes problem with the original implementation)

SourceFiles
    searchableSphereWithFix.C

Contributors/Copyright:
    2009, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef searchableSphereWithFix_H
#define searchableSphereWithFix_H

#include "searchableSphere.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                           Class searchableSphereWithFix Declaration
\*---------------------------------------------------------------------------*/

class searchableSphereWithFix
:
    public searchableSphere
{
private:

    // Private Member Data

    //- Centre point
    const point centre_;

    //- Radius squared
    const scalar radius_;

   //- Disallow default bitwise copy construct
    searchableSphereWithFix(const searchableSphereWithFix&);

    //- Disallow default bitwise assignment
    void operator=(const searchableSphereWithFix&);

    pointIndexHit findNearest
    (
        const point& sample,
        const scalar nearestDistSqr
    ) const;

public:

    //- Runtime type information
    TypeName("searchableSphereWithFix");


    // Constructors

       //- Construct from dictionary (used by searchableSurface)
        searchableSphereWithFix
        (
            const IOobject& io,
            const dictionary& dict
        );

    // Destructor

        virtual ~searchableSphereWithFix();

    virtual void findNearest
    (
        const pointField& sample,
        const scalarField& nearestDistSqr,
        List<pointIndexHit>&
    ) const;

     // regIOobject implementation


     bool writeData(Ostream&) const
        {
            notImplemented("searchableSphereWithFix::writeData(Ostream&) const");
            return false;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
