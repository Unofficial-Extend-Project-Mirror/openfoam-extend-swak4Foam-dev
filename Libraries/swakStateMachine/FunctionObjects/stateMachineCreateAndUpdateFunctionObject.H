/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           |
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
    stateMachineCreateAndUpdateFunctionObject

Description
    Creates a state-machine and updates it at ever time-step

SourceFiles
    stateMachineCreateAndUpdateFunctionObject.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef stateMachineCreateAndUpdateFunctionObject_H
#define stateMachineCreateAndUpdateFunctionObject_H

#include "simpleFunctionObject.H"

#include "StateMachine.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class stateMachineCreateAndUpdateFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class stateMachineCreateAndUpdateFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    stateMachineCreateAndUpdateFunctionObject(const stateMachineCreateAndUpdateFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const stateMachineCreateAndUpdateFunctionObject&);

    StateMachine &machine_;

public:

    //- Runtime type information
    TypeName("stateMachineCreateAndUpdate");


    // Constructors

    //- Construct from components
    stateMachineCreateAndUpdateFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
