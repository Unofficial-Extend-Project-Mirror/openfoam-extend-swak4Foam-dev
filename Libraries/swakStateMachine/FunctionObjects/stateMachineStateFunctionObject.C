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

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "stateMachineStateFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(stateMachineStateFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        stateMachineStateFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

stateMachineStateFunctionObject::stateMachineStateFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    timelineFunctionObject(
        name,
        t,
        dict,
        true  // by default write the first state
    ),
    machineName_(
        dict.lookup("machineName")
    )
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word stateMachineStateFunctionObject::dirName()
{
    return typeName;
}

wordList stateMachineStateFunctionObject::fileNames()
{
    return wordList(1,name());
}

stringList stateMachineStateFunctionObject::columnNames()
{
    const StateMachine &m=StateMachine::machine(machineName_);

    stringList result(4+m.numberOfStates());

    result[0]="state";
    result[1]="code";
    result[2]="timeSinceChange";
    result[3]="stepsSinceChange";
    for(label i=0;i<m.numberOfStates();i++) {
        result[4+i]="ChangedTo_"+m.stateName(i);
    }
    return result;
}

void stateMachineStateFunctionObject::writeSimple()
{
    if(!Pstream::master()) {
        return;
    }
    const StateMachine &m=StateMachine::machine(machineName_);
    const word state(m.stateName(m.currentState()));

    if(verbose()) {
        Info << "Machine " << machineName_
            << " in state " << state
            << " (code: " << m.currentState() << ")"
            << " for " << m.timeSinceChange() << " s" << endl;
    }

    writeTime(name(),time().value());
    stringList result(4+m.numberOfStates());

    result[0]=state;
    result[1]=Foam::name(m.currentState());
    result[2]=Foam::name(m.timeSinceChange());
    result[3]=Foam::name(m.stepsSinceChange());
    for(label i=0;i<m.numberOfStates();i++) {
        result[4+i]=Foam::name(m.changedTo(i));
    }
    writeData(name(),result);
    endData(name());
}

} // namespace Foam

// ************************************************************************* //
