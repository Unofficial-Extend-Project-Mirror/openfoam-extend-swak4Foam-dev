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
    2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "meshLayersGeneralPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "FaceCellWave.H"

#include "emptyFvPatchFields.H"

namespace Foam {

defineTypeNameAndDebug(meshLayersGeneralPluginFunction,1);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

meshLayersGeneralPluginFunction::meshLayersGeneralPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const string &description
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        description
    ),
    cellValues_(mesh().C().size()),
    faceValues_(mesh().nFaces())
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void meshLayersGeneralPluginFunction::doEvaluation()
{
    this->initFacesAndCells();

    FaceCellWave<MeshLayersDistFromPatch> distToPatch(
        mesh(),
        startFaces_,
        startValues_,
        faceValues_,
        cellValues_,
        mesh().C().size()
    );

    autoPtr<volScalarField> pLayers(
        new volScalarField(
            IOobject(
                "layers",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("nLayers",dimless,0),
            "fixedValue"
        )
    );
    volScalarField &layers=pLayers();

    forAll(cellValues_,cellI) {
        const_cast<scalar&>(layers.internalField()[cellI])=
            cellValues_[cellI].dist()/2;
    }
    forAll(layers.boundaryField(), patchI)
    {
        if (!isA<emptyFvPatchScalarField>(layers.boundaryField()[patchI]))
        {
            for(label i=0;i<layers.boundaryField()[patchI].size();i++) {
                label faceI=mesh().boundaryMesh()[patchI].start()+i;

                const_cast<scalar&>(layers.boundaryField()[patchI][i])=
                    faceValues_[faceI].dist()/2;
            }
        }
    }

    layers.correctBoundaryConditions();

    result().setObjectResult(pLayers);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
