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
    2011-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "writeIfPythonFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(writeIfPythonFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        writeIfPythonFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeIfPythonFunctionObject::writeIfPythonFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    conditionDrivenWritingFunctionObject(name,t,dict),
    pythonInterpreterWrapper(
        t.db(),
        dict
    )
{
    if(!parallelNoRun()) {
        initEnvironment(t);

        setRunTime(t);
    }

    readParameters(dict);
}

bool writeIfPythonFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return conditionDrivenWritingFunctionObject::read(dict);
}

void writeIfPythonFunctionObject::readParameters(const dictionary &dict)
{
    readCode(dict,"initVariables",initCode_);
    readCode(dict,"startWrite",writeCode_);
    if(writeControlMode()==scmWriteUntilSwitch) {
        readCode(dict,"stopWrite",stopWriteCode_);
    }
    if(cooldownMode()==cdmRetrigger) {
        readCode(dict,"stopCooldown",stopCooldownCode_);
    }

    pythonInterpreterWrapper::executeCode(
        initCode_,
        false
    );
}

bool writeIfPythonFunctionObject::executeCode(const string code)
{
    if(!parallelNoRun()) {
        setRunTime(time());
    }

    if(writeDebug()) {
        Info << "Evaluating " << code << endl;
    }
    bool result=evaluateCodeTrueOrFalse(code,true);
    if(writeDebug()) {
        Info << "Evaluated to " << result << endl;
    }

    return result;
}

bool writeIfPythonFunctionObject::checkStartWriting()
{
    return executeCode(writeCode_);
}

bool writeIfPythonFunctionObject::checkStopWriting()
{
    return executeCode(stopWriteCode_);
}

bool writeIfPythonFunctionObject::checkStopCooldown()
{
    return executeCode(stopCooldownCode_);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
