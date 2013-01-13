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

#include "facCoeffLaplacianFunctionPlugin.H"
#include "FaFieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "faMatrix.H"
#include "faLaplacianScheme.H"

namespace Foam {

#define addNamedTemplate2ToRunTimeSelectionTable\
(baseType,thisType,Targ1,Targ2,argNames,lookup)                          \
                                                                              \
    /* Add the thisType constructor function to the table, find by lookup */  \
    baseType::add##argNames##ConstructorToTable< thisType< Targ1,Targ2 > >   \
        add_##lookup##_##thisType##Targ1##Targ2##argNames##ConstructorTo##baseType##Table_(#lookup)

typedef facCoeffLaplacianFunctionPlugin<scalar,scalar> laplacianScalarScalar;
defineTemplateTypeNameAndDebug(laplacianScalarScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,scalar,scalar,name,facCoeffScalarLaplacianScalar);

// typedef facCoeffLaplacianFunctionPlugin<scalar,symmTensor> laplacianScalarSymmTensor;
// defineTemplateTypeNameAndDebug(laplacianScalarSymmTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,scalar,symmTensor,name,facCoeffSymmTensorLaplacianScalar);

    //typedef facCoeffLaplacianFunctionPlugin<scalar,tensor> laplacianScalarTensor;
    // defineTemplateTypeNameAndDebug(laplacianScalarTensor,0);
    // addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,scalar,tensor,name,facCoeffTensorLaplacianScalar);

typedef facCoeffLaplacianFunctionPlugin<vector,scalar> laplacianVectorScalar;
defineTemplateTypeNameAndDebug(laplacianVectorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,vector,scalar,name,facCoeffScalarLaplacianVector);

typedef facCoeffLaplacianFunctionPlugin<tensor,scalar> laplacianTensorScalar;
defineTemplateTypeNameAndDebug(laplacianTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,tensor,scalar,name,facCoeffScalarLaplacianTensor);

// typedef facCoeffLaplacianFunctionPlugin<symmTensor,scalar> laplacianSymmTensorScalar;
// defineTemplateTypeNameAndDebug(laplacianSymmTensorScalar,0);
// addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,symmTensor,scalar,name,facCoeffScalarLaplacianSymmTensor);

// typedef facCoeffLaplacianFunctionPlugin<sphericalTensor,scalar> laplacianSphericalTensorScalar;
// defineTemplateTypeNameAndDebug(laplacianSphericalTensorScalar,0);
// addNamedTemplate2ToRunTimeSelectionTable(FaFieldValuePluginFunction, facCoeffLaplacianFunctionPlugin,sphericalTensor,scalar,name,facCoeffScalarLaplacianSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T,class GT>
facCoeffLaplacianFunctionPlugin<T,GT>::facCoeffLaplacianFunctionPlugin(
    const FaFieldValueExpressionDriver &parentDriver,
    const word &name
):
    FaFieldValuePluginFunction(
        parentDriver,
        name,
        word(pTraits<resultType>::typeName),
        string(
            "coeff internalFaField "
            +pTraits<coeffType>::typeName
            +",original internalFaField "
            +pTraits<originalType>::typeName
            +",specString primitive string"
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T,class GT>
void facCoeffLaplacianFunctionPlugin<T,GT>::doEvaluation()
{
    IStringStream spec(specString_);

    const faMesh &aMesh=dynamicCast<FaFieldValueExpressionDriver>(
        parentDriver()
    ).aMesh();

    tmp<fa::laplacianScheme<T> > scheme(
        fa::laplacianScheme<T>::New(
            aMesh,
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
            scheme().facLaplacian(coeff_(),original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T,class GT>
void facCoeffLaplacianFunctionPlugin<T,GT>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0 || index==1);
    if(index==0) {
        this->coeff_.set(
            new coeffType(
                dynamicCast<const FaFieldValueExpressionDriver &>(
                    driver
                ).getResult<coeffType>()
            )
        );
    } else {
        this->original_.set(
            new originalType(
                dynamicCast<const FaFieldValueExpressionDriver &>(
                    driver
                ).getResult<originalType>()
            )
        );
    }
}

template <class T,class GT>
void facCoeffLaplacianFunctionPlugin<T,GT>::setArgument(
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
