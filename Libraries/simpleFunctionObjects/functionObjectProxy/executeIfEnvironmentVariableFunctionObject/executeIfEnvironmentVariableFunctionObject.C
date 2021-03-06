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
    2011, 2013, 2015-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id: executeIfEnvironmentVariableFunctionObject.C,v ff2f69b41452 2018-12-23 14:35:04Z bgschaid $
\*---------------------------------------------------------------------------*/

#include "executeIfEnvironmentVariableFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(executeIfEnvironmentVariableFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfEnvironmentVariableFunctionObject,
        dictionary
    );

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    const Enum<Foam::executeIfEnvironmentVariableFunctionObject::fitVariableMode>
    executeIfEnvironmentVariableFunctionObject::fitVariableModeNames_
    ({
        {fitVariableMode::fvmExists,"exists"},
        {fitVariableMode::fvmDoesNotExist,"doesNotExist"},
        {fitVariableMode::fvmFitsRegexp,"fitsRegexp"}
    });
#else
    template<>
    const char* NamedEnum<Foam::executeIfEnvironmentVariableFunctionObject::fitVariableMode,3>::names[]=
    {
        "exists",
        "doesNotExist",
        "fitsRegexp"
    };
    const NamedEnum<executeIfEnvironmentVariableFunctionObject::fitVariableMode,3> executeIfEnvironmentVariableFunctionObject::fitVariableModeNames_;
#endif


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfEnvironmentVariableFunctionObject::executeIfEnvironmentVariableFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    conditionalFunctionObjectListProxy(
        name,
        t,
        dict
    )
{
    // do it here to avoid the superclass-read being read twice
    readData(dict);

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    start();
#endif
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool executeIfEnvironmentVariableFunctionObject::condition()
{
    bool exists=env(variableName_);
    if(writeDebug()) {
        Info << "Variable " << variableName_ << " "
            << (exists ? "exists" : "does not exist")
            << endl;
    }

    string content;

    switch(fitVariableMode_) {
        case fvmExists:
            return exists;
        case fvmDoesNotExist:
            return !exists;
        case fvmFitsRegexp:
            if(exists) {
                content=getEnv(variableName_);
                if(writeDebug()) {
                    Info << "Content of " << variableName_ << ": "
                        << content
                        << (contentRegexp_.match(content) ? " fits " : " does not fit ")
                        << endl;
                }
                return contentRegexp_.match(content);
            }
            break;
        default:
            FatalErrorIn("executeIfEnvironmentVariableFunctionObject::condition()")
                << "The fitting mode " << fitVariableModeNames_[fitVariableMode_]
                    << " is not implemented" << endl
                    << exit(FatalError);
    }
    return false;
}

void executeIfEnvironmentVariableFunctionObject::readData(const dictionary& dict)
{
    fitVariableMode_=fitVariableModeNames_[
        word(dict.lookup("fitVariableMode"))
    ];
    variableName_=word(dict.lookup("variableName"));

    if(fitVariableMode_==fvmFitsRegexp) {
        contentRegexp_.set(
            string(dict.lookup("contentRegexp")),
            dict.lookupOrDefault<bool>("ignoreCase",false)
        );
    }
}

bool executeIfEnvironmentVariableFunctionObject::read(const dictionary& dict)
{
    readData(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

} // namespace Foam

// ************************************************************************* //
