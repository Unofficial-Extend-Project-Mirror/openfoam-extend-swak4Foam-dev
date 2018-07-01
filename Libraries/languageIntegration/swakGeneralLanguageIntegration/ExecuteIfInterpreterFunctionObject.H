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
    ExecuteIfInterpreterFunctionObject

Description
    This proxy-functionObject only executs its childrens if a interpreter-code
    evaluates to true

SourceFiles
    ExecuteIfInterpreterFunctionObject.C

Contributors/Copyright:
    2011-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef ExecuteIfInterpreterFunctionObject_H
#define ExecuteIfInterpreterFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class ExecuteIfInterpreterFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template<class Wrapper>
class ExecuteIfInterpreterFunctionObject
:
    public conditionalFunctionObjectListProxy,
    public Wrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    ExecuteIfInterpreterFunctionObject(const ExecuteIfInterpreterFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const ExecuteIfInterpreterFunctionObject&);

    //- the code that should be evaluated when the function object starts
    string initCode_;

    //- the code that should be evaluated
    string conditionCode_;

    void readParameters(const dictionary &);
protected:

    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("ExecuteIfInterpreter");


    // Constructors

    //- Construct from components
    ExecuteIfInterpreterFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary& dict);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#include "ExecuteIfInterpreterFunctionObject.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
