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
    2008-2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "setEndTimeWithPythonFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(setEndTimeWithPythonFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        setEndTimeWithPythonFunctionObject,
        dictionary
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

setEndTimeWithPythonFunctionObject::setEndTimeWithPythonFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    timeManipulationWithPythonFunctionObject(name,t,dict)
{
    readParameters(dict);
}

bool setEndTimeWithPythonFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return timeManipulationWithPythonFunctionObject::read(dict);
}

void setEndTimeWithPythonFunctionObject::readParameters(const dictionary &dict)
{
    readCode(dict,"init",initCode_);
    readCode(dict,"endTime",endTimeCode_);

    pythonInterpreterWrapper::executeCode(
        initCode_,
        false
    );
}

scalar setEndTimeWithPythonFunctionObject::endTime()
{
    if(!parallelNoRun()) {
        setRunTime(time());
    }

    if(writeDebug()) {
        Pbug << "Evaluating " << endTimeCode_ << endl;
    }
    scalar result=evaluateCodeScalar(endTimeCode_,true);
    if(writeDebug()) {
        Pbug << "Evaluated to " << result << endl;
    }

    if(result!=time().endTime().value()) {
        Info << "Changing end time because " << endTimeCode_
            << " evaluated to " << result << "(current endTime: "
            << time().endTime().value() << " in " << name() << endl;
    }

    return result;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
