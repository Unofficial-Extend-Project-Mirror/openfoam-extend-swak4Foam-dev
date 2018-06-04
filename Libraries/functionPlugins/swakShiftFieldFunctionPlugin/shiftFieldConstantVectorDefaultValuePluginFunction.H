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
    Foam::shiftFieldConstantVectorDefaultValuePluginFunction

Description

SourceFiles
    shiftFieldConstantVectorDefaultValuePluginFunction.C

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef shiftFieldConstantVectorDefaultValuePluginFunction_H
#define shiftFieldConstantVectorDefaultValuePluginFunction_H

#include "shiftFieldGeneralPluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
    Class shiftFieldConstantVectorDefaultValuePluginFunction Declaration
\*---------------------------------------------------------------------------*/

template<class Type,meshToMeshOrder Order>
class shiftFieldConstantVectorDefaultValuePluginFunction
:
    public shiftFieldGeneralPluginFunction<Type,Order>
{
    //- Disallow default bitwise assignment
    void operator=(const shiftFieldConstantVectorDefaultValuePluginFunction &);

    shiftFieldConstantVectorDefaultValuePluginFunction(const shiftFieldConstantVectorDefaultValuePluginFunction &);

    vector shiftValue_;

protected:

    tmp<pointField> displacement();

    void setArgument(
        label index,
        const Type &content
    );

    void setArgument(
        label index,
        const vector &content
    );

public:
    shiftFieldConstantVectorDefaultValuePluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~shiftFieldConstantVectorDefaultValuePluginFunction() {}

    TypeName("shiftFieldConstantVectorDefaultValuePluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
