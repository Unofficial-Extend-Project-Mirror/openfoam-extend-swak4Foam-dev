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

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "stateMachineFvSolutionFvSchemesFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "StateMachine.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(stateMachineFvSolutionFvSchemesFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        stateMachineFvSolutionFvSchemesFunctionObject,
        dictionary
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

stateMachineFvSolutionFvSchemesFunctionObject::stateMachineFvSolutionFvSchemesFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    manipulateFvSolutionFvSchemesFunctionObject(name,t,dict),
    solutionStateMachineName_(
        dict.lookup("solutionStateMachine")
    ),
    stateToSolutionMapping_(
        dict.lookup("stateToSolution")
    ),
    lastSolutionState_(""),
    schemesStateMachineName_(
        dict.lookup("schemesStateMachine")
    ),
    stateToSchemesMapping_(
        dict.lookup("stateToSchemes")
    ),
    lastSchemesState_(""),
    resetBeforeTrigger_(
        readBool(dict.lookup("resetBeforeTrigger"))
    )
{
    checkTriggerMapping(
        solutionStateMachineName_,
        stateToSolutionMapping_,
        fvSolutionDict()
    );
    checkTriggerMapping(
        schemesStateMachineName_,
        stateToSchemesMapping_,
        fvSchemesDict()
    );
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool stateMachineFvSolutionFvSchemesFunctionObject::manipulateFvSolution(const Time &t)
{
    return triggerMachine(
        t,
        solutionStateMachineName_,
        stateToSolutionMapping_,
        fvSolutionDict(),
        lastSolutionState_
    );
}

bool stateMachineFvSolutionFvSchemesFunctionObject::manipulateFvSchemes(const Time &t)
{
    return triggerMachine(
        t,
        schemesStateMachineName_,
        stateToSchemesMapping_,
        fvSchemesDict(),
        lastSchemesState_
    );
}

void stateMachineFvSolutionFvSchemesFunctionObject::checkTriggerMapping(
    const word &machine,
    const StateToMap &mapping,
    const dictionary &dict
)
{
    const StateMachine &m=StateMachine::machine(machine);

    forAllConstIter(StateToMap,mapping,iter) {
        const word &stateName=iter.key();
        const word &dictName=*iter;

        if(!m.hasState(stateName)) {
                FatalErrorIn("stateMachineFvSolutionFvSchemesFunctionObject::checkTriggerMapping")
                    << "The state named " << stateName
                        << " does not exist in machine " << machine
                        << endl
                        << exit(FatalError);
        }
        if(
            !dict.isDict(dictName)
        ){
            FatalErrorIn("stateMachineFvSolutionFvSchemesFunctionObject::checkTriggerMapping")
                << "No subdictionary " << dictName
                    << " in " << dict.name()
                    << endl
                    << exit(FatalError);
        }
    }
}

bool stateMachineFvSolutionFvSchemesFunctionObject::triggerMachine(
    const Time &t,
    const word &machine,
    const StateToMap &mapping,
    dictionary &dict,
    word &current
)
{
    bool triggered=false;
    const StateMachine &m=StateMachine::machine(machine);

    const word &state=m.stateName(m.currentState());
    word newDict=mapping.found(state) ? mapping[state] : "";
    if(newDict==current) {
        return true;
    }

    if(debug) {
        Info << dict.name() << " before:\n" << dict << endl;
    }

    Info << "Machine " << machine << " jumped to state " << state;

    if(newDict=="") {
        Info << " ... resetting to default" << endl;
        resetDict(dict);
    } else {
        Info << " ... setting to subdict " << newDict << endl;
        if(resetBeforeTrigger_) {
            Info << "First resetting " << dict.name() << endl;
            resetDict(dict);
        }
        dictionary newVals(dict.subDict(newDict));
        Info << "Merging into " << dict.name() << ":" << endl
            << newVals << endl;
        dict.merge(newVals);
    }

    current=newDict;

    if(debug) {
        Info << dict.name() << " after:\n" << dict << endl;
    }

    return true;
}

} // namespace Foam

// ************************************************************************* //
