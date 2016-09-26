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

typedef shiftFieldExpressionDefaultValuePluginFunction<scalar,SHIFT_METHOD> shiftScalarWeight;
defineTemplateTypeNameAndDebug(shiftScalarWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarWeight , name, shiftExpressionScalarFieldDefault);
typedef shiftFieldExpressionDefaultValuePluginFunction<scalar,MAP_METHOD> shiftScalarMap;
defineTemplateTypeNameAndDebug(shiftScalarMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarMap , name, mapExpressionScalarFieldDefault);

typedef shiftFieldExpressionDefaultValuePluginFunction<vector,SHIFT_METHOD> shiftVectorWeight;
defineTemplateTypeNameAndDebug(shiftVectorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorWeight , name, shiftExpressionVectorFieldDefault);
typedef shiftFieldExpressionDefaultValuePluginFunction<vector,MAP_METHOD> shiftVectorMap;
defineTemplateTypeNameAndDebug(shiftVectorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorMap , name, mapExpressionVectorFieldDefault);

#ifndef FOAM_MESHTOMESH_MAPSRCTOTGT_NO_TENSOR
typedef shiftFieldExpressionDefaultValuePluginFunction<tensor,SHIFT_METHOD> shiftTensorWeight;
defineTemplateTypeNameAndDebug(shiftTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorWeight , name, shiftExpressionTensorFieldDefault);
typedef shiftFieldExpressionDefaultValuePluginFunction<tensor,MAP_METHOD> shiftTensorMap;
defineTemplateTypeNameAndDebug(shiftTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorMap , name, mapExpressionTensorFieldDefault);

typedef shiftFieldExpressionDefaultValuePluginFunction<symmTensor,SHIFT_METHOD> shiftSymmTensorWeight;
defineTemplateTypeNameAndDebug(shiftSymmTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorWeight , name, shiftExpressionSymmTensorFieldDefault);
typedef shiftFieldExpressionDefaultValuePluginFunction<symmTensor,MAP_METHOD> shiftSymmTensorMap;
defineTemplateTypeNameAndDebug(shiftSymmTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorMap , name, mapExpressionSymmTensorFieldDefault);

typedef shiftFieldExpressionDefaultValuePluginFunction<sphericalTensor,SHIFT_METHOD> shiftSphericalTensorWeight;
defineTemplateTypeNameAndDebug(shiftSphericalTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorWeight , name, shiftExpressionSphericalTensorFieldDefault);
typedef shiftFieldExpressionDefaultValuePluginFunction<sphericalTensor,MAP_METHOD> shiftSphericalTensorMap;
defineTemplateTypeNameAndDebug(shiftSphericalTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorMap , name, mapExpressionSphericalTensorFieldDefault);
#endif


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
shiftFieldExpressionDefaultValuePluginFunction<Type,Order>::shiftFieldExpressionDefaultValuePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type,Order>(
        parentDriver,
        name,
        string("shiftValue internalField pointVectorField,defaultValue primitive ")+pTraits<Type>::typeName
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class Type,meshToMeshOrder Order>
tmp<pointField> shiftFieldExpressionDefaultValuePluginFunction<Type,Order>::displacement()
{
    return tmp<pointField>(
        new pointField(
            shiftValue_
        )
    );
}

template<class Type,meshToMeshOrder Order>
void shiftFieldExpressionDefaultValuePluginFunction<Type,Order>::setArgument(
    label index,
    const Type &content
) {
    assert(index==2);

    this->defaultValue_=content;
}


template<class Type,meshToMeshOrder Order>
void shiftFieldExpressionDefaultValuePluginFunction<Type,Order>::setArgument(
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
        shiftFieldGeneralPluginFunction<Type,Order>::setArgument(
            index,
            content,
            driver
        );
    }
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
