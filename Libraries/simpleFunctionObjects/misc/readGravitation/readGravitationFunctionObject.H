//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
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
    readGravitationFunctionObject

Description
    Read the gravitational acceleration. This is done to satisfy boundary
    conditions that rely on it for solvers/utilities that don't provide it

SourceFiles
    readGravitationFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef readGravitationFunctionObject_H
#define readGravitationFunctionObject_H

#include "simpleFunctionObject.H"
#include "uniformDimensionedFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class readGravitationFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class readGravitationFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    readGravitationFunctionObject(const readGravitationFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const readGravitationFunctionObject&);

    //- the actual field
    uniformDimensionedVectorField g_;

public:

    //- Runtime type information
    TypeName("readGravitation");


    // Constructors

    //- Construct from components
    readGravitationFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
