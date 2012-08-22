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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "facLnGradFunctionPlugin.H"
#include "FaFieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "lnGradScheme.H"

namespace Foam {

defineTemplateTypeNameAndDebug(facLnGradFunctionPlugin<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facLnGradFunctionPlugin,scalar , name, facLnGradScalar);

defineTemplateTypeNameAndDebug(facLnGradFunctionPlugin<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facLnGradFunctionPlugin,vector , name, facLnGradVector);

defineTemplateTypeNameAndDebug(facLnGradFunctionPlugin<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facLnGradFunctionPlugin,tensor , name, facLnGradTensor);

// defineTemplateTypeNameAndDebug(facLnGradFunctionPlugin<symmTensor>,1);
// addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facLnGradFunctionPlugin,symmTensor , name, facLnGradSymmTensor);

// defineTemplateTypeNameAndDebug(facLnGradFunctionPlugin<sphericalTensor>,1);
// addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facLnGradFunctionPlugin,sphericalTensor , name, facLnGradSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
facLnGradFunctionPlugin<T>::facLnGradFunctionPlugin(
    const FaFieldValueExpressionDriver &parentDriver,
    const word &name
):
    FaFieldValuePluginFunction(
        parentDriver,
        name,
        word(pTraits<resultType>::typeName),
        string(
            "original internalFaField "
            +pTraits<originalType>::typeName
            +",specString primitive string"
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void facLnGradFunctionPlugin<T>::doEvaluation()
{
    IStringStream spec(specString_);

    const faMesh &aMesh=dynamicCast<FaFieldValueExpressionDriver>(
        parentDriver()
    ).aMesh();

    tmp<fa::lnGradScheme<T> > scheme(
        fa::lnGradScheme<T>::New(
            aMesh,
            spec
        )
    );

    autoPtr<resultType> pInterpol(
        new resultType(
            IOobject(
                "facLnGrad"+this->original_->name(),
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            scheme().lnGrad(original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T>
void facLnGradFunctionPlugin<T>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0);
    this->original_.set(
        new originalType(
            dynamicCast<const FaFieldValueExpressionDriver &>(
                driver
            ).getResult<originalType>()
        )
    );
}

template <class T>
void facLnGradFunctionPlugin<T>::setArgument(
    label index,
    const string &value
)
{
    assert(index==1);

    specString_=value;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
