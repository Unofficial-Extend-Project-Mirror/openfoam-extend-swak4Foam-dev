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

#include "facConvectionDivFunctionPlugin.H"
#include "FaFieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "convectionScheme.H"

namespace Foam {

defineTemplateTypeNameAndDebug(facConvectionDivFunctionPlugin<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facConvectionDivFunctionPlugin,scalar , name, facConvectionDivScalar);

defineTemplateTypeNameAndDebug(facConvectionDivFunctionPlugin<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facConvectionDivFunctionPlugin,vector , name, facConvectionDivVector);

defineTemplateTypeNameAndDebug(facConvectionDivFunctionPlugin<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facConvectionDivFunctionPlugin,tensor , name, facConvectionDivTensor);

// defineTemplateTypeNameAndDebug(facConvectionDivFunctionPlugin<symmTensor>,1);
// addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facConvectionDivFunctionPlugin,symmTensor , name, facConvectionDivSymmTensor);

// defineTemplateTypeNameAndDebug(facConvectionDivFunctionPlugin<sphericalTensor>,1);
// addNamedTemplateToRunTimeSelectionTable(FaFieldValuePluginFunction, facConvectionDivFunctionPlugin,sphericalTensor , name, facConvectionDivSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
facConvectionDivFunctionPlugin<T>::facConvectionDivFunctionPlugin(
    const FaFieldValueExpressionDriver &parentDriver,
    const word &name
):
    FaFieldValuePluginFunction(
        parentDriver,
        name,
        word(pTraits<resultType>::typeName),
        string(
            "flow internalFaField surfaceScalarFaField,original internalFaField "
            +pTraits<originalType>::typeName
            +",specString primitive string"
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void facConvectionDivFunctionPlugin<T>::doEvaluation()
{
    IStringStream spec(specString_);

    const faMesh &aMesh=dynamicCast<FaFieldValueExpressionDriver>(
        parentDriver()
    ).aMesh();

    tmp<fa::convectionScheme<T> > scheme(
        fa::convectionScheme<T>::New(
            aMesh,
            flow_(),
            spec
        )
    );

    autoPtr<resultType> pInterpol(
        new resultType(
            IOobject(
                "facInterpolated"+this->original_->name(),
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            scheme().facDiv(flow_(),original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T>
void facConvectionDivFunctionPlugin<T>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0 || index==1);
    if(index==1) {
        this->original_.set(
            new originalType(
                dynamicCast<const FaFieldValueExpressionDriver &>(
                    driver
                ).getResult<originalType>()
            )
        );
    } else {
        this->flow_.set(
            new edgeScalarField(
                dynamicCast<const FaFieldValueExpressionDriver &>(
                    driver
                ).getResult<edgeScalarField>()
            )
        );
    }
}

template <class T>
void facConvectionDivFunctionPlugin<T>::setArgument(
    label index,
    const string &value
)
{
    assert(index==2);

    specString_=value;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
