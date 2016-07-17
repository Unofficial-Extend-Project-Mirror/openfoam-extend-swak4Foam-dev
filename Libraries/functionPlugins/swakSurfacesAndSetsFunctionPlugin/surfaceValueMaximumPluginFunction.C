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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "surfaceValueMaximumPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

typedef surfaceValueMaximumPluginFunction<scalar> surfaceMaximumScalar;
defineTemplateTypeNameAndDebug(surfaceMaximumScalar,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceMaximumScalar , name, surfaceValueMaximumScalar);

typedef surfaceValueMaximumPluginFunction<vector> surfaceMaximumVector;
defineTemplateTypeNameAndDebug(surfaceMaximumVector,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceMaximumVector , name, surfaceValueMaximumVector);

typedef surfaceValueMaximumPluginFunction<tensor> surfaceMaximumTensor;
defineTemplateTypeNameAndDebug(surfaceMaximumTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceMaximumTensor , name, surfaceValueMaximumTensor);

typedef surfaceValueMaximumPluginFunction<symmTensor> surfaceMaximumSymmTensor;
defineTemplateTypeNameAndDebug(surfaceMaximumSymmTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceMaximumSymmTensor , name, surfaceValueMaximumSymmTensor);

typedef surfaceValueMaximumPluginFunction<sphericalTensor> surfaceMaximumSphericalTensor;
defineTemplateTypeNameAndDebug(surfaceMaximumSphericalTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, surfaceMaximumSphericalTensor , name, surfaceValueMaximumSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
surfaceValueMaximumPluginFunction<Type>::surfaceValueMaximumPluginFunction(
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
void surfaceValueMaximumPluginFunction<Type>::doEvaluation()
{
    typedef typename GeneralSurfaceEvaluationPluginFunction<Type>::resultType rType;
    autoPtr<rType> pValueMaximum(
        new rType(
            IOobject(
                "surfaceValueMaximumInCell",
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
    List<bool> here(pValueMaximum->size(),false);
    const Field<Type> vals=this->values();

    forAll(cells,i) {
        const label cellI=cells[i];

        if(cellI>=0) {
            if(here[cellI]) {
                pValueMaximum()[cellI]=max(
                    vals[i],
                    pValueMaximum()[cellI]
                );
            } else {
                here[cellI]=true;
                pValueMaximum()[cellI]=vals[i];
            }
        }
    }

    pValueMaximum->correctBoundaryConditions();

    this->result().setObjectResult(pValueMaximum);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
