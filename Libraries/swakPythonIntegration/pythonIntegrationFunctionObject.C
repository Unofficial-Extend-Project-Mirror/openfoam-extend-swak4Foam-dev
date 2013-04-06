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
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "pythonIntegrationFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"
#include "IFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(pythonIntegrationFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        pythonIntegrationFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

pythonIntegrationFunctionObject::pythonIntegrationFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(name),
    pythonInterpreterWrapper(dict),
    time_(t)
{
    if(parallelNoRun()) {
        return;
    }

    initEnvironment(t);

    {
        PyObject *m = PyImport_AddModule("__main__");
        PyObject_SetAttrString(
            m,
            "functionObjectName",
            PyString_FromString(this->name().c_str())
        );
    }

    setRunTime();

    read(dict);
}

pythonIntegrationFunctionObject::~pythonIntegrationFunctionObject()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void pythonIntegrationFunctionObject::setRunTime()
{
    pythonInterpreterWrapper::setRunTime(time_);
}

bool pythonIntegrationFunctionObject::start()
{
    if(parallelNoRun()) {
        return true;
    }

    setRunTime();

    executeCode(startCode_,true);

    return true;
}

bool pythonIntegrationFunctionObject::execute(bool)
{
    if(parallelNoRun()) {
        return true;
    }

    setRunTime();

    executeCode(executeCode_,true);

    if(this->time_.outputTime()) {
        executeCode(writeCode_,true);
    }

    return true;
}

bool pythonIntegrationFunctionObject::end()
{
    if(parallelNoRun()) {
        return true;
    }

    setRunTime();

    executeCode(endCode_,true);

    return true;
}

bool pythonIntegrationFunctionObject::read(const dictionary& dict)
{
    if(parallelNoRun()) {
        return true;
    }

    readCode(dict,"start",startCode_);
    readCode(dict,"end",endCode_);
    readCode(dict,"execute",executeCode_);
    readCode(dict,"write",writeCode_,false);

    return true; // start();
}

} // namespace Foam

// ************************************************************************* //
