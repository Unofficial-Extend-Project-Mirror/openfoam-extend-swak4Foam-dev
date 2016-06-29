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
    2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "cellColouringPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "FaceCellWave.H"

#include "emptyFvPatchFields.H"
#include "coupledFvPatchFields.H"

namespace Foam {

defineTypeNameAndDebug(cellColouringPluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, cellColouringPluginFunction, name, cellColouring);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

cellColouringPluginFunction::cellColouringPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        ""
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void cellColouringPluginFunction::doEvaluation()
{
    // this data has to be initialized by initFacesAndCells
    List<CellColouringData> cellValues(mesh().C().size());
    forAll(cellValues,cellI) {
        cellValues[cellI]=CellColouringData(
            mesh().cells()[cellI]
        );
    }
    List<CellColouringData> faceValues(mesh().nFaces());;

    label cnt=0;

    while(true) {
        label startCell=-1;

        forAll(cellValues,cellI) {
            if(
                startCell<0
                &&
                cellValues[cellI].colour()<0
            ) {
                startCell=cellI;
            }
        }

        Pbug << "Found cell: " << startCell << endl;

        label cpuToDoIt=pTraits<label>::max;
        if(startCell>=0) {
            cpuToDoIt=Pstream::myProcNo();
        }
        reduce(cpuToDoIt,minOp<label>());
        if(cpuToDoIt==pTraits<label>::max) {
            break;
        }
        cnt++;
        labelList startFaces;
        List<CellColouringData> startValues;
        if(cpuToDoIt==Pstream::myProcNo()) {
            const cell &c=mesh().cells()[startCell];
            const label newCol=cellValues[startCell].calcColour();
            const label startFaceI=cellValues[startCell].nextFace(c);

            Pbug << "Start: " << startCell << " " << cellValues[startCell]
                << " / " << c
                << " -> " << newCol << " " << startFaceI << endl;

            cellValues[startCell].setColour(newCol);
            startFaces=c;
            startValues=List<CellColouringData>(
                c.size()
            );
            forAll(startFaces,i) {
                startValues[i]=faceValues[startFaces[i]];
                startValues[i].setColour(
                    newCol,
                    startFaceI==startFaces[i]
                );
            }
        } else {
            startFaces.resize(0);
            startValues.resize(0);
        }

        FaceCellWave<CellColouringData> distToPatch(
            mesh(),
            startFaces,
            startValues,
            faceValues,
            cellValues,
            mesh().C().size()
        );
    }
    Dbug << cnt << " restarts needed" << endl;
    autoPtr<volScalarField> pRegions(
        new volScalarField(
            IOobject(
                "colours",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("nRegions",dimless,0),
            "zeroGradient"
        )
    );
    volScalarField &regions=pRegions();

    forAll(cellValues,cellI) {
        regions.internalField()[cellI]=cellValues[cellI].colour();
    }

    regions.correctBoundaryConditions();

    // #define CHECK_FOR_CHECKERBOARD
#ifdef CHECK_FOR_CHECKERBOARD
    Info << "Starting check" << endl;

    const cellList &cells=mesh().cells();
    const labelList &own=mesh().faceOwner();
    const labelList &nei=mesh().faceNeighbour();

    forAll(cells,cellI) {
        const cell &c=cells[cellI];
        forAll(c,i) {
            label faceI=c[i];
            if(faceI<mesh().nInternalFaces()) {
                label otherCellI;
                if(own[faceI]==cellI) {
                    otherCellI=nei[faceI];
                } else {
                    otherCellI=own[faceI];
                }
                if(regions[cellI]==regions[otherCellI]) {
                    Pout << "Same value " << regions[cellI] << " for "
                        << cellI << " and " << otherCellI << endl;
                }
            } else {
                label patchI=mesh().boundaryMesh().whichPatch(faceI);
                if(
                    isA<coupledFvPatchField<scalar> >(
                        regions.boundaryField()[patchI]
                    )
                ) {
                    const coupledFvPatchField<scalar> &pf(
                        dynamicCast<coupledFvPatchField<scalar> >(
                            regions.boundaryField()[patchI]
                        )
                    );
                    if(
                        regions[cellI]
                        ==
                        pf.patchNeighbourField()()[faceI-pf.patch().patch().start()]
                    ) {
                        Pout << "Same value " << regions[cellI] << " for "
                            << cellI << " and " << faceI << " on "
                            << patchI << endl;
                    }
                }
            }
        }
    }
    Info << "Ending check" << endl;
#endif

    result().setObjectResult(pRegions);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
