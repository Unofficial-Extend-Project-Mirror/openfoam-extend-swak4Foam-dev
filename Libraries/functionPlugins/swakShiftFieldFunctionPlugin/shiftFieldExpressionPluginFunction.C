/*---------------------------------------------------------------------------*\
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
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"
#include "shiftFieldExpressionPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "meshToMesh.H"
#include "MeshInterpolationOrder.H"

namespace Foam {

defineTemplateTypeNameAndDebug(shiftFieldExpressionPluginFunction<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionPluginFunction,scalar , name, shiftExpressionScalarField);
defineTemplateTypeNameAndDebug(shiftFieldExpressionPluginFunction<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionPluginFunction,vector , name, shiftExpressionVectorField);
defineTemplateTypeNameAndDebug(shiftFieldExpressionPluginFunction<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionPluginFunction,tensor , name, shiftExpressionTensorField);
defineTemplateTypeNameAndDebug(shiftFieldExpressionPluginFunction<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionPluginFunction,symmTensor , name, shiftExpressionSymmTensorField);
defineTemplateTypeNameAndDebug(shiftFieldExpressionPluginFunction<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionPluginFunction,sphericalTensor , name, shiftExpressionSphericalTensorField);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
shiftFieldExpressionPluginFunction<Type>::shiftFieldExpressionPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type>(
        parentDriver,
        name,
        "shiftValue internalField pointVectorField"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class Type>
tmp<pointField> shiftFieldExpressionPluginFunction<Type>::displacement()
{
    return tmp<pointField>(
        new pointField(
            shiftValue_
        )
    );
}

template<class Type>
void shiftFieldExpressionPluginFunction<Type>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0 || index==1);

    if(index==1) {
        shiftValue_.set(
            new pointVectorField(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<pointVectorField>()
            )
        );
    } else {
        shiftFieldGeneralPluginFunction<Type>::setArgument(
            index,
            content,
            driver
        );
    }
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
