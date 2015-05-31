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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2012-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "mqCellShapePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "hexMatcher.H"
#include "wedgeMatcher.H"
#include "prismMatcher.H"
#include "pyrMatcher.H"
#include "tetWedgeMatcher.H"
#include "tetMatcher.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(mqCellShapePluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, mqCellShapePluginFunction , name, mqCellShape);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

mqCellShapePluginFunction::mqCellShapePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        string("")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void mqCellShapePluginFunction::doEvaluation()
{
    autoPtr<volScalarField> pShape(
        new volScalarField(
            IOobject(
                "cellShape",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("aRatio",dimless,0),
            "zeroGradient"
        )
    );

    volScalarField &shape=pShape();

    // Construct shape recognizers
    hexMatcher hex;
    prismMatcher prism;
    wedgeMatcher wedge;
    pyrMatcher pyr;
    tetWedgeMatcher tetWedge;
    tetMatcher tet;

    forAll(shape,cellI) {
        if(hex.isA(mesh(),cellI)) {
            shape[cellI]=1;
        } else if(prism.isA(mesh(),cellI)) {
            shape[cellI]=2;
        } else if(wedge.isA(mesh(),cellI)) {
            shape[cellI]=3;
        } else if(pyr.isA(mesh(),cellI)) {
            shape[cellI]=4;
        } else if(tetWedge.isA(mesh(),cellI)) {
            shape[cellI]=5;
        } else if(tet.isA(mesh(),cellI)) {
            shape[cellI]=6;
        } else {
            shape[cellI]=0;
        }
    }

    shape.correctBoundaryConditions();

    result().setObjectResult(pShape);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
