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

typedef shiftFieldExpressionPluginFunction<scalar,meshToMesh::imCellVolumeWeight> shiftScalarWeight;
defineTemplateTypeNameAndDebug(shiftScalarWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarWeight , name, shiftExpressionScalarField);
typedef shiftFieldExpressionPluginFunction<scalar,meshToMesh::imMapNearest> shiftScalarMap;
defineTemplateTypeNameAndDebug(shiftScalarMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftScalarMap , name, mapExpressionScalarField);

typedef shiftFieldExpressionPluginFunction<vector,meshToMesh::imCellVolumeWeight> shiftVectorWeight;
defineTemplateTypeNameAndDebug(shiftVectorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorWeight , name, shiftExpressionVectorField);
typedef shiftFieldExpressionPluginFunction<vector,meshToMesh::imMapNearest> shiftVectorMap;
defineTemplateTypeNameAndDebug(shiftVectorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftVectorMap , name, mapExpressionVectorField);

typedef shiftFieldExpressionPluginFunction<tensor,meshToMesh::imCellVolumeWeight> shiftTensorWeight;
defineTemplateTypeNameAndDebug(shiftTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorWeight , name, shiftExpressionTensorField);
typedef shiftFieldExpressionPluginFunction<tensor,meshToMesh::imMapNearest> shiftTensorMap;
defineTemplateTypeNameAndDebug(shiftTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftTensorMap , name, mapExpressionTensorField);

typedef shiftFieldExpressionPluginFunction<symmTensor,meshToMesh::imCellVolumeWeight> shiftSymmTensorWeight;
defineTemplateTypeNameAndDebug(shiftSymmTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorWeight , name, shiftExpressionSymmTensorField);
typedef shiftFieldExpressionPluginFunction<symmTensor,meshToMesh::imMapNearest> shiftSymmTensorMap;
defineTemplateTypeNameAndDebug(shiftSymmTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSymmTensorMap , name, mapExpressionSymmTensorField);

typedef shiftFieldExpressionPluginFunction<sphericalTensor,meshToMesh::imCellVolumeWeight> shiftSphericalTensorWeight;
defineTemplateTypeNameAndDebug(shiftSphericalTensorWeight,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorWeight , name, shiftExpressionSphericalTensorField);
typedef shiftFieldExpressionPluginFunction<sphericalTensor,meshToMesh::imMapNearest> shiftSphericalTensorMap;
defineTemplateTypeNameAndDebug(shiftSphericalTensorMap,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, shiftSphericalTensorMap , name, mapExpressionSphericalTensorField);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
shiftFieldExpressionPluginFunction<Type,Order>::shiftFieldExpressionPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type,Order>(
        parentDriver,
        name,
        "shiftValue internalField pointVectorField"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class Type,meshToMeshOrder Order>
tmp<pointField> shiftFieldExpressionPluginFunction<Type,Order>::displacement()
{
    return tmp<pointField>(
        new pointField(
            shiftValue_
        )
    );
}

template<class Type,meshToMeshOrder Order>
void shiftFieldExpressionPluginFunction<Type,Order>::setArgument(
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
