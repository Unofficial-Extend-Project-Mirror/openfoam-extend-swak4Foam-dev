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
    stateMachineSetStateFunctionObject

Description
    Forces a state machine to a certain state

SourceFiles
    stateMachineSetStateFunctionObject.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef stateMachineSetStateFunctionObject_H
#define stateMachineSetStateFunctionObject_H

#include "simpleFunctionObject.H"

#include "StateMachine.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class stateMachineSetStateFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class stateMachineSetStateFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    stateMachineSetStateFunctionObject(const stateMachineSetStateFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const stateMachineSetStateFunctionObject&);

    StateMachine &machine_;

    const label state_;

public:

    //- Runtime type information
    TypeName("stateMachineSetState");


    // Constructors

    //- Construct from components
    stateMachineSetStateFunctionObject
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
