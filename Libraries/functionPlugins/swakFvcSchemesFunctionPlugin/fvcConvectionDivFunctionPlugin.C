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

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvcConvectionDivFunctionPlugin.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "convectionScheme.H"

namespace Foam {

defineTemplateTypeNameAndDebug(fvcConvectionDivFunctionPlugin<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcConvectionDivFunctionPlugin,scalar , name, fvcConvectionDivScalar);

defineTemplateTypeNameAndDebug(fvcConvectionDivFunctionPlugin<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcConvectionDivFunctionPlugin,vector , name, fvcConvectionDivVector);

defineTemplateTypeNameAndDebug(fvcConvectionDivFunctionPlugin<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcConvectionDivFunctionPlugin,tensor , name, fvcConvectionDivTensor);

defineTemplateTypeNameAndDebug(fvcConvectionDivFunctionPlugin<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcConvectionDivFunctionPlugin,symmTensor , name, fvcConvectionDivSymmTensor);

defineTemplateTypeNameAndDebug(fvcConvectionDivFunctionPlugin<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcConvectionDivFunctionPlugin,sphericalTensor , name, fvcConvectionDivSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
fvcConvectionDivFunctionPlugin<T>::fvcConvectionDivFunctionPlugin(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word(pTraits<resultType>::typeName),
        string(
            "flow internalField surfaceScalarField,original internalField "
            +pTraits<originalType>::typeName
            +",specString primitive string"
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void fvcConvectionDivFunctionPlugin<T>::doEvaluation()
{
    IStringStream spec(specString_);

    tmp<fv::convectionScheme<T> > scheme(
        fv::convectionScheme<T>::New(
            mesh(),
            flow_(),
            spec
        )
    );

    autoPtr<resultType> pInterpol(
        new resultType(
            IOobject(
                "fvcInterpolated"+this->original_->name(),
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            scheme().fvcDiv(flow_(),original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T>
void fvcConvectionDivFunctionPlugin<T>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0 || index==1);
    if(index==1) {
        this->original_.set(
            new originalType(
                dynamicCast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<originalType>()
            )
        );
    } else {
        this->flow_.set(
            new surfaceScalarField(
                dynamicCast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<surfaceScalarField>()
            )
        );
    }
}

template <class T>
void fvcConvectionDivFunctionPlugin<T>::setArgument(
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
