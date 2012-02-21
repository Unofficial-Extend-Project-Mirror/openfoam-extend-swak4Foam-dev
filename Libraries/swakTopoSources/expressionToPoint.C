/*---------------------------------------------------------------------------*\
 ##   ####  ######     | 
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "expressionToPoint.H"
#include "polyMesh.H"
#include "pointSet.H"

#include "addToRunTimeSelectionTable.H"

#include "FieldValueExpressionDriver.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(expressionToPoint, 0);

addToRunTimeSelectionTable(topoSetSource, expressionToPoint, word);

addToRunTimeSelectionTable(topoSetSource, expressionToPoint, istream);

}


Foam::topoSetSource::addToUsageTable Foam::expressionToPoint::usage_
(
    expressionToPoint::typeName,
    "\n    Usage: expressionToPoint <expression>\n\n"
    "    Select all points for which expression evaluates to true\n\n"
);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::expressionToPoint::combine(topoSet& set, const bool add) const
{
    fvMesh mesh(set.db());

    FieldValueExpressionDriver driver
        (
            mesh.time().timeName(),
            mesh.time(),
            mesh,
            true, // cache stuff
            true, // search in memory
            true  // search on disc
        );
    driver.parse(expression_);
    if(!driver.resultIsTyp<pointScalarField>(true)) {
        FatalErrorIn("Foam::expressionToPoint::combine(topoSet& set, const bool add) const")
            << "Expression " << expression_ << " does not evaluate to a logical expression"
                << endl
                << exit(FatalError);
    }
    const pointScalarField &condition=driver.getResult<pointScalarField>();

    forAll(condition, pointI)
    {
        if (condition[pointI])
        {
            addOrDelete(set, pointI, add);
        }
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from components
Foam::expressionToPoint::expressionToPoint
(
    const polyMesh& mesh,
    const string& expression
)
:
    topoSetSource(mesh),
    expression_(expression)
{}


// Construct from dictionary
Foam::expressionToPoint::expressionToPoint
(
    const polyMesh& mesh,
    const dictionary& dict
)
:
    topoSetSource(mesh),
    expression_(dict.lookup("expression"))
{}


// Construct from Istream
Foam::expressionToPoint::expressionToPoint
(
    const polyMesh& mesh,
    Istream& is
)
:
    topoSetSource(mesh),
    expression_(checkIs(is))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::expressionToPoint::~expressionToPoint()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::topoSetSource::sourceType Foam::expressionToPoint::setType() const
{
    return POINTSETSOURCE;
}

void Foam::expressionToPoint::applyToSet
(
    const topoSetSource::setAction action,
    topoSet& set
) const
{
    if ((action == topoSetSource::ADD) || (action == topoSetSource::NEW))
    {
        Info<< "    Adding all elements of for which " << expression_ << " evaluates to true ..."
            << endl;
        
        combine(set,true);
    }
    else if (action == topoSetSource::DELETE)
    {
        Info<< "    Removing all elements of for which " << expression_ << " evaluates to true ..."
            << endl;

        combine(set,false);
    }
}


// ************************************************************************* //
