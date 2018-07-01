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

#include "pythonInterpreterWrapper.H"

#include "ExecuteIfInterpreterFunctionObject.H"
#include "SetDeltaTWithInterpreterFunctionObject.H"
#include "SetEndTimeWithInterpreterFunctionObject.H"
#include "StdoutFromScriptProvider.H"
#include "WriteAndEndInterpreterFunctionObject.H"
#include "WriteIfInterpreterFunctionObject.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    typedef ExecuteIfInterpreterFunctionObject<pythonInterpreterWrapper> executeIfPythonFunctionObject;

    defineTemplateTypeNameAndDebugWithName(executeIfPythonFunctionObject,"executeIfPython", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfPythonFunctionObject,
        dictionary
    );

    typedef TimeManipulationWithInterpreterFunctionObject<pythonInterpreterWrapper> timeManipulationWithPythonFunctionObject;

    defineTemplateTypeNameAndDebugWithName(timeManipulationWithPythonFunctionObject,"timeManipulationWithPython", 0);

    typedef SetDeltaTWithInterpreterFunctionObject<pythonInterpreterWrapper> setDeltaTWithPythonFunctionObject;

    defineTemplateTypeNameAndDebugWithName(setDeltaTWithPythonFunctionObject,"setDeltaTWithPython", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        setDeltaTWithPythonFunctionObject,
        dictionary
    );

    typedef SetEndTimeWithInterpreterFunctionObject<pythonInterpreterWrapper> setEndTimeWithPythonFunctionObject;

    defineTemplateTypeNameAndDebugWithName(setEndTimeWithPythonFunctionObject,"setEndTimeWithPython", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        setEndTimeWithPythonFunctionObject,
        dictionary
    );

    typedef StdoutFromScriptProvider<pythonInterpreterWrapper> stdoutFromPythonScriptProvider;
    defineTemplateTypeNameAndDebugWithName(stdoutFromPythonScriptProvider,"stdoutFromPythonScriptProvider",0);

    // to keep the macro happy
    typedef dynamicFunctionObjectListProxy::dynamicDictionaryProvider dynamicFunctionObjectListProxydynamicDictionaryProvider;

    addToRunTimeSelectionTable
    (
        dynamicFunctionObjectListProxydynamicDictionaryProvider,
        stdoutFromPythonScriptProvider,
        dictionary
    );

    typedef WriteAndEndInterpreterFunctionObject<pythonInterpreterWrapper> writeAndEndPythonFunctionObject;

    defineTemplateTypeNameAndDebugWithName(writeAndEndPythonFunctionObject,"writeAndEndPython", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        writeAndEndPythonFunctionObject,
        dictionary
    );

    typedef WriteIfInterpreterFunctionObject<pythonInterpreterWrapper> writeIfPythonFunctionObject;

    defineTemplateTypeNameAndDebugWithName(writeIfPythonFunctionObject,"writeIfPython", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        writeIfPythonFunctionObject,
        dictionary
    );
} // namespace Foam

// ************************************************************************* //
