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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::swakRegistryProxySet

Description
    A sampledSet that is only a Proxy for one found in the 
    Set-registry of swak4Foam

SourceFiles
    swakRegistryProxySet.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef swakRegistryProxySet_H
#define swakRegistryProxySet_H

#include "sampledSet.H"

#include "SetsRepository.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class swakRegistryProxySet Declaration
\*---------------------------------------------------------------------------*/

class swakRegistryProxySet
:
    public sampledSet
{
    // Private data

    word setName_;

    const sampledSet &realSet() const;

    sampledSet &realSet();

public:

    //- Runtime type information
    TypeName("swakRegistryProxy");


    // Constructors

        //- Construct from dictionary
        swakRegistryProxySet
        (
            const word& name,
            const polyMesh& mesh,
            meshSearch& search,
            const dictionary& dict
        );


    // Destructor

        virtual ~swakRegistryProxySet();


    // Member Functions
    //    virtual point getRefPoint (const List< point > &) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
