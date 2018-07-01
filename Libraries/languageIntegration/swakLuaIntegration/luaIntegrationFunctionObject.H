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
    luaIntegrationFunctionObject

Description
    Integrates a lua-interpreter that allows the execution of Lua-scripts

SourceFiles
    luaIntegrationFunctionObject.C

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef luaIntegrationFunctionObject_H
#define luaIntegrationFunctionObject_H

#include "simpleFunctionObject.H"
#include "dictionary.H"
#include "OFstream.H"
#include "HashPtrTable.H"
#include "boolList.H"
#include "Field.H"

#include "luaInterpreterWrapper.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class luaIntegrationFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class luaIntegrationFunctionObject
:
    public simpleFunctionObject,
    public luaInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    luaIntegrationFunctionObject(const luaIntegrationFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const luaIntegrationFunctionObject&);

    // Private data

    //- Code to be executed at the start
    string startCode_;

    //- Code to be executed at the end
    string endCode_;

    //- Code to be executed at execute
    string executeCode_;

    //- Code to be executed when data is written to disc
    string writeCode_;

    //- reference to the time
    const Time& time_;

    //- set the lua variable with the time to the current runTime
    void setRunTime();

public:

    //- Runtime type information
    TypeName("luaIntegration");


    // Constructors

    //- Construct from components
    luaIntegrationFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    virtual ~luaIntegrationFunctionObject();

    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual void writeSimple();

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

    //- Execute in the end
    virtual bool end();

#ifdef FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH
    virtual void updateMesh(const mapPolyMesh&) {
    }

    virtual void movePoints(
#ifdef FOAM_MOVEPOINTS_GETS_POLYMESH
        const polyMesh&
#else
        const pointField&
#endif
    ) {
    }
#endif
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
