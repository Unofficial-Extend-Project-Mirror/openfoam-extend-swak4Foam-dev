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
    writeAndEndSwakExpressionFunctionObject

Description
    Check whether a swak-condition is met. If yes everything is written to disk
    and the run ends gracefully

SourceFiles
    writeAndEndSwakExpressionFunctionObject.C

Contributors/Copyright:
    2012-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef writeAndEndSwakExpressionFunctionObject_H
#define writeAndEndSwakExpressionFunctionObject_H

#include "writeAndEndFunctionObject.H"

#include "CommonValueExpressionDriver.H"

#include "LogicalAccumulationNamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeAndEndSwakExpressionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class writeAndEndSwakExpressionFunctionObject
:
    public writeAndEndFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeAndEndSwakExpressionFunctionObject(const writeAndEndSwakExpressionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeAndEndSwakExpressionFunctionObject&);

    //- the expression driver
    autoPtr<CommonValueExpressionDriver> driver_;

    //- the logical expression
    exprString logicalExpression_;

    void readParameters(const dictionary &);

private:
    //- possible logical accumulations
    LogicalAccumulationNamedEnum::value logicalAccumulation_;

protected:

    //- do the actual checking
    virtual bool endRunNow();

public:

    //- Runtime type information
    TypeName("writeAndEndSwakExpression");


    // Constructors

    //- Construct from components
    writeAndEndSwakExpressionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary &dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
