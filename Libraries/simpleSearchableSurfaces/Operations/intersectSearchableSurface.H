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
    Foam::intersectSearchableSurface

Description
    Searching on rotated surface

SourceFiles
    intersectSearchableSurface.C

Contributors/Copyright:
    2009, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef intersectSearchableSurface_H
#define intersectSearchableSurface_H

#include "binaryOperationSearchableSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                           Class intersectSearchableSurface Declaration
\*---------------------------------------------------------------------------*/

class intersectSearchableSurface
:
    public binaryOperationSearchableSurface
{
private:
        //- Disallow default bitwise copy construct
        intersectSearchableSurface(const intersectSearchableSurface&);

        //- Disallow default bitwise assignment
        void operator=(const intersectSearchableSurface&);

protected:

    virtual bool decidePoint(
        const hitWhom who,
        const bool inA,
        const bool inB
    ) const;

    //- Check whether the normal for a point on surface A should be reverted
    bool revertNormalA(const pointIndexHit&) const
        { return false; }

    //- Check whether the normal for a point on surface B should be reverted
    bool revertNormalB(const pointIndexHit&) const
        { return false; }

public:

    //- Runtime type information
    TypeName("intersect");


    // Constructors

        //- Construct from dictionary (used by transformationSearchableSurface)
        intersectSearchableSurface
        (
            const IOobject& io,
            const dictionary& dict
        );

    // Destructor

        virtual ~intersectSearchableSurface();

   void getVolumeType
    (
        const pointField& points,
        List<volumeType>& volType
    ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
