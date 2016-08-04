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
#include "shiftFieldExpressionDefaultValuePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "meshToMesh.H"
#include "MeshInterpolationOrder.H"

namespace Foam {

defineTemplateTypeNameAndDebug(shiftFieldExpressionDefaultValuePluginFunction<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionDefaultValuePluginFunction,scalar , name, shiftExpressionScalarFieldDefault);
defineTemplateTypeNameAndDebug(shiftFieldExpressionDefaultValuePluginFunction<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionDefaultValuePluginFunction,vector , name, shiftExpressionVectorFieldDefault);
defineTemplateTypeNameAndDebug(shiftFieldExpressionDefaultValuePluginFunction<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionDefaultValuePluginFunction,tensor , name, shiftExpressionTensorFieldDefault);
defineTemplateTypeNameAndDebug(shiftFieldExpressionDefaultValuePluginFunction<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionDefaultValuePluginFunction,symmTensor , name, shiftExpressionSymmTensorFieldDefault);
defineTemplateTypeNameAndDebug(shiftFieldExpressionDefaultValuePluginFunction<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldExpressionDefaultValuePluginFunction,sphericalTensor , name, shiftExpressionSphericalTensorFieldDefault);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
shiftFieldExpressionDefaultValuePluginFunction<Type>::shiftFieldExpressionDefaultValuePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type>(
        parentDriver,
        name,
        string("shiftValue internalField pointVectorField,defaultValue primitive ")+pTraits<Type>::typeName
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class Type>
tmp<pointField> shiftFieldExpressionDefaultValuePluginFunction<Type>::displacement()
{
    return tmp<pointField>(
        new pointField(
            shiftValue_
        )
    );
}

template<class Type>
void shiftFieldExpressionDefaultValuePluginFunction<Type>::setArgument(
    label index,
    const Type &content
) {
    assert(index==2);

    this->defaultValue_=content;
}


template<class Type>
void shiftFieldExpressionDefaultValuePluginFunction<Type>::setArgument(
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
