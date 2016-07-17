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
    2008-2011, 2013-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "setDeltaTWithPythonFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(setDeltaTWithPythonFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        setDeltaTWithPythonFunctionObject,
        dictionary
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

setDeltaTWithPythonFunctionObject::setDeltaTWithPythonFunctionObject
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

bool setDeltaTWithPythonFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return timeManipulationWithPythonFunctionObject::read(dict);
}

void setDeltaTWithPythonFunctionObject::readParameters(const dictionary &dict)
{
    readCode(dict,"init",initCode_);
    readCode(dict,"deltaT",deltaTCode_);

    pythonInterpreterWrapper::executeCode(
        initCode_,
        false
    );
}

scalar setDeltaTWithPythonFunctionObject::deltaT()
{
    if(!parallelNoRun()) {
        setRunTime(time());
    }

    if(writeDebug()) {
        Pbug << "Evaluating " << deltaTCode_ << endl;
    }

    scalar result=evaluateCodeScalar(deltaTCode_,true);

    if(writeDebug()) {
        Pbug << "Evaluated to " << result << endl;
    }

    if(result!=time().deltaT().value()) {
        Info << "Changing timestep because " << deltaTCode_
            << " evaluated to " << result << "(current deltaT: "
            << time().deltaT().value() << " in " << name() << endl;
    }

    return result;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
