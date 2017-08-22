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

Class
    luaInterpreterWrapper

Description
    Makes sure that the object receives a lua-Interpreter on its own

SourceFiles
    luaInterpreterWrapper.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef luaInterpreterWrapper_H
#define luaInterpreterWrapper_H

#include "dictionary.H"
#include "objectRegistry.H"
#include "swakTime.H"

extern "C" {
        # include "lua.h"
        # include "lauxlib.h"
        # include "lualib.h"
}

#include "generalInterpreterWrapperCRTP.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

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

    void initIPython();

    void initInteractiveSpecial();

    void printError();

protected:

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

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
