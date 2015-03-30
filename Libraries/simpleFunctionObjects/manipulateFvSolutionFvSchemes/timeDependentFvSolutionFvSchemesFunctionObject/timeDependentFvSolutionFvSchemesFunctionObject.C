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

Contributors/Copyright:
    2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "timeDependentFvSolutionFvSchemesFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(timeDependentFvSolutionFvSchemesFunctionObject, 1);

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

bool timeDependentFvSolutionFvSchemesFunctionObject::manipulate(const Time &t)
{
    triggerTrigger(
        t,
        solutionTriggers_,
        fvSolutionDict(),
        currentSolutionTrigger_
    );
    triggerTrigger(
        t,
        schemesTriggers_,
        fvSchemesDict(),
        currentSchemesTrigger_
    );
    return true;
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
        if(!dict.isDict(triggers[i].second())) {
            FatalErrorIn("timeDependentFvSolutionFvSchemesFunctionObject::checkTriggerList")
                << "No subdictionary " << triggers[i].second()
                    << " in " << dict.name()
                    << endl
                    << exit(FatalError);
        }
    }
}

void timeDependentFvSolutionFvSchemesFunctionObject::triggerTrigger(
    const Time &t,
    const List<TriggerTimeName> &triggers,
    dictionary &dict,
    label &current
)
{
    forAll(triggers,i) {
        if(current>=i) {
            // already triggered this
            continue;
        }
        if(t.value()<triggers[i].first()) {
            // have not reached this trigger
            break;
        }
        Info << "Reached trigger " << triggers[i].second()
            << " for t=" << triggers[i].first()
            << " of " << dict.name() << endl;
        if(resetBeforeTrigger_) {
            Info << "First resetting " << dict.name() << endl;
            resetDict(dict);
        }
        dictionary newVals(dict.subDict(triggers[i].second()));
        if(debug) {
            Info << dict.name() << " before:\n" << dict << endl;
        }
        Info << "Merging into " << dict.name() << ":" << endl
            << newVals << endl;
        dict.merge(newVals);
        if(debug) {
            Info << dict.name() << " after:\n" << dict << endl;
        }
        current=i;
    }
}

} // namespace Foam

// ************************************************************************* //
