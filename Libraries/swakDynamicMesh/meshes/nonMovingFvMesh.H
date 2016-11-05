/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
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
    Foam::nonMovingFvMesh

Description
    Foam::nonMovingFvMesh - Basically the staticFvMesh-class but it pretends
    to have moved

SourceFiles
    nonMovingFvMesh.C

\*---------------------------------------------------------------------------*/

#ifndef nonMovingFvMesh_H
#define nonMovingFvMesh_H

#include "dynamicFvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class nonMovingFvMesh Declaration
\*---------------------------------------------------------------------------*/

class nonMovingFvMesh
:
    public dynamicFvMesh
{
    // Private Member Functions

        //- Disallow default bitwise copy construct
        nonMovingFvMesh(const nonMovingFvMesh&);

        //- Disallow default bitwise assignment
        void operator=(const nonMovingFvMesh&);


public:

    //- Runtime type information
    TypeName("nonMovingFvMesh");


    // Constructors

        //- Construct from IOobject
        nonMovingFvMesh(const IOobject& io);


    //- Destructor
    ~nonMovingFvMesh();


    // Member Functions

        //- Dummy update function which does not change the mesh
        virtual bool update();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
