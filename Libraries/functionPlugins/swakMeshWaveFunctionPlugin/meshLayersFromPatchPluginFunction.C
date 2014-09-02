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
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "meshLayersFromPatchPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "MeshLayersDistFromPatch.H"
#include "FaceCellWave.H"

#include "emptyFvPatchFields.H"

namespace Foam {

defineTypeNameAndDebug(meshLayersFromPatchPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, meshLayersFromPatchPluginFunction , name, meshLayersFromPatch);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

meshLayersFromPatchPluginFunction::meshLayersFromPatchPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        string("patchName primitive word")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void meshLayersFromPatchPluginFunction::setArgument(
    label index,
    const word &patchName
) {
    assert(index==0);

    patchName_=patchName;
}

void meshLayersFromPatchPluginFunction::doEvaluation()
{
    label patchI=mesh().boundaryMesh().findPatchID(patchName_);
    if(patchI<0) {
        FatalErrorIn("meshLayersFromPatchPluginFunction::doEvaluation()")
            << "Patch name " << patchName_ << " not in valid names"
                << mesh().boundaryMesh().names()
                << endl
                << exit(FatalError);
    }

    List<MeshLayersDistFromPatch> cellValues(mesh().C().size());
    List<MeshLayersDistFromPatch> faceValues(mesh().nFaces());
    labelList startFaces(mesh().boundaryMesh()[patchI].size());
    for(label i=0;i<mesh().boundaryMesh()[patchI].size();i++) {
        startFaces[i]=mesh().boundaryMesh()[patchI].start()+i;
    }
    List<MeshLayersDistFromPatch> startValues(
        mesh().boundaryMesh()[patchI].size(),
        MeshLayersDistFromPatch(1)
    );

    FaceCellWave<MeshLayersDistFromPatch> distToPatch(
        mesh(),
        startFaces,
        startValues,
        faceValues,
        cellValues,
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

    forAll(cellValues,cellI) {
        layers.internalField()[cellI]=cellValues[cellI].dist()/2;
    }
    forAll(layers.boundaryField(), patchI)
    {
        if (!isA<emptyFvPatchScalarField>(layers.boundaryField()[patchI]))
        {
            for(label i=0;i<layers.boundaryField()[patchI].size();i++) {
                label faceI=mesh().boundaryMesh()[patchI].start()+i;

                layers.boundaryField()[patchI][i]=faceValues[faceI].dist()/2;
            }
        }
    }

    layers.correctBoundaryConditions();

    result().setObjectResult(pLayers);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
