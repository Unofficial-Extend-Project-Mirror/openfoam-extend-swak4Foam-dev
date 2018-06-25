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

#include "addToRunTimeSelectionTable.H"

#include "luaInterpreterWrapper.H"

#include "ExecuteIfInterpreterFunctionObject.H"
#include "SetDeltaTWithInterpreterFunctionObject.H"
#include "SetEndTimeWithInterpreterFunctionObject.H"
#include "StdoutFromScriptProvider.H"
#include "WriteAndEndInterpreterFunctionObject.H"
#include "WriteIfInterpreterFunctionObject.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    typedef ExecuteIfInterpreterFunctionObject<luaInterpreterWrapper> executeIfLuaFunctionObject;

    defineTemplateTypeNameAndDebugWithName(executeIfLuaFunctionObject,"executeIfLua", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfLuaFunctionObject,
        dictionary
    );

    typedef TimeManipulationWithInterpreterFunctionObject<luaInterpreterWrapper> timeManipulationWithLuaFunctionObject;

    defineTemplateTypeNameAndDebugWithName(timeManipulationWithLuaFunctionObject,"timeManipulationWithLua", 0);

    typedef SetDeltaTWithInterpreterFunctionObject<luaInterpreterWrapper> setDeltaTWithLuaFunctionObject;

    defineTemplateTypeNameAndDebugWithName(setDeltaTWithLuaFunctionObject,"setDeltaTWithLua", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        setDeltaTWithLuaFunctionObject,
        dictionary
    );

    typedef SetEndTimeWithInterpreterFunctionObject<luaInterpreterWrapper> setEndTimeWithLuaFunctionObject;

    defineTemplateTypeNameAndDebugWithName(setEndTimeWithLuaFunctionObject,"setEndTimeWithLua", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        setEndTimeWithLuaFunctionObject,
        dictionary
    );

    typedef StdoutFromScriptProvider<luaInterpreterWrapper> stdoutFromLuaScriptProvider;
    defineTemplateTypeNameAndDebugWithName(stdoutFromLuaScriptProvider,"stdoutFromLuaScriptProvider",0);

    // to keep the macro happy
    typedef dynamicFunctionObjectListProxy::dynamicDictionaryProvider dynamicFunctionObjectListProxydynamicDictionaryProvider;

    addToRunTimeSelectionTable
    (
        dynamicFunctionObjectListProxydynamicDictionaryProvider,
        stdoutFromLuaScriptProvider,
        dictionary
    );

    typedef WriteAndEndInterpreterFunctionObject<luaInterpreterWrapper> writeAndEndLuaFunctionObject;

    defineTemplateTypeNameAndDebugWithName(writeAndEndLuaFunctionObject,"writeAndEndLua", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        writeAndEndLuaFunctionObject,
        dictionary
    );

    typedef WriteIfInterpreterFunctionObject<luaInterpreterWrapper> writeIfLuaFunctionObject;

    defineTemplateTypeNameAndDebugWithName(writeIfLuaFunctionObject,"writeIfLua", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        writeIfLuaFunctionObject,
        dictionary
    );
} // namespace Foam

// ************************************************************************* //
