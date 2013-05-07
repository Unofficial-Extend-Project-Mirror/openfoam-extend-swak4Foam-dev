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
    Foam::translateSearchableSurface

Description
    Searching on translated surface

SourceFiles
    translateSearchableSurface.C

Contributors/Copyright:
    2009, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef translateSearchableSurface_H
#define translateSearchableSurface_H

#include "transformationSearchableSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                           Class translateSearchableSurface Declaration
\*---------------------------------------------------------------------------*/

class translateSearchableSurface
:
    public transformationSearchableSurface
{
private:

    // Private Member Data

    vector translation_;


        //- Disallow default bitwise copy construct
        translateSearchableSurface(const translateSearchableSurface&);

        //- Disallow default bitwise assignment
        void operator=(const translateSearchableSurface&);

protected:

    // Do the transformation for a point
    virtual point transform(const point &) const;

    // Do the inverse transformation for a point
    virtual point inverseTransform(const point &) const;

public:

    //- Runtime type information
    TypeName("translate");


    // Constructors

        //- Construct from components
        translateSearchableSurface(const IOobject& io, const treeBoundBox& bb);

        //- Construct from dictionary (used by transformationSearchableSurface)
        translateSearchableSurface
        (
            const IOobject& io,
            const dictionary& dict
        );

    // Destructor

        virtual ~translateSearchableSurface();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
