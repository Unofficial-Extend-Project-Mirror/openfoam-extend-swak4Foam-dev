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
    executeIfWriteTimeFunctionObject

Description
    This function objects only executes its children only if the current time
    is a writeTime

SourceFiles
    executeIfWriteTimeFunctionObject.C

Contributors/Copyright:
    2011, 2013, 2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef executeIfWriteTimeFunctionObject_H
#define executeIfWriteTimeFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"
#include "regExp.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class executeIfWriteTimeFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class executeIfWriteTimeFunctionObject
:
    public conditionalFunctionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeIfWriteTimeFunctionObject(const executeIfWriteTimeFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const executeIfWriteTimeFunctionObject&);

protected:

    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("executeIfWriteTime");


    // Constructors

    //- Construct from components
    executeIfWriteTimeFunctionObject
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
