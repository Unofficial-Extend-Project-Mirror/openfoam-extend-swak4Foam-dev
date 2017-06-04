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
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "surfaceValueAveragePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

typedef surfaceValueAveragePluginFunction<scalar> surfaceAverageScalar;
defineTemplateTypeNameAndDebug(surfaceAverageScalar,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceAverageScalar , name, surfaceValueAverageScalar);

typedef surfaceValueAveragePluginFunction<vector> surfaceAverageVector;
defineTemplateTypeNameAndDebug(surfaceAverageVector,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceAverageVector , name, surfaceValueAverageVector);

typedef surfaceValueAveragePluginFunction<tensor> surfaceAverageTensor;
defineTemplateTypeNameAndDebug(surfaceAverageTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceAverageTensor , name, surfaceValueAverageTensor);

typedef surfaceValueAveragePluginFunction<symmTensor> surfaceAverageSymmTensor;
defineTemplateTypeNameAndDebug(surfaceAverageSymmTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceAverageSymmTensor , name, surfaceValueAverageSymmTensor);

typedef surfaceValueAveragePluginFunction<sphericalTensor> surfaceAverageSphericalTensor;
defineTemplateTypeNameAndDebug(surfaceAverageSphericalTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceAverageSphericalTensor , name, surfaceValueAverageSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
surfaceValueAveragePluginFunction<Type>::surfaceValueAveragePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    GeneralSurfaceEvaluationPluginFunction<Type>(
        parentDriver,
        name
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void surfaceValueAveragePluginFunction<Type>::doEvaluation()
{
    typedef typename GeneralSurfaceEvaluationPluginFunction<Type>::resultType rType;
    autoPtr<rType> pValueAverage(
        new rType(
            IOobject(
                "surfaceValueAverageInCell",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensioned<Type>("no",dimless,pTraits<Type>::zero)
        )
    );

    const labelList &cells=this->meshCells();
    const scalarField &area=this->theSurface().magSf();
    scalarField areaSum(pValueAverage->size(),0);
    const Field<Type> vals=this->values();

    forAll(cells,i) {
        const label cellI=cells[i];

        if(cellI>=0) {
            pValueAverage()[cellI]+=area[i]*vals[i];
            areaSum[cellI]+=area[i];
        }
    }

    forAll(areaSum,cellI) {
        if(areaSum[cellI]>SMALL) {
            const_cast<Type&>(pValueAverage->internalField()[cellI])
                /=areaSum[cellI];
        }
    }

    pValueAverage->correctBoundaryConditions();

    this->result().setObjectResult(pValueAverage);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
