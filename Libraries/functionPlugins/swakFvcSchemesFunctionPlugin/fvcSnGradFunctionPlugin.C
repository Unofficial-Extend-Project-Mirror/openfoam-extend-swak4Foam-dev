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
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvcSnGradFunctionPlugin.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "snGradScheme.H"

namespace Foam {

defineTemplateTypeNameAndDebug(fvcSnGradFunctionPlugin<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcSnGradFunctionPlugin,scalar , name, fvcSnGradScalar);

defineTemplateTypeNameAndDebug(fvcSnGradFunctionPlugin<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcSnGradFunctionPlugin,vector , name, fvcSnGradVector);

defineTemplateTypeNameAndDebug(fvcSnGradFunctionPlugin<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcSnGradFunctionPlugin,tensor , name, fvcSnGradTensor);

defineTemplateTypeNameAndDebug(fvcSnGradFunctionPlugin<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcSnGradFunctionPlugin,symmTensor , name, fvcSnGradSymmTensor);

defineTemplateTypeNameAndDebug(fvcSnGradFunctionPlugin<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcSnGradFunctionPlugin,sphericalTensor , name, fvcSnGradSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
fvcSnGradFunctionPlugin<T>::fvcSnGradFunctionPlugin(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word(pTraits<resultType>::typeName),
        string(
            "original internalField "
            +pTraits<originalType>::typeName
            +",specString primitive string"
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void fvcSnGradFunctionPlugin<T>::doEvaluation()
{
    IStringStream spec(specString_);

    tmp<fv::snGradScheme<T> > scheme(
        fv::snGradScheme<T>::New(
            mesh(),
            spec
        )
    );

    autoPtr<resultType> pInterpol(
        new resultType(
            IOobject(
                "fvcSnGrad"+this->original_->name(),
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            scheme().snGrad(original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T>
void fvcSnGradFunctionPlugin<T>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0);
    this->original_.set(
        new originalType(
            //            dynamicCast<const FieldValueExpressionDriver &>(
            dynamic_cast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<originalType>()
        )
    );
}

template <class T>
void fvcSnGradFunctionPlugin<T>::setArgument(
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
