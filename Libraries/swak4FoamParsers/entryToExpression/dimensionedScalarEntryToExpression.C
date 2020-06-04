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
    2014, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: dimensionedScalarEntryToExpression.C,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
\*---------------------------------------------------------------------------*/

#include "dimensionedScalarEntryToExpression.H"

#include "addToRunTimeSelectionTable.H"

#include "dimensionedScalar.H"

namespace Foam {

defineTypeNameAndDebug(dimensionedScalarEntryToExpression,0);

addNamedToRunTimeSelectionTable(entryToExpression,dimensionedScalarEntryToExpression, nothing,dimensionedScalar);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

dimensionedScalarEntryToExpression::dimensionedScalarEntryToExpression()
    :
    entryToExpression()
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

dimensionedScalarEntryToExpression::~dimensionedScalarEntryToExpression()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

string dimensionedScalarEntryToExpression::toExpr(const entry &e)
{
    // const primitiveEntry &pe=dynamicCast<const primitiveEntry&>(e);
    // dimensionedScalar val(pe);
    dimensionedScalar val(e.stream());
    OStringStream o;

    o << val.value();

    return o.str();
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
