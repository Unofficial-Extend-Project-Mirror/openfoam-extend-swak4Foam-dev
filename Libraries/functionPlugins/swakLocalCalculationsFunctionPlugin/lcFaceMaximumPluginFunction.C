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

#include "lcFaceMaximumPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(lcFaceMaximumPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, lcFaceMaximumPluginFunction , name, lcFaceMaximum);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcFaceMaximumPluginFunction::lcFaceMaximumPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    localCellCalculationFunction(
        parentDriver,
        name,
        string("faceField internalField surfaceScalarField")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void lcFaceMaximumPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0);

    original_.set(
        new surfaceScalarField(
            dynamicCast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<surfaceScalarField>()
        )
    );
}

void lcFaceMaximumPluginFunction::doCellCalculation(volScalarField &field)
{
    const cellList &cl=field.mesh().cells();
    const surfaceScalarField &o=original_();

    forAll(field,cellI) {
        scalar maxVal=-1e30;

        const cell &c=cl[cellI];
        forAll(c,i) {
            const label faceI=c[i];
            if(faceI<field.mesh().nInternalFaces()) {
                maxVal=max(maxVal,o[faceI]);
            } else {
                label patchID=field.mesh().boundaryMesh().whichPatch(faceI);
                label startI=field.mesh().boundaryMesh()[patchID].start();
                maxVal=max(
                    maxVal,
                    o.boundaryField()[patchID][faceI-startI]
                );
            }
        }

        field[cellI]=maxVal;
    }
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
