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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "StoredStackExpressionResult.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(StoredStackExpressionResult,0);

addToRunTimeSelectionTable(ExpressionResult, StoredStackExpressionResult, dictionary);
addToRunTimeSelectionTable(ExpressionResult, StoredStackExpressionResult, nothing);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

StoredStackExpressionResult::StoredStackExpressionResult()
:
    StackExpressionResult()
{
    // reset the setting of the parent-class
    setNeedsReset(false);
}

StoredStackExpressionResult::StoredStackExpressionResult(
    const StoredStackExpressionResult &rhs
)
:
    StackExpressionResult(rhs)
{
    // reset the setting of the parent-class
    setNeedsReset(false);
}

StoredStackExpressionResult::StoredStackExpressionResult(const dictionary &dict)
:
    StackExpressionResult(dict)
{
    Dbug << "StoredStackExpressionResult(const dictionary &dict)" << endl;
    Dbug << "Value: " << (*this) << endl;

    // reset the setting of the parent-class
    setNeedsReset(false);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

StoredStackExpressionResult::~StoredStackExpressionResult()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void StoredStackExpressionResult::resetInternal()
{
    // do nothing
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void StoredStackExpressionResult::operator=(const StoredStackExpressionResult& rhs)
{
    Dbug << "operator=(const StoredStackExpressionResult& rhs)" << endl;

    // Check for assignment to self
    if (this == &rhs)
    {
        FatalErrorIn("StoredStackExpressionResult::operator=(const StoredStackExpressionResult&)")
            << "Attempted assignment to self"
            << exit(FatalError);
    }

    static_cast<StackExpressionResult&>(*this)=rhs;
}

// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //


} // namespace

// ************************************************************************* //
