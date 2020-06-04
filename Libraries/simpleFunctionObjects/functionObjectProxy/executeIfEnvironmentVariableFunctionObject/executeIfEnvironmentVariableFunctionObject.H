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
    executeIfEnvironmentVariableFunctionObject

Description
    This proxy-functionObject only executs its childrens if an environment
    variable fits a certain condition:
    - exists
    - does not exist
    - fits a regular expression

SourceFiles
    executeIfEnvironmentVariableFunctionObject.C

Contributors/Copyright:
    2011, 2013, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: executeIfEnvironmentVariableFunctionObject.H,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef executeIfEnvironmentVariableFunctionObject_H
#define executeIfEnvironmentVariableFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"
#include "regExp.H"

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
#else
#include "NamedEnum.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class executeIfEnvironmentVariableFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class executeIfEnvironmentVariableFunctionObject
:
    public conditionalFunctionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeIfEnvironmentVariableFunctionObject(const executeIfEnvironmentVariableFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const executeIfEnvironmentVariableFunctionObject&);

    //- Name of the variable
    word variableName_;

    //- the regular expression against which the content of the environment variable should be matched
    regExp contentRegexp_;

    //- reread the data
    void readData(const dictionary& dict);

public:
    // some compilers need that
    enum fitVariableMode {
        fvmExists,
        fvmDoesNotExist,
        fvmFitsRegexp,
    };

private:

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    static const Enum<fitVariableMode> fitVariableModeNames_;
#else
    static const NamedEnum<fitVariableMode,3> fitVariableModeNames_;
#endif

    //- the mode with which the variable should be fitted
    fitVariableMode fitVariableMode_;
protected:

    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("executeIfEnvironmentVariable");


    // Constructors

    //- Construct from components
    executeIfEnvironmentVariableFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary& dict);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
