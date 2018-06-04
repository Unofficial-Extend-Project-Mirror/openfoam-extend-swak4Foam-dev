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

Class
    Foam::searchableSphereWithFix

Description
    Searching on sphere (fixes problem with the original implementation)

SourceFiles
    searchableSphereWithFix.C

Contributors/Copyright:
    2009, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
