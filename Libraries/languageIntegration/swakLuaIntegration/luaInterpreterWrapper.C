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
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "luaInterpreterWrapper.H"
#include "addToRunTimeSelectionTable.H"

#include "IFstream.H"

#include "GlobalVariablesRepository.H"

#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#ifdef FOAM_HAS_STRINGOPS
#include "stringOps.H"
#endif

// #include <fcntl.h>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(luaInterpreterWrapper, 1);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

void luaInterpreterWrapper::initInteractiveSpecial() {
    initLuaPrompt();
}


luaInterpreterWrapper::luaInterpreterWrapper
(
    const objectRegistry& obr,
    const dictionary& dict,
    bool forceToNamespace
):
    generalInterpreterWrapperCRTP<luaInterpreterWrapper>(
        obr,
        dict,
        forceToNamespace,
        "lua"
    ),
    luaState_(NULL),
    useLuaPrompt_(dict.lookupOrDefault<bool>("useLuaPrompt",true)),
    hasLuaPrompt_(false)
{
    Pbug << "Starting constructor" << endl;

    syncParallel();

#ifdef FOAM_HAS_LOCAL_DEBUGSWITCHES
    debug=dict.lookupOrDefault<label>("debugLuaWrapper",debug());
#else
    debug=dict.lookupOrDefault<label>("debugLuaWrapper",debug);
#endif

    if(Pstream::parRun()) {
        Pbug << "This is a parallel run" << endl;

        parallelMasterOnly_=readBool(dict.lookup("parallelMasterOnly"));
    }

    if(parallelNoRun(true)) {
        Pbug << "Getting out because of 'parallelNoRun'" << endl;
        return;
    }


    Pbug << "Getting new interpreter" << endl;
    luaState_=luaL_newstate();
    luaL_openlibs(luaState_);
    Pbug << "Interpreter state: " << getHex(luaState_) << endl;

    if(
        (
            interactiveAfterExecute_
            ||
            interactiveAfterException_
        )
        &&
        useLuaPrompt_
    ) {
        initLuaPrompt();
    }
    //    interactiveLoop("Clean");


    Pbug << "End constructor" << endl;
 }

void luaInterpreterWrapper::initLuaPrompt() {
    Dbug << "Importing luaprompt library" << endl;
    int result=luaL_dostring(luaState_,"_prompt=require 'prompt'");
    if(result) {
        Dbug << "No prompt library" << endl;
        return;
    }
    hasLuaPrompt_=true;
    result=luaL_dostring(
        luaState_,
        "_prompt.colorize = true;"
        "_prompt.name = 'swakprompt';"
        "_prompt.history = os.getenv('HOME') .. '/.lua_history';"
        //        "_prompt.prompts = {'%  ', '%% '};"
    );
    Dbug << "Customizing prompt: " << result << endl;
}

void luaInterpreterWrapper::initEnvironment(const Time &t)
{
    assertParallel("initEnvironment");

    Pbug << "initEnvironment" << endl;

    setInterpreter();

    lua_pushstring(luaState_,"notAFunctionObject");
    lua_setglobal(luaState_,"functionObjectName");
    lua_pushstring(luaState_,getEnv("FOAM_CASE").c_str());
    lua_setglobal(luaState_,"caseDir");
    lua_pushstring(luaState_,(t.path()/t.caseSystem()).c_str());
    lua_setglobal(luaState_,"systemDir");
    lua_pushstring(luaState_,(t.path()/t.caseConstant()).c_str());
    lua_setglobal(luaState_,"constantDir");
    lua_pushstring(luaState_,(t.path()/t.constant()/"polyMesh").c_str());
    lua_setglobal(luaState_,"meshDir");
    if(Pstream::parRun()) {
        lua_pushstring(luaState_,t.path().c_str());
        lua_setglobal(luaState_,"procDir");
    }
    lua_pushboolean(luaState_,Pstream::parRun());
    lua_setglobal(luaState_,"parRun");
    lua_pushinteger(luaState_,Pstream::myProcNo());
    lua_setglobal(luaState_,"myProcNo");

    releaseInterpreter();
}

luaInterpreterWrapper::~luaInterpreterWrapper()
{
    if(parallelNoRun()) {
        return;
    }

    if(luaState_){
        lua_close(luaState_);
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void luaInterpreterWrapper::setRunTime(const Time &time)
{
    assertParallel("setRunTime");

    Pbug << "setRunTime " << time.timeName() << endl;

    setInterpreter();

    lua_pushnumber(luaState_,time.value());
    lua_setglobal(luaState_,"runTime");
    lua_pushnumber(luaState_,time.endTime().value());
    lua_setglobal(luaState_,"endTime");
    lua_pushnumber(luaState_,time.deltaT().value());
    lua_setglobal(luaState_,"deltaT");
    lua_pushboolean(luaState_,time.outputTime());
    lua_setglobal(luaState_,"outputTime");
    lua_pushstring(luaState_,time.timeName().c_str());
    lua_setglobal(luaState_,"timeName");
    lua_pushstring(luaState_,(time.path()/time.timeName()).c_str());
    lua_setglobal(luaState_,"timeDir");

    releaseInterpreter();
}

void luaInterpreterWrapper::setInterpreter()
{
    assertParallel("setInterpreter");

    Pbug << "setInterpreter" << endl;

    // TODO
}

void luaInterpreterWrapper::releaseInterpreter()
{
    assertParallel("releaseInterpreter");

    Pbug << "releaseInterpreter" << endl;

    // TODO
}

bool luaInterpreterWrapper::executeCodeInternal(
    const string &code
) {
    Dbug << "executeCodeInternal " << code << endl;

    int rcode=luaL_dostring(luaState_,code.c_str());

    Dbug << "Execution returned " << rcode << endl;

    if(rcode!=0) {
        printError();
    }
    return rcode==0;
}

bool luaInterpreterWrapper::executeCodeCaptureOutputInternal(
    const string &code,
    string &stdout
) {
    Dbug << "executeCodeCaptureOutputInternal " << code << endl;
    // TODO

    return false;
}

template <typename Result,class Func>
Result luaInterpreterWrapper::evaluateCodeInternal(
    const string &code,
    bool &success
)
{
    Dbug << "evaluateCodeInternal " << code << endl;

    Result result=pTraits<Result>::zero;
    success=false;

    int rcode=luaL_dostring(luaState_,code.c_str());

    Dbug << "Execution returned " << rcode << endl;

    if(rcode==0) {
        result=Func()(luaState_);
        Dbug << "Result: " << result << endl;
        success=true;
    } else {
        printError();
    }
    return result;
}

void luaInterpreterWrapper::printError(){
    string error(lua_tostring(luaState_,-1));
    Info << nl << "Lua error: " << nl << error << nl << endl;
}

void luaInterpreterWrapper::interactiveLoop(
    const string &banner
) {
    Dbug << "interactiveLoop" << endl;

    string cmd="print('"+banner+"\\n')";
    Dbug << "Executing " << cmd.c_str() << endl;
    int fail=luaL_dostring(luaState_,cmd.c_str());
    Dbug << "Printing failed: " << fail << endl;
    if(hasLuaPrompt_) {
        fail=luaL_dostring(
            luaState_,
            "describe=_prompt.describe;"
            "print('Use the describe function to get info about symbols. describe(_G) for everything');"
            "print('Exit with Ctrl-D');"
            "_prompt.enter()"
        );
    } else {
        fail=luaL_dostring(
            luaState_,
            "print('Exit with Ctrl-D');"
            "debug.debug()"
        );
    }
    Dbug << "Interactive failed " << fail << endl;
}

void luaInterpreterWrapper::doAfterExecution(
    bool fail,
    const string &code,
    bool putVariables,
    bool failOnException
)
{
    if(fail) {
        Info << "Lua Exception" << endl;
        // TODO
    }

    if(
        interactiveAfterException_
        &&
        fail
    ) {
        Info << "Got an exception for "<< code
            << " now you can interact." << endl;
        interactiveLoop("Exception handling");
    }
    if(
        fail
        &&
        (
            !tolerateExceptions_
            ||
            failOnException
        )
    ) {
        FatalErrorIn("luaInterpreterWrapper::doAfterExecution")
            << "Lua exception raised by " << nl
	    << code << endl
	    << "To debug set 'interactiveAfterException true;' in " << dict().name()
                << endl << exit(FatalError);
    }

    if(interactiveAfterExecute_) {
        Info << "Executed "<< code
            << " now you can interact." << endl;
        interactiveLoop("After execution");
    }

    if(putVariables) {
        setGlobals();
    }
}

void luaInterpreterWrapper::getGlobals()
{
    assertParallel("getGlobals");

    if(swakToInterpreterNamespaces_.size()==0) {
        return;
    }

    Pbug << "Getting global variables from namespaces "
        << swakToInterpreterNamespaces_ << endl;

    forAll(swakToInterpreterNamespaces_,nameI) {
        Pbug << "Namespace: " << swakToInterpreterNamespaces_[nameI] << endl;

        const GlobalVariablesRepository::ResultTable &vars=
            GlobalVariablesRepository::getGlobalVariables(
                obr()
            ).getNamespace(
               swakToInterpreterNamespaces_[nameI]
            );
        forAllConstIter(
            GlobalVariablesRepository::ResultTable,
            vars,
            iter
        ) {
            const word &var=iter.key();
            const ExpressionResult &value=*(*iter);

            Pbug << "Variable: " << var << endl;

            // TODO

            Pbug << "Variable done" << endl;
        }
        Pbug << "Namespace done" << endl;
    }
    Dbug << "End getGlobals" << endl;
}

void luaInterpreterWrapper::setGlobals()
{
    assertParallel("setGlobals");

    if(interpreterToSwakVariables_.size()==0) {
        return;
    }

    Dbug << "Writing variables " << interpreterToSwakVariables_
        << " to namespace " << interpreterToSwakNamespace_ << endl;

    forAll(interpreterToSwakVariables_,i) {
        const word &name=interpreterToSwakVariables_[i];

        Dbug << "Getting variable "<< name << endl;

        // TODO
    }
}

} // namespace Foam

// ************************************************************************* //
