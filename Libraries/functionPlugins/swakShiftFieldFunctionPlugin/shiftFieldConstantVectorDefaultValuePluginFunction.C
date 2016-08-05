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

defineTemplateTypeNameAndDebug(shiftFieldConstantVectorDefaultValuePluginFunction<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorDefaultValuePluginFunction,scalar , name, shiftConstantScalarFieldDefault);

    // this can't be instantiated because of conflicting types
    // defineTemplateTypeNameAndDebug(shiftFieldConstantVectorDefaultValuePluginFunction<vector>,1);
    // addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorDefaultValuePluginFunction,vector , name, shiftConstantVectorFieldDefault);

defineTemplateTypeNameAndDebug(shiftFieldConstantVectorDefaultValuePluginFunction<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorDefaultValuePluginFunction,tensor , name, shiftConstantTensorFieldDefault);
defineTemplateTypeNameAndDebug(shiftFieldConstantVectorDefaultValuePluginFunction<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorDefaultValuePluginFunction,symmTensor , name, shiftConstantSymmTensorFieldDefault);
defineTemplateTypeNameAndDebug(shiftFieldConstantVectorDefaultValuePluginFunction<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorDefaultValuePluginFunction,sphericalTensor , name, shiftConstantSphericalTensorFieldDefault);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
shiftFieldConstantVectorDefaultValuePluginFunction<Type>::shiftFieldConstantVectorDefaultValuePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type,meshToMesh::imCellVolumeWeight>(
        parentDriver,
        name,
        string("shiftValue primitive vector,defaultValue primitive ")+pTraits<Type>::typeName
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
tmp<pointField> shiftFieldConstantVectorDefaultValuePluginFunction<Type>::displacement()
{
    return tmp<pointField>(
        new pointField(
            this->mesh().points().size(),
            shiftValue_
        )
    );
}



template<class Type>
void shiftFieldConstantVectorDefaultValuePluginFunction<Type>::setArgument(
    label index,
    const Type &content
) {
    assert(index==2);

    this->defaultValue_=content;
}

template<class Type>
void shiftFieldConstantVectorDefaultValuePluginFunction<Type>::setArgument(
    label index,
    const vector &content
) {
    assert(index==1);

    this->shiftValue_=content;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
