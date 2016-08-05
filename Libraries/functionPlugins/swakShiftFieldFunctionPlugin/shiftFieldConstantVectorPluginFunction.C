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
#include "shiftFieldConstantVectorPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "meshToMesh.H"
#include "MeshInterpolationOrder.H"

namespace Foam {

typedef shiftFieldConstantVectorPluginFunction<scalar,meshToMesh::imCellVolumeWeight> shiftScalarWeight;
defineTemplateTypeNameAndDebug(shiftScalarWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarWeight , name, shiftConstantScalarField);
typedef shiftFieldConstantVectorPluginFunction<scalar,meshToMesh::imMapNearest> shiftScalarMap;
defineTemplateTypeNameAndDebug(shiftScalarMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarMap , name, mapConstantScalarField);

typedef shiftFieldConstantVectorPluginFunction<vector,meshToMesh::imCellVolumeWeight> shiftVectorWeight;
defineTemplateTypeNameAndDebug(shiftVectorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorWeight , name, shiftConstantVectorField);
typedef shiftFieldConstantVectorPluginFunction<vector,meshToMesh::imMapNearest> shiftVectorMap;
defineTemplateTypeNameAndDebug(shiftVectorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorMap , name, mapConstantVectorField);

typedef shiftFieldConstantVectorPluginFunction<tensor,meshToMesh::imCellVolumeWeight> shiftTensorWeight;
defineTemplateTypeNameAndDebug(shiftTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorWeight , name, shiftConstantTensorField);
typedef shiftFieldConstantVectorPluginFunction<tensor,meshToMesh::imMapNearest> shiftTensorMap;
defineTemplateTypeNameAndDebug(shiftTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorMap , name, mapConstantTensorField);

typedef shiftFieldConstantVectorPluginFunction<symmTensor,meshToMesh::imCellVolumeWeight> shiftSymmTensorWeight;
defineTemplateTypeNameAndDebug(shiftSymmTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorWeight , name, shiftConstantSymmTensorField);
typedef shiftFieldConstantVectorPluginFunction<symmTensor,meshToMesh::imMapNearest> shiftSymmTensorMap;
defineTemplateTypeNameAndDebug(shiftSymmTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorMap , name, mapConstantSymmTensorField);

typedef shiftFieldConstantVectorPluginFunction<sphericalTensor,meshToMesh::imCellVolumeWeight> shiftSphericalTensorWeight;
defineTemplateTypeNameAndDebug(shiftSphericalTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorWeight , name, shiftConstantSphericalTensorField);
typedef shiftFieldConstantVectorPluginFunction<sphericalTensor,meshToMesh::imMapNearest> shiftSphericalTensorMap;
defineTemplateTypeNameAndDebug(shiftSphericalTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorMap , name, mapConstantSphericalTensorField);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
shiftFieldConstantVectorPluginFunction<Type,Order>::shiftFieldConstantVectorPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type,Order>(
        parentDriver,
        name,
        "shiftValue primitive vector"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class Type,meshToMeshOrder Order>
tmp<pointField> shiftFieldConstantVectorPluginFunction<Type,Order>::displacement()
{
    return tmp<pointField>(
        new pointField(
            this->mesh().points().size(),
            shiftValue_
        )
    );
}

template<class Type,meshToMeshOrder Order>
void shiftFieldConstantVectorPluginFunction<Type,Order>::setArgument(
    label index,
    const vector &content
) {
    assert(index==1);

    shiftValue_=content;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
