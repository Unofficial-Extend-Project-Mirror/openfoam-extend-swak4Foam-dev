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

#include "writeAndEndPythonFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(writeAndEndPythonFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        writeAndEndPythonFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeAndEndPythonFunctionObject::writeAndEndPythonFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    writeAndEndFunctionObject(name,t,dict),
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

bool writeAndEndPythonFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return writeAndEndFunctionObject::read(dict);
}

void writeAndEndPythonFunctionObject::readParameters(const dictionary &dict)
{
    readCode(dict,"init",initCode_);
    readCode(dict,"condition",conditionCode_);

    pythonInterpreterWrapper::executeCode(
        initCode_,
        false
    );
}

bool writeAndEndPythonFunctionObject::endRunNow()
{
    if(!parallelNoRun()) {
        setRunTime(time());
    }

    if(writeDebug()) {
        Info << "Evaluating " << conditionCode_ << endl;
    }
    bool result=evaluateCodeTrueOrFalse(conditionCode_,true);
    if(writeDebug()) {
        Info << "Evaluated to " << result << endl;
    }

    if(result) {
        Info << "Stopping because python code  " << conditionCode_
            << " evaluated to 'true' in " << name() << endl;
    }

    return result;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
