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

#include "addToRunTimeSelectionTable.H"

#include "python3InterpreterWrapper.H"

#include "ExecuteIfInterpreterFunctionObject.H"
#include "SetDeltaTWithInterpreterFunctionObject.H"
#include "SetEndTimeWithInterpreterFunctionObject.H"
#include "StdoutFromScriptProvider.H"
#include "WriteAndEndInterpreterFunctionObject.H"
#include "WriteIfInterpreterFunctionObject.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    typedef ExecuteIfInterpreterFunctionObject<python3InterpreterWrapper> executeIfPython3FunctionObject;

    defineTemplateTypeNameAndDebugWithName(executeIfPython3FunctionObject,"executeIfPython3", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfPython3FunctionObject,
        dictionary
    );

    typedef TimeManipulationWithInterpreterFunctionObject<python3InterpreterWrapper> timeManipulationWithPython3FunctionObject;

    defineTemplateTypeNameAndDebugWithName(timeManipulationWithPython3FunctionObject,"timeManipulationWithPython3", 0);

    typedef SetDeltaTWithInterpreterFunctionObject<python3InterpreterWrapper> setDeltaTWithPython3FunctionObject;

    defineTemplateTypeNameAndDebugWithName(setDeltaTWithPython3FunctionObject,"setDeltaTWithPython3", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        setDeltaTWithPython3FunctionObject,
        dictionary
    );

    typedef SetEndTimeWithInterpreterFunctionObject<python3InterpreterWrapper> setEndTimeWithPython3FunctionObject;

    defineTemplateTypeNameAndDebugWithName(setEndTimeWithPython3FunctionObject,"setEndTimeWithPython3", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        setEndTimeWithPython3FunctionObject,
        dictionary
    );

    typedef StdoutFromScriptProvider<python3InterpreterWrapper> stdoutFromPython3ScriptProvider;
    defineTemplateTypeNameAndDebugWithName(stdoutFromPython3ScriptProvider,"stdoutFromPython3ScriptProvider",0);

    // to keep the macro happy
    typedef dynamicFunctionObjectListProxy::dynamicDictionaryProvider dynamicFunctionObjectListProxydynamicDictionaryProvider;

    addToRunTimeSelectionTable
    (
        dynamicFunctionObjectListProxydynamicDictionaryProvider,
        stdoutFromPython3ScriptProvider,
        dictionary
    );

    typedef WriteAndEndInterpreterFunctionObject<python3InterpreterWrapper> writeAndEndPython3FunctionObject;

    defineTemplateTypeNameAndDebugWithName(writeAndEndPython3FunctionObject,"writeAndEndPython3", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        writeAndEndPython3FunctionObject,
        dictionary
    );

    typedef WriteIfInterpreterFunctionObject<python3InterpreterWrapper> writeIfPython3FunctionObject;

    defineTemplateTypeNameAndDebugWithName(writeIfPython3FunctionObject,"writeIfPython3", 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        writeIfPython3FunctionObject,
        dictionary
    );
} // namespace Foam

// ************************************************************************* //
