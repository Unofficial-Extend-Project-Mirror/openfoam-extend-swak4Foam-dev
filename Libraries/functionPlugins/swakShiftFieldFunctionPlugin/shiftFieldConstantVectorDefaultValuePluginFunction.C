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
#include "shiftFieldConstantVectorDefaultValuePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "meshToMesh.H"
#include "MeshInterpolationOrder.H"

namespace Foam {

typedef shiftFieldConstantVectorDefaultValuePluginFunction<scalar,SHIFT_METHOD> shiftScalarWeight;
defineTemplateTypeNameAndDebug(shiftScalarWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarWeight , name, shiftConstantScalarFieldDefault);
typedef shiftFieldConstantVectorDefaultValuePluginFunction<scalar,MAP_METHOD> shiftScalarMap;
defineTemplateTypeNameAndDebug(shiftScalarMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarMap , name, mapConstantScalarFieldDefault);

// typedef shiftFieldConstantVectorDefaultValuePluginFunction<vector,SHIFT_METHOD> shiftVectorWeight;
// defineTemplateTypeNameAndDebug(shiftVectorWeight,1);
// addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorWeight , name, shiftConstantVectorFieldDefault);
// typedef shiftFieldConstantVectorDefaultValuePluginFunction<vector,MAP_METHOD> shiftVectorMap;
// defineTemplateTypeNameAndDebug(shiftVectorMap,1);
// addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorMap , name, mapConstantVectorFieldDefault);

#ifndef FOAM_MESHTOMESH_MAPSRCTOTGT_NO_TENSOR
typedef shiftFieldConstantVectorDefaultValuePluginFunction<tensor,SHIFT_METHOD> shiftTensorWeight;
defineTemplateTypeNameAndDebug(shiftTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorWeight , name, shiftConstantTensorFieldDefault);
typedef shiftFieldConstantVectorDefaultValuePluginFunction<tensor,MAP_METHOD> shiftTensorMap;
defineTemplateTypeNameAndDebug(shiftTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorMap , name, mapConstantTensorFieldDefault);

typedef shiftFieldConstantVectorDefaultValuePluginFunction<symmTensor,SHIFT_METHOD> shiftSymmTensorWeight;
defineTemplateTypeNameAndDebug(shiftSymmTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorWeight , name, shiftConstantSymmTensorFieldDefault);
typedef shiftFieldConstantVectorDefaultValuePluginFunction<symmTensor,MAP_METHOD> shiftSymmTensorMap;
defineTemplateTypeNameAndDebug(shiftSymmTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorMap , name, mapConstantSymmTensorFieldDefault);

typedef shiftFieldConstantVectorDefaultValuePluginFunction<sphericalTensor,SHIFT_METHOD> shiftSphericalTensorWeight;
defineTemplateTypeNameAndDebug(shiftSphericalTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorWeight , name, shiftConstantSphericalTensorFieldDefault);
typedef shiftFieldConstantVectorDefaultValuePluginFunction<sphericalTensor,MAP_METHOD> shiftSphericalTensorMap;
defineTemplateTypeNameAndDebug(shiftSphericalTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorMap , name, mapConstantSphericalTensorFieldDefault);
#endif



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
shiftFieldConstantVectorDefaultValuePluginFunction<Type,Order>::shiftFieldConstantVectorDefaultValuePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type,Order>(
        parentDriver,
        name,
        string("shiftValue primitive vector,defaultValue primitive ")+pTraits<Type>::typeName
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
tmp<pointField> shiftFieldConstantVectorDefaultValuePluginFunction<Type,Order>::displacement()
{
    return tmp<pointField>(
        new pointField(
            this->mesh().points().size(),
            shiftValue_
        )
    );
}



template<class Type,meshToMeshOrder Order>
void shiftFieldConstantVectorDefaultValuePluginFunction<Type,Order>::setArgument(
    label index,
    const Type &content
) {
    assert(index==2);

    this->defaultValue_=content;
}

template<class Type,meshToMeshOrder Order>
void shiftFieldConstantVectorDefaultValuePluginFunction<Type,Order>::setArgument(
    label index,
    const vector &content
) {
    assert(index==1);

    this->shiftValue_=content;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
