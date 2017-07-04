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

#include "fvcLaplacianFunctionPlugin.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "laplacianScheme.H"

namespace Foam {

defineTemplateTypeNameAndDebug(fvcLaplacianFunctionPlugin<scalar>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcLaplacianFunctionPlugin,scalar , name, fvcLaplacianScalar);

defineTemplateTypeNameAndDebug(fvcLaplacianFunctionPlugin<vector>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcLaplacianFunctionPlugin,vector , name, fvcLaplacianVector);

defineTemplateTypeNameAndDebug(fvcLaplacianFunctionPlugin<tensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcLaplacianFunctionPlugin,tensor , name, fvcLaplacianTensor);

defineTemplateTypeNameAndDebug(fvcLaplacianFunctionPlugin<symmTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcLaplacianFunctionPlugin,symmTensor , name, fvcLaplacianSymmTensor);

defineTemplateTypeNameAndDebug(fvcLaplacianFunctionPlugin<sphericalTensor>,1);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, fvcLaplacianFunctionPlugin,sphericalTensor , name, fvcLaplacianSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T>
fvcLaplacianFunctionPlugin<T>::fvcLaplacianFunctionPlugin(
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
void fvcLaplacianFunctionPlugin<T>::doEvaluation()
{
    IStringStream spec(specString_);

    tmp<fv::laplacianScheme<T,scalar> > scheme(
        fv::laplacianScheme<T,scalar>::New(
            mesh(),
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
            const_cast<fv::laplacianScheme<T,scalar>&>(scheme()).fvcLaplacian(original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T>
void fvcLaplacianFunctionPlugin<T>::setArgument(
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
void fvcLaplacianFunctionPlugin<T>::setArgument(
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
