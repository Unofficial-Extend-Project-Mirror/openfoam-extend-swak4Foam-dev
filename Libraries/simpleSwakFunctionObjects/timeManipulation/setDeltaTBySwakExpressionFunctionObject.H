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
    setDeltaTBySwakExpressionFunctionObject

Description
    Set the time-step according to a swakExpression

SourceFiles
    setDeltaTBySwakExpressionFunctionObject.C

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef setDeltaTBySwakExpressionFunctionObject_H
#define setDeltaTBySwakExpressionFunctionObject_H

#include "timeManipulationFunctionObject.H"

#include "swakDataEntry.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
    using namespace Function1Types;
#endif

/*---------------------------------------------------------------------------*\
                           Class setDeltaTBySwakExpressionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class setDeltaTBySwakExpressionFunctionObject
:
    public timeManipulationFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    setDeltaTBySwakExpressionFunctionObject(const setDeltaTBySwakExpressionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const setDeltaTBySwakExpressionFunctionObject&);

    swakDataEntry<scalar> deltaTExpression_;

public:

    //- Runtime type information
    TypeName("setDeltaTBySwakExpression");


    // Constructors

    //- Construct from components
    setDeltaTBySwakExpressionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    virtual scalar deltaT();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
