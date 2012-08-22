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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "setHasSetPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(setHasSetPluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, setHasSetPluginFunction , name, setHasSet);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

setHasSetPluginFunction::setHasSetPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    GeneralSetsPluginFunction(
        parentDriver,
        name,
        "volLogicalField",
        string("setName primitive word")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void setHasSetPluginFunction::doEvaluation()
{
    autoPtr<volScalarField> pHasSet(
        new volScalarField(
            IOobject(
                "setHasSetInCell",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("no",dimless,0)
        )
    );

    const labelList &cells=theSet().cells();

    forAll(cells,i) {
        const label cellI=cells[i];

        pHasSet()[cellI]=1;
    }

    pHasSet->correctBoundaryConditions();

    result().setObjectResult(pHasSet);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
