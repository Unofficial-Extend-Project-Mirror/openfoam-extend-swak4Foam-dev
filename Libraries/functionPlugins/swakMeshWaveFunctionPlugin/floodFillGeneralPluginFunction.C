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

#include "floodFillGeneralPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "FaceCellWave.H"

#include "emptyFvPatchFields.H"

namespace Foam {

defineTypeNameAndDebug(floodFillGeneralPluginFunction,1);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

floodFillGeneralPluginFunction::floodFillGeneralPluginFunction(
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

void floodFillGeneralPluginFunction::doEvaluation()
{
    this->initFacesAndCells();

    label regionID=1;

    while(true) {
        label startCell=-1;

        forAll(cellValues_,cellI) {
            if(
                startCell<0
                &&
                cellValues_[cellI].val()<0
                &&
                !cellValues_[cellI].blocked()
            ) {
                startCell=cellI;
            }
            cellValues_[cellI].setTarget(regionID);
        }
        forAll(faceValues_,faceI) {
            faceValues_[faceI].setTarget(regionID);
        }

        label cpuToDoIt=pTraits<label>::max;
        if(startCell>=0) {
            cpuToDoIt=Pstream::myProcNo();
        }
        reduce(cpuToDoIt,minOp<label>());
        if(cpuToDoIt==pTraits<label>::max) {
            break;
        }
        if(cpuToDoIt==Pstream::myProcNo()) {
            const cell &c=mesh().cells()[startCell];
            cellValues_[startCell]=FloodFillData(
                regionID,
                regionID,
                true
            );
            startFaces_=c;
            startValues_=List<FloodFillData>(
                c.size(),
                FloodFillData(regionID,regionID)
            );
        } else {
            startFaces_.resize(0);
            startValues_.resize(0);
        }

        FaceCellWave<FloodFillData> distToPatch(
            mesh(),
            startFaces_,
            startValues_,
            faceValues_,
            cellValues_,
            mesh().C().size()
        );

        regionID++;
    }

    autoPtr<volScalarField> pRegions(
        new volScalarField(
            IOobject(
                "regions",
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

    forAll(cellValues_,cellI) {
        const_cast<scalar&>(regions.internalField()[cellI])=
            cellValues_[cellI].val();
    }

    regions.correctBoundaryConditions();

    result().setObjectResult(pRegions);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
