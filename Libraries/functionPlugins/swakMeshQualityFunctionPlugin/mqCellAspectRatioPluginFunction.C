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

#include "mqCellAspectRatioPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(mqCellAspectRatioPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, mqCellAspectRatioPluginFunction , name, mqCellAspectRatio);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

mqCellAspectRatioPluginFunction::mqCellAspectRatioPluginFunction(
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

void mqCellAspectRatioPluginFunction::doEvaluation()
{
    autoPtr<volScalarField> pAspectRatio(
        new volScalarField(
            IOobject(
                "cellAspectRatio",
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

    volScalarField &aspectRatio=pAspectRatio();

    Vector<label> meshD=mesh().geometricD();

    vectorField sumMagClosed(mesh().nCells(), vector::zero);
    const labelList& own = mesh().faceOwner();
    const labelList& nei = mesh().faceNeighbour();
    const vectorField& areas = mesh().faceAreas();

    forAll (own, faceI)
    {
        sumMagClosed[own[faceI]] += cmptMag(areas[faceI]);
    }

    forAll (nei, faceI)
    {
        sumMagClosed[nei[faceI]] += cmptMag(areas[faceI]);
    }

    const scalarField& vols = mesh().cellVolumes();

    label nDims = 0;
    for (direction dir = 0; dir < vector::nComponents; dir++)
    {
        if (meshD[dir] == 1)
        {
            nDims++;
        }
    }

    // Check the sums
    forAll(aspectRatio, cellI)
    {
        // Calculate the aspect ration as the maximum of Cartesian component
        // aspect ratio to the total area hydraulic area aspect ratio
        scalar minCmpt = VGREAT;
        scalar maxCmpt = -VGREAT;
        for (direction dir = 0; dir < vector::nComponents; dir++)
        {
            if (meshD[dir] == 1)
            {
                minCmpt = min(minCmpt, sumMagClosed[cellI][dir]);
                maxCmpt = max(maxCmpt, sumMagClosed[cellI][dir]);
            }
        }

        aspectRatio[cellI] = maxCmpt/(minCmpt + VSMALL);
        if (nDims == 3)
        {
            aspectRatio[cellI] = max
            (
                aspectRatio[cellI],
                1.0/6.0*cmptSum(sumMagClosed[cellI])/pow(vols[cellI], 2.0/3.0)
            );
        }
    }

    result().setObjectResult(pAspectRatio);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
