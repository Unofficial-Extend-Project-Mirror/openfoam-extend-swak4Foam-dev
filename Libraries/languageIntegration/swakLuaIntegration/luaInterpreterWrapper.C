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
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "luaInterpreterWrapper.H"
#include "LuaFoamDictionaryParserDriver.H"

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

#include "addToRunTimeSelectionTable.H"
// #include <fcntl.h>

#include "functions4Lua.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(luaInterpreterWrapper, 0);
    addNamedToRunTimeSelectionTable(generalInterpreterWrapper,luaInterpreterWrapper,dictionary,lua);

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
    useLuaPrompt_(
        dict.lookupOrDefault<bool>("useLuaPrompt",true)
    ),
    hasLuaPrompt_(false),
    parseToWrappedField_(
        dict.lookupOrDefault<bool>("parseToWrappedField",true)
    ),
    wrapBoolLabelAsScalar_(
        dict.lookupOrDefault<bool>("wrapBoolLabelAsScalar",true)
    )
{
    if(generalInterpreterWrapper::debug>debug) {
        debug=1;
    }

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

    // extending the Lua-interpreter
    addOpenFOAMFunctions(luaState_);

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


    Pbug << "End constructor: Size of Lua-stack "
        << lua_gettop(luaState_) << endl ;
 }

void luaInterpreterWrapper::write(Ostream &os) const
{
    generalInterpreterWrapper::write(os);

    os.writeKeyword("debugLuaWrapper")
#ifdef FOAM_HAS_LOCAL_DEBUGSWITCHES
        << debug()
#else
        << debug
#endif
        << token::END_STATEMENT << nl;

    os.writeKeyword("useLuaPrompt")
        << useLuaPrompt_ << token::END_STATEMENT << nl;
    os.writeKeyword("parseToWrappedField")
        << parseToWrappedField_ << token::END_STATEMENT << nl;
    os.writeKeyword("wrapBoolLabelAsScalar")
        << wrapBoolLabelAsScalar_ << token::END_STATEMENT << nl;
}

void luaInterpreterWrapper::initLuaPrompt() {
    Dbug << "Importing luaprompt library: Size of Lua-stack "
        << lua_gettop(luaState_) << endl;
    int result=luaL_dostring(luaState_,"_prompt=require 'prompt'");
    if(result) {
        Dbug << "No prompt library: Size of Lua-stack "
            << lua_gettop(luaState_) << endl;
        lua_pop(luaState_,1);
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
    lua_pushnumber(luaState_,time.deltaT0().value());
    lua_setglobal(luaState_,"deltaT0");
    lua_pushboolean(luaState_,time.outputTime());
    lua_setglobal(luaState_,"outputTime");
    lua_pushstring(luaState_,time.timeName().c_str());
    lua_setglobal(luaState_,"timeName");
    lua_pushstring(luaState_,(time.path()/time.timeName()).c_str());
    lua_setglobal(luaState_,"timeDir");
    lua_pushinteger(luaState_,time.timeIndex());
    lua_setglobal(luaState_,"timeIndex");

    releaseInterpreter();
}

void luaInterpreterWrapper::setInterpreter()
{
    assertParallel("setInterpreter");

    Pbug << "setInterpreter: Size of Lua-stack "
        << lua_gettop(luaState_) << endl ;
}

void luaInterpreterWrapper::releaseInterpreter()
{
    assertParallel("releaseInterpreter");

    Pbug << "releaseInterpreter: Size of Lua-stack "
        << lua_gettop(luaState_) << endl ;

    if(lua_gettop(luaState_)>0) {
        FatalErrorIn("releaseInterpreter")
            << "Stack must be 0 but is " << lua_gettop(luaState_) << nl
                << "This is not your fault. Problem in the implementation of luaInterpreterWrapper"
                << endl
                << exit(FatalError);
    }
}

bool luaInterpreterWrapper::executeCodeInternal(
    const string &code
) {
    Dbug << "executeCodeInternal " << code << endl;

    int rcode=luaL_dostring(luaState_,code.c_str());

    Dbug << "Execution returned " << rcode << endl;

    if(rcode!=0) {
        //   printError();
        Info << "Lua Error!" << endl;
    }
    return rcode==0;
}

bool luaInterpreterWrapper::executeCodeCaptureOutputInternal(
    const string &code,
    string &stdout
) {
    Dbug << "executeCodeCaptureOutputInternal " << code << endl;

    string redirectCode=
        "_oldPrint=print\n"
        "_stdoutBuf=''\n"
        "print=function(...)\n"
        "    for i,val in ipairs({...}) do\n"
        "        if i~=1 then\n"
        "            _stdoutBuf=_stdoutBuf..'\\t'\n"
        "        end\n"
        "        _stdoutBuf=_stdoutBuf..tostring(val)\n"
        "    end\n"
        "    _stdoutBuf=_stdoutBuf..'\\n'\n"
        "end\n";
    int rcode=luaL_dostring(luaState_,redirectCode.c_str());
    Dbug << "Redirect worked" << rcode << endl;

    rcode=luaL_dostring(luaState_,code.c_str());

    Dbug << "Execution returned " << rcode << endl;

    if(rcode!=0) {
        //        printError();
    }

    int type=lua_getglobal(luaState_,"_stdoutBuf");
    Dbug << "Getting _stdoutBuf " << type << endl;
    stdout=string(lua_tostring(luaState_,-1));
    Dbug << "Output: " << stdout << endl;
    lua_pop(luaState_,1);
    int r=luaL_dostring(luaState_,"print=_oldPrint;_oldPrint=nil;_stdoutBuf=nil");
    Dbug << "Clearing " << r << endl;

    return rcode==0;
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
        lua_pop(luaState_,1);
    } else {
        Info << "Lua Error!" << endl;
        // printError();
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

        printError();
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

template<class T>
void luaInterpreterWrapper::vectorSpaceToStack(const T &val)
{
    lua_newtable(luaState_);
    for (label cmpt=0; cmpt<pTraits<T>::nComponents; cmpt++) {
        lua_pushnumber(luaState_,val[cmpt]);
        lua_setfield(
            luaState_,
            -2,
            pTraits<T>::componentNames[cmpt]
        );
    }
}

template<class T>
void luaInterpreterWrapper::vectorSpaceArrayToStack(const Field<T> &f)
{
    lua_newtable(luaState_);
    forAll(f,i) {
        vectorSpaceToStack(f[i]);
        lua_seti(luaState_,-2,i+1);
    }
}

template<class T>
autoPtr<Field<T> > luaInterpreterWrapper::stackToVectorSpaceArray(label length)
{
    autoPtr<Field<T> > pf(new Field<T>(length));
    Field<T> &f=pf();

    forAll(f,i) {
        lua_geti(
            luaState_,
            -1,
            i+1
        );
        f[i]=stackToVectorSpace<T>();
        lua_pop(luaState_,1);
    }

    return pf;
}

template<class T>
T luaInterpreterWrapper::stackToVectorSpace()
{
    T result=pTraits<T>::zero;
    for (label cmpt=0; cmpt<pTraits<T>::nComponents; cmpt++) {
        lua_getfield(
            luaState_,
            -1,
            pTraits<T>::componentNames[cmpt]
        );
        result[cmpt]=lua_tonumber(luaState_,-1);
        lua_pop(luaState_,1);
    }
    return result;
}

template<class T>
bool luaInterpreterWrapper::isVectorSpace()
{
    for (label cmpt=0; cmpt<pTraits<T>::nComponents; cmpt++) {
        int type=lua_getfield(
            luaState_,
            -1,
            pTraits<T>::componentNames[cmpt]
        );
        lua_pop(luaState_,1);
        if(
            type!=LUA_TNUMBER
            &&
            type!=LUA_TBOOLEAN
        ) {
            return false;
        }
    }
    return true;
}

void luaInterpreterWrapper::resultToLuaVariable(
    const word &var,
    const word &namespaceName,
    const ExpressionResult &value,
    bool noDict,
    bool forceNative
) {
    if(value.isSingleValue()) {
        ExpressionResult val=value.getUniform(
            1,
            !warnOnNonUniform_,
            !Pstream::parRun() || !parallelMasterOnly_
        );

        Dbug << var << " is a single value" << endl;
        if(val.valueType()==pTraits<scalar>::typeName) {
            lua_pushnumber(luaState_,val.getResult<scalar>()()[0]);
            lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
        } else if(val.valueType()==pTraits<vector>::typeName) {
            vectorSpaceToStack(val.getResult<vector>()()[0]);
            lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
        } else if(val.valueType()==pTraits<tensor>::typeName) {
            vectorSpaceToStack(val.getResult<tensor>()()[0]);
            lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
        } else if(val.valueType()==pTraits<symmTensor>::typeName) {
            vectorSpaceToStack(val.getResult<symmTensor>()()[0]);
            lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
        } else if(val.valueType()==pTraits<sphericalTensor>::typeName) {
            vectorSpaceToStack(val.getResult<sphericalTensor>()()[0]);
            lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
        } else {
            WarningIn("luaInterpreterWrapper::getGlobals()")
                << "Single variable " << var << " in "
                    << namespaceName
                    << " is of unsupported type "
                    << val.valueType()
                    << endl;
        }
    } else {
        Dbug << var << " is an array" << endl;
        bool byReference=forceNative;
        if(!noDict) {
            byReference=readBool(dict().lookup("referenceToGlobal"));
        }

        if(value.valueType()==pTraits<scalar>::typeName) {
            if(byReference){
                addFieldToLua(
                    luaState_,
                    var,
                    &value.getRef<scalar>()
                );
            } else {
                const Field<scalar> f(const_cast<ExpressionResult&>(
                    value
                ).getResult<scalar>(true));
                lua_newtable(luaState_);
                forAll(f,i) {
                    lua_pushnumber(luaState_,f[i]);
                    lua_seti(luaState_,-2,i+1);
                }
                lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
            }
        } else if(value.valueType()==pTraits<vector>::typeName) {
            if(byReference){
                addFieldToLua(
                    luaState_,
                    var,
                    &value.getRef<vector>()
                );
            } else {
                const Field<vector> f(const_cast<ExpressionResult&>(
                    value
                ).getResult<vector>(true));
                vectorSpaceArrayToStack(f);
                lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
            }
        } else if(value.valueType()==pTraits<tensor>::typeName) {
            if(byReference){
                addFieldToLua(
                    luaState_,
                    var,
                    &value.getRef<tensor>()
                );
            } else {
                const Field<tensor> f(const_cast<ExpressionResult&>(
                    value
                ).getResult<tensor>(true));
                vectorSpaceArrayToStack(f);
                lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
            }
        } else if(value.valueType()==pTraits<symmTensor>::typeName) {
            if(byReference){
                addFieldToLua(
                    luaState_,
                    var,
                    &value.getRef<symmTensor>()
                );
            } else {
                const Field<symmTensor> f(const_cast<ExpressionResult&>(
                    value
                ).getResult<symmTensor>(true));
                vectorSpaceArrayToStack(f);
                lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
            }
        } else if(value.valueType()==pTraits<sphericalTensor>::typeName) {
            if(byReference){
                addFieldToLua(
                    luaState_,
                    var,
                    &value.getRef<sphericalTensor>()
                );
            } else {
                const Field<sphericalTensor> f(const_cast<ExpressionResult&>(
                    value
                ).getResult<sphericalTensor>(true));
                vectorSpaceArrayToStack(f);
                lua_setglobal(luaState_,const_cast<char*>(var.c_str()));
            }
        } else {
            WarningIn("luaInterpreterWrapper::getGlobals()")
                << "Array variable " << var << " in "
                    << namespaceName
                    << " is of unsupported type "
                    << value.valueType()
                    << endl;
        }
    }
}

void luaInterpreterWrapper::getVariablesFromDriver(
    CommonValueExpressionDriver &driver,
    const wordList &names
) {
    forAll(names,i) {
        const word &var=names[i];
        const ExpressionResult &value=const_cast<const CommonValueExpressionDriver&>(driver).variable(var);

        resultToLuaVariable(
            var,
            "variables",
            value,
            true
        );
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

            resultToLuaVariable(
                var,
                swakToInterpreterNamespaces_[nameI],
                value
            );


            Pbug << "Variable done" << endl;
        }
        Pbug << "Namespace done" << endl;
    }
    Dbug << "End getGlobals" << endl;
}

class LuaToScalar {
public:
    scalar operator()(lua_State *L) {
        return lua_tonumber(L,-1);
    }
};

class LuaToBool {
public:
    bool operator()(lua_State *L) {
        return lua_toboolean(L,-1);
    }
};

class LuaToString {
public:
    string operator()(lua_State *L) {
        return string(lua_tostring(L,-1));
    }
};

class luaInterpreterWrapper::ValidOk {
public:
    template<class T>
    bool operator()(T &t) {
        return true;
    }
};

template<class T,class F,class V>
bool luaInterpreterWrapper::isListListOnStack(int lType) {
    label i=1;
    while(1) {
        int type=lua_geti(
            luaState_,
            -1,
            i
        );
        if(type!=LUA_TTABLE) {
            lua_pop(luaState_,1);
            break;
        }
        label j=1;
        while(1) {
            int type=lua_geti(
                luaState_,
                -1,
                j
            );
            if(
                type==LUA_TNIL
            ) {
                lua_pop(luaState_,1);
                break;
            } else if (
                type==LUA_TTABLE
            ) {
                lua_pop(luaState_,2);
                WarningIn("luaInterpreterWrapper::isListListOnStack")
                    << "Problem : got an unexpected table"
                        << endl;
                return false;
            }
            T val(F()(luaState_));
            if(
                !V()(val)
                !=
                (
                    lType!=LUA_TNIL
                    &&
                    type!=lType
                )
            ) {
                lua_pop(luaState_,2);
                return false;
            }
            lua_pop(luaState_,1);
            j++;
        }
        i++;
        lua_pop(luaState_,1);
    }
    return true;
}

template<class T,class F>
void luaInterpreterWrapper::addListListFromStackToDict(
    dictionary &dict,
    const word &name
) {
    DynamicList<DynamicList<T> > result;
    label i=1;
    while(1) {
        int type=lua_geti(
            luaState_,
            -1,
            i
        );
        if(type==LUA_TNIL) {
            lua_pop(luaState_,1);
            break;
        }
        int j=1;
        DynamicList<T> lst;
        while(1) {
            int type=lua_geti(
                luaState_,
                -1,
                j
            );
            if(type==LUA_TNIL) {
                lua_pop(luaState_,1);
                break;
            }
            T val(F()(luaState_));
            lst.append(val);
            lua_pop(luaState_,1);
            j++;
        }
        result.append(
            lst
        );
        lua_pop(luaState_,1);
        i++;
    }
    dict.set(name,result);
}

template<class T,class F,class V>
bool luaInterpreterWrapper::addListFromStackToDict(
    dictionary &dict,
    const word &name
) {
    DynamicList<T> result;
    label cnt=1;
    while(1) {
        int type=lua_geti(
            luaState_,
            -1,
            cnt
        );
        if(type==LUA_TNIL) {
            lua_pop(luaState_,1);
            break;
        }
        if(type==LUA_TTABLE) {
            lua_pop(luaState_,1);
            WarningIn("luaInterpreterWrapper::addListFromStackToDict")
                << "Problem in " << name << ": got an unexpected table"
                    << endl;
            return false;
        }
        T val(F()(luaState_));
        if(!V()(val)) {
            lua_pop(luaState_,1);
            return false;
        }
        result.append(
            val
        );
        lua_pop(luaState_,1);
        cnt++;
    }
    dict.set(name,result);
    return true;
}

void luaInterpreterWrapper::extractTopToDict(dictionary &dict) {
    Dbug << "luaInterpreterWrapper::extractTopToDict for "
        << dict.name() << endl;

    lua_pushnil(luaState_);
    Dbug << "Stack before loop " << lua_gettop(luaState_) << endl;
    while(lua_next(luaState_,-2)) {
        Dbug << "Next entry. " << lua_gettop(luaState_) << endl;
        word name(lua_tostring(luaState_,-2));
        Dbug << "Entry: " << name << endl;
        int type=lua_type(luaState_,-1);
        Dbug << "Type is " << type << " -> "
            << lua_typename(luaState_,type) << endl;
        switch(type) {
            case LUA_TNIL:
                WarningIn("luaInterpreterWrapper::extractTopToDict()")
                    << "No symbol " << name << " in Lua namespace"
                        << endl;
                break;
            case LUA_TNUMBER:
                dict.set(name,lua_tonumber(luaState_,-1));
                break;
            case LUA_TBOOLEAN:
                dict.set(name,bool(lua_toboolean(luaState_,-1)));
                break;
            case LUA_TSTRING:
                {
                    string val(lua_tostring(luaState_,-1));
                    if(ValidWord()(val)) {
                        dict.set(name,word(val));
                    } else {
                        dict.set(name,val);
                    }
                }
                break;
            case LUA_TTABLE:
                {
                    int itype=lua_geti(
                        luaState_,
                        -1,
                        1
                    );
                    lua_pop(luaState_,1);
                    if(
                        itype!=LUA_TNIL
                    ) {
                        Dbug << name << " is an array" << endl;
                        switch(itype) {
                            case LUA_TNUMBER:
                                addListFromStackToDict<scalar,LuaToScalar,ValidOk>(
                                    dict,
                                    name
                                );
                                break;
                            case LUA_TBOOLEAN:
                                addListFromStackToDict<bool,LuaToBool,ValidOk>(
                                    dict,
                                    name
                                );
                                break;
                            case LUA_TSTRING:
                                {
                                    bool ok=addListFromStackToDict<word,LuaToString,ValidWord>(
                                        dict,
                                        name
                                    );
                                    if(!ok) {
                                        addListFromStackToDict<string,LuaToString,ValidOk>(
                                            dict,
                                            name
                                        );
                                    }
                                }
                                break;
                            case LUA_TTABLE:
                                if(isListListOnStack<bool,LuaToBool,ValidOk>(LUA_TBOOLEAN)) {
                                    addListListFromStackToDict<bool, LuaToBool>(
                                        dict,
                                        name
                                    );
                                } else if(isListListOnStack<word,LuaToString,ValidWord>(
                                    LUA_TSTRING
                                )) {
                                    addListListFromStackToDict<word, LuaToString>(
                                        dict,
                                        name
                                    );
                                } else if(isListListOnStack<string,LuaToString,ValidOk>(
                                    LUA_TSTRING
                                )) {
                                    addListListFromStackToDict<string, LuaToString>(
                                        dict,
                                        name
                                    );
                                } else if(isListListOnStack<scalar,LuaToScalar,ValidOk>()) {
                                    addListListFromStackToDict<scalar, LuaToScalar>(
                                        dict,
                                        name
                                    );
                                }
                                break;
                        }
                    } else {
                        Dbug << name << " is a dictionary" << endl;

                        dictionary subDict;
                        subDict.name()=name;
                        extractTopToDict(subDict);
                        dict.set(name,subDict);
                    }
                }
                break;
            case LUA_TUSERDATA:
                if(isLuaFieldWrap<scalar>(luaState_, -1)) {
                    dict.set(
                        name,
                        getLuaField<scalar>(luaState_, -1)
                    );
                } else if(isLuaFieldWrap<vector>(luaState_, -1)) {
                    dict.set(
                        name,
                        getLuaField<vector>(luaState_, -1)
                    );
                } else if(isLuaFieldWrap<tensor>(luaState_, -1)) {
                    dict.set(
                        name,
                        getLuaField<tensor>(luaState_, -1)
                    );
                } else if(isLuaFieldWrap<symmTensor>(luaState_, -1)) {
                    dict.set(
                        name,
                        getLuaField<symmTensor>(luaState_, -1)
                    );
                } else if(isLuaFieldWrap<sphericalTensor>(luaState_, -1)) {
                    dict.set(
                        name,
                        getLuaField<sphericalTensor>(luaState_, -1)
                    );
                } else {
                    WarningIn("luaInterpreterWrapper::extractTopToDict()")
                        << name << " is an unsupported Lua-user-type "
                            << endl;
                }
                break;
            default:
                WarningIn("luaInterpreterWrapper::extractTopToDict()")
                    << name << " is of unsupported Lua-type "
                        << lua_typename(luaState_,type)
                        << endl;
        }

        lua_pop(luaState_,1);
        Dbug << "Stack before next " << lua_gettop(luaState_) << endl;
    }
    Dbug << "luaInterpreterWrapper::extractTopToDict - finished "
        << lua_gettop(luaState_) << endl;
}

bool luaInterpreterWrapper::extractDictionary(
    const word &name,
    dictionary &dict
) {
    Dbug << "luaInterpreterWrapper::extractDictionary" << endl;

    int type=lua_getglobal(luaState_,name.c_str());
    if(type==LUA_TTABLE) {
        extractTopToDict(dict);
    } else if(type==LUA_TNIL) {
        WarningIn("luaInterpreterWrapper::extractDictionary()")
            << "No symbol " << name << " in Lua namespace"
                << endl;
    }

    Dbug << "Before popping. Stack size " << lua_gettop(luaState_) << endl;
    lua_pop(luaState_,1);
    Dbug << "finished extracting. Stack size " << lua_gettop(luaState_)
        << endl;

    return true;
}

bool luaInterpreterWrapper::luaVariableToResult(
    const word &name,
    ExpressionResult &eResult
) {
    int type=lua_getglobal(luaState_,name.c_str());
    Dbug << "Type is " << type << " -> "
        << lua_typename(luaState_,type) << endl;

    switch(type) {
        case LUA_TNIL:
            WarningIn("luaInterpreterWrapper::setGlobals()")
                << "No symbol " << name << " in Lua namespace"
                    << endl;
            lua_pop(luaState_,1);
            return false;
            break;
        case LUA_TNUMBER:
        case LUA_TBOOLEAN:
            eResult.setSingleValue(
                lua_tonumber(luaState_,-1)
            );
            lua_pop(luaState_,1);
            break;
        case LUA_TTABLE:
            if(isVectorSpace<vector>()) {
                eResult.setSingleValue(
                    stackToVectorSpace<vector>()
                );
            } else if(isVectorSpace<tensor>()) {
                eResult.setSingleValue(
                    stackToVectorSpace<tensor>()
                );
            } else if(isVectorSpace<symmTensor>()) {
                eResult.setSingleValue(
                    stackToVectorSpace<symmTensor>()
                );
            } else if(isVectorSpace<sphericalTensor>()) {
                eResult.setSingleValue(
                    stackToVectorSpace<sphericalTensor>()
                );
            } else {
                int type=lua_geti(
                    luaState_,
                    -1,
                    1
                );
                lua_pop(luaState_,1);
                if(
                    type==LUA_TNUMBER
                    ||
                    type==LUA_TBOOLEAN
                    ||
                    type==LUA_TTABLE
                ) {
                    label length=0;
                    bool goOn=true;
                    while(goOn) {
                        int ntype=lua_geti(
                            luaState_,
                            -1,
                            length+1
                        );
                        lua_pop(luaState_,1);
                        if(ntype!=LUA_TNIL) {
                            length+=1;
                        } else {
                            goOn=false;
                        }
                    }
                    Pbug << name << " is array of length " << length << endl;
                    if(
                        type==LUA_TNUMBER
                        ||
                        type==LUA_TBOOLEAN
                    ) {
                        scalarField f(length);
                        forAll(f,i) {
                            lua_geti(
                                luaState_,
                                -1,
                                i+1
                            );
                            f[i]=lua_tonumber(luaState_,-1);
                            lua_pop(luaState_,1);
                        }
                        eResult.setResult(f);
                    } else {
                        lua_geti(
                            luaState_,
                            -1,
                            1
                        );
                        if(isVectorSpace<vector>()) {
                            lua_pop(luaState_,1);
                            eResult.setResult(
                                stackToVectorSpaceArray<vector>(length).ptr()
                            );
                        } else if(isVectorSpace<tensor>()) {
                            lua_pop(luaState_,1);
                            eResult.setResult(
                                stackToVectorSpaceArray<tensor>(length).ptr()
                            );
                        } else if(isVectorSpace<symmTensor>()) {
                            lua_pop(luaState_,1);
                            eResult.setResult(
                                stackToVectorSpaceArray<symmTensor>(length).ptr()
                            );
                        } else if(isVectorSpace<sphericalTensor>()) {
                            lua_pop(luaState_,1);
                            eResult.setResult(
                                stackToVectorSpaceArray<sphericalTensor>(length).ptr()
                            );
                        } else {
                            lua_pop(luaState_,1);
                            WarningIn("luaInterpreterWrapper::setGlobals()")
                                << name << " is an unrecognized array-type"
                                    << endl;
                            lua_pop(luaState_,1);
                            return false;
                        }
                    }
                } else {
                    WarningIn("luaInterpreterWrapper::setGlobals()")
                        << name << " is an unrecognized table-type"
                            << endl;
                    lua_pop(luaState_,1);
                    return false;
                }
            }
            lua_pop(luaState_,1);
            break;
        case LUA_TUSERDATA:
            if(isLuaFieldWrap<scalar>(luaState_, -1)) {
                eResult.setResult(getLuaField<scalar>(luaState_, -1));
                lua_pop(luaState_,1);
            } else if(isLuaFieldWrap<vector>(luaState_, -1)) {
                eResult.setResult(getLuaField<vector>(luaState_, -1));
                lua_pop(luaState_,1);
            } else if(isLuaFieldWrap<tensor>(luaState_, -1)) {
                eResult.setResult(getLuaField<tensor>(luaState_, -1));
                lua_pop(luaState_,1);
            } else if(isLuaFieldWrap<symmTensor>(luaState_, -1)) {
                eResult.setResult(getLuaField<symmTensor>(luaState_, -1));
                lua_pop(luaState_,1);
            } else if(isLuaFieldWrap<sphericalTensor>(luaState_, -1)) {
                eResult.setResult(getLuaField<sphericalTensor>(luaState_, -1));
                lua_pop(luaState_,1);
            } else {
                WarningIn("luaInterpreterWrapper::setGlobals()")
                    << name << " is an unsupported Lua-user-type "
                        << endl;
                lua_pop(luaState_,1);
            }
            break;
        default:
            WarningIn("luaInterpreterWrapper::setGlobals()")
                << name << " is of unsupported Lua-type "
                    << lua_typename(luaState_,type)
                    << endl;
            lua_pop(luaState_,1);
            return false;
    }
    return true;
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

        Dbug << "Setting variable "<< name << endl;

        ExpressionResult eResult;

        if(luaVariableToResult(name,eResult)) {
            Dbug << "Adding " << name << " to "<< interpreterToSwakNamespace_ << endl;
            ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
                obr()
            ).addValue(
                name,
                interpreterToSwakNamespace_,
                eResult
            );
            res.noReset();
        }
    }
}

void luaInterpreterWrapper::setVariablesInGlobal(
    const word &namespaceName,
    const wordList &names
) {
    forAll(names,nameI) {
        const word &var=names[nameI];
        ExpressionResult val;
        if(luaVariableToResult(var, val)) {
            ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
                obr()
            ).addValue(
                var,
                namespaceName,
                val
            );
        }
    }
}

autoPtr<RawFoamDictionaryParserDriver> luaInterpreterWrapper::getParserInternal(
    RawFoamDictionaryParserDriver::ErrorMode mode
) {
    return autoPtr<RawFoamDictionaryParserDriver>(
        new LuaFoamDictionaryParserDriver(
            *this,
            mode
        )
    );
}

bool luaInterpreterWrapper::startDictionary() {
    Dbug << "Starting table " << endl;
    Dbug << "Stack in the beginning: " << lua_gettop(luaState_) << endl;

    lua_newtable(luaState_);

    return true;
}

bool luaInterpreterWrapper::wrapUpDictionary(const word &name) {
    Dbug << "Setting global " << name << endl;
    lua_setglobal(luaState_,name.c_str());

    Dbug << "Stack in the end: " << lua_gettop(luaState_) << endl;
    return true;
}

void luaInterpreterWrapper::setReference(const word &name,Field<scalar> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}

void luaInterpreterWrapper::setReference(const word &name,Field<vector> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}

void luaInterpreterWrapper::setReference(const word &name,Field<tensor> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}

void luaInterpreterWrapper::setReference(const word &name,Field<symmTensor> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}

void luaInterpreterWrapper::setReference(const word &name,Field<sphericalTensor> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}

#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
void luaInterpreterWrapper::setReference(const word &name,Field<diagTensor> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}

void luaInterpreterWrapper::setReference(const word &name,Field<symmTensor4thOrder> &value) {
    addFieldToLua(
        luaState_,
        name,
        &value
    );
}
#endif

} // namespace Foam

// ************************************************************************* //
