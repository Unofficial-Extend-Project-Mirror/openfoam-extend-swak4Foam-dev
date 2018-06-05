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

Class
    luaInterpreterWrapper

Description
    Makes sure that the object receives a lua-Interpreter on its own

SourceFiles
    luaInterpreterWrapper.C

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef luaInterpreterWrapper_H
#define luaInterpreterWrapper_H

#include "dictionary.H"
#include "objectRegistry.H"
#include "swakTime.H"

#include "Field.H"

extern "C" {
        # include "lua.h"
        # include "lauxlib.h"
        # include "lualib.h"
}

#if LUA_VERSION_NUM<503
#error "Lua-version too low. Must be at least 5.3"
#endif

#include "generalInterpreterWrapperCRTP.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class LuaFoamDictionaryParserDriver;

/*---------------------------------------------------------------------------*\
                           Class luaInterpreterWrapper Declaration
\*---------------------------------------------------------------------------*/


class luaInterpreterWrapper
:
    public generalInterpreterWrapperCRTP<luaInterpreterWrapper>
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    luaInterpreterWrapper(const luaInterpreterWrapper&);

    //- Disallow default bitwise assignment
    void operator=(const luaInterpreterWrapper&);

    // Private data

    //- state of 'my' Lua subinterpreter
    lua_State *luaState_;

    void getGlobals();

    void setGlobals();

    bool useLuaPrompt_;

    bool hasLuaPrompt_;

    bool parseToWrappedField_;

    bool wrapBoolLabelAsScalar_;

    void initLuaPrompt();

    void initInteractiveSpecial();

    void printError();

    template<class T>
    void vectorSpaceToStack(const T &val);

    template<class T>
    void vectorSpaceArrayToStack(const Field<T> &val);

    template<class T>
    T stackToVectorSpace();

    template<class T>
    autoPtr<Field<T> > stackToVectorSpaceArray(label length);

    template<class T>
    bool isVectorSpace();

    class ValidOk;

    template<class T,class F,class V>
    bool addListFromStackToDict(
        dictionary &dict,
        const word &name
    );

    template<class T,class F,class V>
    bool isListListOnStack(int lType=LUA_TNIL);

    template<class T,class F>
    void addListListFromStackToDict(
        dictionary &dict,
        const word &name
    );

protected:

    bool parseToWrappedField() const
        { return parseToWrappedField_;}

    bool wrapBoolLabelAsScalar() const
        { return wrapBoolLabelAsScalar_;}

    lua_State *luaState() {
        return luaState_;
    }

    //- Execute after the lua-code has been called
    void doAfterExecution(
        bool fail,
        const string &code,
        bool putVariables,
        bool failOnException=false
    );

    //- set the state to "my" interpreter
    void setInterpreter();

    //- release the state to "my" interpreter
    void releaseInterpreter();

    //- run an interactive sesssion
    void interactiveLoop(const string &banner);

    bool executeCodeInternal(
        const string &code
    );

    bool executeCodeCaptureOutputInternal(
        const string &code,
        string &stdout
    );

    autoPtr<RawFoamDictionaryParserDriver> getParserInternal(
        RawFoamDictionaryParserDriver::ErrorMode mode
    );

    bool startDictionary();

    bool wrapUpDictionary(const word &name);

    void extractTopToDict(dictionary &dict);

    void resultToLuaVariable(
        const word &var,
        const word &namespaceName,
        const ExpressionResult &value,
        bool noDict=false,
        bool forceNative=true
    );

    bool luaVariableToResult(
        const word &var,
        ExpressionResult &eResult
    );
public:

    //- Execute code and return a value
    template <typename T,class Func>
    T evaluateCodeInternal(
        const string &code,
        bool &success
    );

    class toBool {
    public:
        bool operator()(lua_State *L) {
            return bool(lua_toboolean(L, -1));
        }
    };

    class toScalar {
    public:
        scalar operator()(lua_State *L) {
            return scalar(lua_tonumber(L, -1));
        }
    };

    class toLabel {
    public:
        label operator()(lua_State *L) {
            return scalar(lua_tointeger(L, -1));
        }
    };

    //- set up the environment with the basic variables
    void initEnvironment(const Time &t);

    //- set the variable 'runTime' in the interpreter
    void setRunTime(const Time &time);

    bool extractDictionary(
        const word &name,
        dictionary &dict
    );

    void getVariablesFromDriver(
        CommonValueExpressionDriver &driver,
        const wordList &names
    );
    void setVariablesInGlobal(
        const word &globalNamespace,
        const wordList &names
    );

    void setReference(const word &name,Field<scalar> &value);
    void setReference(const word &name,Field<vector> &value);
    void setReference(const word &name,Field<tensor> &value);
    void setReference(const word &name,Field<symmTensor> &value);
    void setReference(const word &name,Field<sphericalTensor> &value);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    void setReference(const word &name,Field<diagTensor> &value);
    void setReference(const word &name,Field<symmTensor4thOrder> &value);
#endif

    //- Runtime type information
    TypeName("luaInterpreterWrapper");


    // Constructors

    //- Construct from components
    luaInterpreterWrapper
    (
        const objectRegistry& obr,
        const dictionary&,
        bool forceToNamespace=false
    );

    virtual ~luaInterpreterWrapper();

    void write(Ostream &os) const;

    friend LuaFoamDictionaryParserDriver;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
