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

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::nonMovingFvMesh

Description
    Foam::nonMovingFvMesh - Basically the staticFvMesh-class but it pretends
    to have moved

SourceFiles
    nonMovingFvMesh.C

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
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
