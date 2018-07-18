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
    2011-2013, 2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "luaIntegrationFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"
#include "IFstream.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(luaIntegrationFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        luaIntegrationFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

luaIntegrationFunctionObject::luaIntegrationFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(
        name,
        t,
        dict
    ),
    luaInterpreterWrapper(
        t.db(),
        dict
    ),
    time_(t)
{
    Pbug << "Constructor" << endl;

    if(!parallelNoRun()) {
        initEnvironment(t);

        setInterpreter();

        lua_pushstring(luaState(),this->name().c_str());
        lua_setglobal(luaState(),"functionObjectName");

        releaseInterpreter();

        setRunTime();
    }

    read(dict);
}

luaIntegrationFunctionObject::~luaIntegrationFunctionObject()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void luaIntegrationFunctionObject::setRunTime()
{
    Pbug << "setRunTime" << endl;

    luaInterpreterWrapper::setRunTime(time_);
}

bool luaIntegrationFunctionObject::start()
{
    Pbug << "start" << endl;

    simpleFunctionObject::start();

    if(!parallelNoRun()) {
        setRunTime();
    }

    dictionariesToInterpreterStructs();

    bool ok=executeCode(startCode_,true);

    interpreterStructsToDictionaries();

    return ok;

}

void luaIntegrationFunctionObject::writeSimple()
{
    Pbug << "writeSimple" << endl;

    if(!parallelNoRun()) {
        setRunTime();
    }

    dictionariesToInterpreterStructs();

    executeCode(executeCode_,true);

    if(this->time_.outputTime()) {
        executeCode(writeCode_,true);
    }

    interpreterStructsToDictionaries();
}

bool luaIntegrationFunctionObject::end()
{
    Pbug << "end" << endl;

    if(!parallelNoRun()) {
        setRunTime();
    }

    dictionariesToInterpreterStructs();

    bool ok=executeCode(endCode_,true);

    interpreterStructsToDictionaries();

    return ok;
}

bool luaIntegrationFunctionObject::read(const dictionary& dict)
{
    Pbug << "read" << endl;

    readCode(dict,"start",startCode_);
    readCode(dict,"end",endCode_);
    readCode(dict,"execute",executeCode_);
    readCode(dict,"write",writeCode_,false);

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    return start();
#else
    return true;
#endif
}

} // namespace Foam

// ************************************************************************* //
