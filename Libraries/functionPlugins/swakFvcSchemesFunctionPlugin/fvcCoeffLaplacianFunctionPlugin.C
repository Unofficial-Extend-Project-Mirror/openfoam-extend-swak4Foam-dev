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

#include "fvcCoeffLaplacianFunctionPlugin.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "laplacianScheme.H"

namespace Foam {

#define addNamedTemplate2ToRunTimeSelectionTable\
(baseType,thisType,Targ1,Targ2,argNames,lookup)                          \
                                                                              \
    /* Add the thisType constructor function to the table, find by lookup */  \
    baseType::add##argNames##ConstructorToTable< thisType< Targ1,Targ2 > >   \
        add_##lookup##_##thisType##Targ1##Targ2##argNames##ConstructorTo##baseType##Table_(#lookup)

    // scalar

typedef fvcCoeffLaplacianFunctionPlugin<scalar,scalar> laplacianScalarScalar;
defineTemplateTypeNameAndDebug(laplacianScalarScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,scalar,scalar,name,fvcCoeffScalarLaplacianScalar);

// typedef fvcCoeffLaplacianFunctionPlugin<scalar,vector> laplacianScalarVector;
// defineTemplateTypeNameAndDebug(laplacianScalarVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,scalar,vector,name,fvcCoeffVectorLaplacianScalar);

typedef fvcCoeffLaplacianFunctionPlugin<scalar,tensor> laplacianScalarTensor;
defineTemplateTypeNameAndDebug(laplacianScalarTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,scalar,tensor,name,fvcCoeffTensorLaplacianScalar);

typedef fvcCoeffLaplacianFunctionPlugin<scalar,symmTensor> laplacianScalarSymmTensor;
defineTemplateTypeNameAndDebug(laplacianScalarSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,scalar,symmTensor,name,fvcCoeffSymmTensorLaplacianScalar);

// typedef fvcCoeffLaplacianFunctionPlugin<scalar,sphericalTensor> laplacianScalarSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianScalarSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,scalar,sphericalTensor,name,fvcCoeffSphericalTensorLaplacianScalar);

    // vector

typedef fvcCoeffLaplacianFunctionPlugin<vector,scalar> laplacianVectorScalar;
defineTemplateTypeNameAndDebug(laplacianVectorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,vector,scalar,name,fvcCoeffScalarLaplacianVector);

// typedef fvcCoeffLaplacianFunctionPlugin<vector,vector> laplacianVectorVector;
// defineTemplateTypeNameAndDebug(laplacianVectorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,vector,vector,name,fvcCoeffVectorLaplacianVector);

typedef fvcCoeffLaplacianFunctionPlugin<vector,tensor> laplacianVectorTensor;
defineTemplateTypeNameAndDebug(laplacianVectorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,vector,tensor,name,fvcCoeffTensorLaplacianVector);

typedef fvcCoeffLaplacianFunctionPlugin<vector,symmTensor> laplacianVectorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianVectorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,vector,symmTensor,name,fvcCoeffSymmTensorLaplacianVector);

// typedef fvcCoeffLaplacianFunctionPlugin<vector,sphericalTensor> laplacianVectorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianVectorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,vector,sphericalTensor,name,fvcCoeffSphericalTensorLaplacianVector);

    // tensor

typedef fvcCoeffLaplacianFunctionPlugin<tensor,scalar> laplacianTensorScalar;
defineTemplateTypeNameAndDebug(laplacianTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,tensor,scalar,name,fvcCoeffScalarLaplacianTensor);

// typedef fvcCoeffLaplacianFunctionPlugin<tensor,vector> laplacianTensorVector;
// defineTemplateTypeNameAndDebug(laplacianTensorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,tensor,vector,name,fvcCoeffVectorLaplacianTensor);

typedef fvcCoeffLaplacianFunctionPlugin<tensor,tensor> laplacianTensorTensor;
defineTemplateTypeNameAndDebug(laplacianTensorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,tensor,tensor,name,fvcCoeffTensorLaplacianTensor);

typedef fvcCoeffLaplacianFunctionPlugin<tensor,symmTensor> laplacianTensorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianTensorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,tensor,symmTensor,name,fvcCoeffSymmTensorLaplacianTensor);

// typedef fvcCoeffLaplacianFunctionPlugin<tensor,sphericalTensor> laplacianTensorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianTensorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,tensor,sphericalTensor,name,fvcCoeffSphericalTensorLaplacianTensor);

    // symmTensor

typedef fvcCoeffLaplacianFunctionPlugin<symmTensor,scalar> laplacianSymmTensorScalar;
defineTemplateTypeNameAndDebug(laplacianSymmTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,symmTensor,scalar,name,fvcCoeffScalarLaplacianSymmTensor);


// typedef fvcCoeffLaplacianFunctionPlugin<symmTensor,vector> laplacianSymmTensorVector;
// defineTemplateTypeNameAndDebug(laplacianSymmTensorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,symmTensor,vector,name,fvcCoeffVectorLaplacianSymmTensor);

typedef fvcCoeffLaplacianFunctionPlugin<symmTensor,tensor> laplacianSymmTensorTensor;
defineTemplateTypeNameAndDebug(laplacianSymmTensorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,symmTensor,tensor,name,fvcCoeffTensorLaplacianSymmTensor);

typedef fvcCoeffLaplacianFunctionPlugin<symmTensor,symmTensor> laplacianSymmTensorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianSymmTensorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,symmTensor,symmTensor,name,fvcCoeffSymmTensorLaplacianSymmTensor);

// typedef fvcCoeffLaplacianFunctionPlugin<symmTensor,sphericalTensor> laplacianSymmTensorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianSymmTensorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,symmTensor,sphericalTensor,name,fvcCoeffSphericalTensorLaplacianSymmTensor);

    // sphericalTensor

typedef fvcCoeffLaplacianFunctionPlugin<sphericalTensor,scalar> laplacianSphericalTensorScalar;
defineTemplateTypeNameAndDebug(laplacianSphericalTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,sphericalTensor,scalar,name,fvcCoeffScalarLaplacianSphericalTensor);


// typedef fvcCoeffLaplacianFunctionPlugin<sphericalTensor,vector> laplacianSphericalTensorVector;
// defineTemplateTypeNameAndDebug(laplacianSphericalTensorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,sphericalTensor,vector,name,fvcCoeffVectorLaplacianSphericalTensor);

typedef fvcCoeffLaplacianFunctionPlugin<sphericalTensor,tensor> laplacianSphericalTensorTensor;
defineTemplateTypeNameAndDebug(laplacianSphericalTensorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,sphericalTensor,tensor,name,fvcCoeffTensorLaplacianSphericalTensor);

typedef fvcCoeffLaplacianFunctionPlugin<sphericalTensor,symmTensor> laplacianSphericalTensorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianSphericalTensorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,sphericalTensor,symmTensor,name,fvcCoeffSymmTensorLaplacianSphericalTensor);

// typedef fvcCoeffLaplacianFunctionPlugin<sphericalTensor,sphericalTensor> laplacianSphericalTensorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianSphericalTensorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcCoeffLaplacianFunctionPlugin,sphericalTensor,sphericalTensor,name,fvcCoeffSphericalTensorLaplacianSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T,class GT>
fvcCoeffLaplacianFunctionPlugin<T,GT>::fvcCoeffLaplacianFunctionPlugin(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word(pTraits<resultType>::typeName),
        string(
            "coeff internalField "
            +pTraits<coeffType>::typeName
            +",original internalField "
            +pTraits<originalType>::typeName
            +",specString primitive string"
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T,class GT>
void fvcCoeffLaplacianFunctionPlugin<T,GT>::doEvaluation()
{
    IStringStream spec(specString_);

    tmp<fv::laplacianScheme<T,GT> > scheme(
        fv::laplacianScheme<T,GT>::New(
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
            scheme().fvcLaplacian(coeff_(),original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T,class GT>
void fvcCoeffLaplacianFunctionPlugin<T,GT>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0 || index==1);
    if(index==0) {
        this->coeff_.set(
            new coeffType(
                //                dynamicCast<const FieldValueExpressionDriver &>(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<coeffType>()
            )
        );
    } else {
        this->original_.set(
            new originalType(
                //                dynamicCast<const FieldValueExpressionDriver &>(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<originalType>()
            )
        );
    }
}

template <class T,class GT>
void fvcCoeffLaplacianFunctionPlugin<T,GT>::setArgument(
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
