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

Contributors/Copyright:
    2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "timeDependentFvSolutionFvSchemesFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(timeDependentFvSolutionFvSchemesFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        timeDependentFvSolutionFvSchemesFunctionObject,
        dictionary
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

timeDependentFvSolutionFvSchemesFunctionObject::timeDependentFvSolutionFvSchemesFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    manipulateFvSolutionFvSchemesFunctionObject(name,t,dict),
    solutionTriggers_(dict.lookup("solutionTriggers")),
    currentSolutionTrigger_(-1),
    schemesTriggers_(dict.lookup("schemesTriggers")),
    currentSchemesTrigger_(-1),
    resetBeforeTrigger_(
        readBool(dict.lookup("resetBeforeTrigger"))
    )
{
    checkTriggerList(
        solutionTriggers_,
        fvSolutionDict()
    );
    checkTriggerList(
        schemesTriggers_,
        fvSchemesDict()
    );
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool timeDependentFvSolutionFvSchemesFunctionObject::manipulateFvSolution(const Time &t)
{
    return triggerTrigger(
        t,
        solutionTriggers_,
        fvSolutionDict(),
        currentSolutionTrigger_
    );
}

bool timeDependentFvSolutionFvSchemesFunctionObject::manipulateFvSchemes(const Time &t)
{
    return triggerTrigger(
        t,
        schemesTriggers_,
        fvSchemesDict(),
        currentSchemesTrigger_
    );
}

void timeDependentFvSolutionFvSchemesFunctionObject::checkTriggerList(
    const List<TriggerTimeName> &triggers,
    const dictionary &dict
)
{
    forAll(triggers,i) {
        if(i>0) {
            scalar now=triggers[i].first();
            scalar old=triggers[i-1].first();
            if(now <= old) {
                FatalErrorIn("timeDependentFvSolutionFvSchemesFunctionObject::checkTriggerList")
                    << "Checking triggers for " << dict.name() << endl
                        << "Times must me ordered but for " << i
                        << "the entry " << now << " is less than " << old
                        << endl
                        << exit(FatalError);
            }
        }
        word name(triggers[i].second());
        if(
            name != "reset"
            &&
            !dict.isDict(name)
        ){
            FatalErrorIn("timeDependentFvSolutionFvSchemesFunctionObject::checkTriggerList")
                << "No subdictionary " << triggers[i].second()
                    << " in " << dict.name()
                    << endl
                    << exit(FatalError);
        }
    }
}

bool timeDependentFvSolutionFvSchemesFunctionObject::triggerTrigger(
    const Time &t,
    const List<TriggerTimeName> &triggers,
    dictionary &dict,
    label &current
)
{
    bool triggered=false;

    forAll(triggers,i) {
        if(current>=i) {
            // already triggered this
            continue;
        }
        if(t.value()<triggers[i].first()) {
            // have not reached this trigger
            break;
        }
        word name(triggers[i].second());
        Info << "Reached trigger " << name
            << " for t=" << triggers[i].first()
            << " of " << dict.name() << endl;

        triggered=true;

        if(
            resetBeforeTrigger_
            &&
            name != "reset"
        ) {
            Info << "First resetting " << dict.name() << endl;
            resetDict(dict);
        }
        if(debug) {
            Info << dict.name() << " before:\n" << dict << endl;
        }
        if(name!="reset") {
            dictionary newVals(dict.subDict(triggers[i].second()));
            Info << "Merging into " << dict.name() << ":" << endl
                << newVals << endl;
            dict.merge(newVals);
        } else {
            Info << "Resetting " << dict.name() << endl;
            resetDict(dict);
        }
        if(debug) {
            Info << dict.name() << " after:\n" << dict << endl;
        }
        current=i;
    }

    return triggered;
}

} // namespace Foam

// ************************************************************************* //
