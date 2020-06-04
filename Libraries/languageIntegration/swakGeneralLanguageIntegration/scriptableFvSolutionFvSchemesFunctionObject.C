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
    2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: scriptableFvSolutionFvSchemesFunctionObject.C,v 6277e2c3db14 2018-03-20 19:32:05Z bgschaid $
\*---------------------------------------------------------------------------*/

#include "scriptableFvSolutionFvSchemesFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(scriptableFvSolutionFvSchemesFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        scriptableFvSolutionFvSchemesFunctionObject,
        dictionary
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

scriptableFvSolutionFvSchemesFunctionObject::scriptableFvSolutionFvSchemesFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    manipulateFvSolutionFvSchemesFunctionObject(name,t,dict),
    interpreter_(
        generalInterpreterWrapper::New(
            this->obr(),
            dict
        )
    ),
    schemesFromDict_(
        dict.lookup("schemesFromDict")
    ),
    dictFromSchemes_(
        dict.lookup("dictFromSchemes")
    ),
    solutionFromDict_(
        dict.lookup("solutionFromDict")
    ),
    dictFromSolution_(
        dict.lookup("dictFromSolution")
    )
{
    interpreter().readCode(
        dict,
        "init",
        initCode_
    );
    interpreter().readCode(
        dict,
        "schemes",
        schemesCode_
    );
    interpreter().readCode(
        dict,
        "solution",
        solutionCode_
    );

    Info << name << " initializing" << endl;
    interpreter().executeCode(
        initCode_,
        true // set global variables if necessary
    );
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool scriptableFvSolutionFvSchemesFunctionObject::manipulateFvSolution(const Time &t)
{
    return executeCodeWriteToDict(
        solutionCode_,
        dictFromSolution_,
        solutionFromDict_,
        fvSolutionDict()
    );
}

bool scriptableFvSolutionFvSchemesFunctionObject::manipulateFvSchemes(const Time &t)
{
    return executeCodeWriteToDict(
        schemesCode_,
        dictFromSchemes_,
        schemesFromDict_,
        fvSchemesDict()
    );
}


bool scriptableFvSolutionFvSchemesFunctionObject::executeCodeWriteToDict(
    const string code,
    const word &toDict,
    const word &fromDict,
    dictionary &dict
)
{
    Dbug << "Setting from " << fromDict << " to " << toDict
        << " : " << dict.name() << endl;

    bool triggered=false;

    interpreter().insertDictionary(
        toDict,
        dict
    );

    interpreter().executeCode(
        code,
        true
    );

    dictionary newValues;

    interpreter().extractDictionary(
        fromDict,
        newValues
    );

    Dbug << "New values " << newValues << endl;

    triggered=dict.merge(newValues);
    if(triggered) {
        Info << "Changed " << dict.name() << endl;
    }

    Dbug << "Result " << dict << endl;

    return triggered;
}

generalInterpreterWrapper &scriptableFvSolutionFvSchemesFunctionObject::interpreter()
{
    return interpreter_();
}

} // namespace Foam

// ************************************************************************* //
