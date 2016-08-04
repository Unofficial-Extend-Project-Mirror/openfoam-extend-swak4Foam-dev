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

defineTemplateTypeNameAndDebug(shiftFieldConstantVectorPluginFunction<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorPluginFunction,scalar , name, shiftConstantScalarField);
defineTemplateTypeNameAndDebug(shiftFieldConstantVectorPluginFunction<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorPluginFunction,vector , name, shiftConstantVectorField);
defineTemplateTypeNameAndDebug(shiftFieldConstantVectorPluginFunction<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorPluginFunction,tensor , name, shiftConstantTensorField);
defineTemplateTypeNameAndDebug(shiftFieldConstantVectorPluginFunction<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorPluginFunction,symmTensor , name, shiftConstantSymmTensorField);
defineTemplateTypeNameAndDebug(shiftFieldConstantVectorPluginFunction<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, shiftFieldConstantVectorPluginFunction,sphericalTensor , name, shiftConstantSphericalTensorField);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
shiftFieldConstantVectorPluginFunction<Type>::shiftFieldConstantVectorPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    shiftFieldGeneralPluginFunction<Type>(
        parentDriver,
        name,
        "shiftValue primitive vector"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class Type>
tmp<pointField> shiftFieldConstantVectorPluginFunction<Type>::displacement()
{
    return tmp<pointField>(
        new pointField(
            this->mesh().points().size(),
            shiftValue_
        )
    );
}

template<class Type>
void shiftFieldConstantVectorPluginFunction<Type>::setArgument(
    label index,
    const vector &content
) {
    assert(index==1);

    shiftValue_=content;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
