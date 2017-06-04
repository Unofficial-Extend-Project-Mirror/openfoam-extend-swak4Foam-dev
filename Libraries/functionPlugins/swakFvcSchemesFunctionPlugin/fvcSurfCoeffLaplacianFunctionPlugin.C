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

#include "fvcSurfCoeffLaplacianFunctionPlugin.H"
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

typedef fvcSurfCoeffLaplacianFunctionPlugin<scalar,scalar> laplacianScalarScalar;
defineTemplateTypeNameAndDebug(laplacianScalarScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,scalar,scalar,name,fvcSurfCoeffScalarLaplacianScalar);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<scalar,vector> laplacianScalarVector;
// defineTemplateTypeNameAndDebug(laplacianScalarVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,scalar,vector,name,fvcSurfCoeffVectorLaplacianScalar);

typedef fvcSurfCoeffLaplacianFunctionPlugin<scalar,tensor> laplacianScalarTensor;
defineTemplateTypeNameAndDebug(laplacianScalarTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,scalar,tensor,name,fvcSurfCoeffTensorLaplacianScalar);

typedef fvcSurfCoeffLaplacianFunctionPlugin<scalar,symmTensor> laplacianScalarSymmTensor;
defineTemplateTypeNameAndDebug(laplacianScalarSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,scalar,symmTensor,name,fvcSurfCoeffSymmTensorLaplacianScalar);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<scalar,sphericalTensor> laplacianScalarSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianScalarSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,scalar,sphericalTensor,name,fvcSurfCoeffSphericalTensorLaplacianScalar);

    // vector

typedef fvcSurfCoeffLaplacianFunctionPlugin<vector,scalar> laplacianVectorScalar;
defineTemplateTypeNameAndDebug(laplacianVectorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,vector,scalar,name,fvcSurfCoeffScalarLaplacianVector);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<vector,vector> laplacianVectorVector;
// defineTemplateTypeNameAndDebug(laplacianVectorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,vector,vector,name,fvcSurfCoeffVectorLaplacianVector);

typedef fvcSurfCoeffLaplacianFunctionPlugin<vector,tensor> laplacianVectorTensor;
defineTemplateTypeNameAndDebug(laplacianVectorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,vector,tensor,name,fvcSurfCoeffTensorLaplacianVector);

typedef fvcSurfCoeffLaplacianFunctionPlugin<vector,symmTensor> laplacianVectorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianVectorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,vector,symmTensor,name,fvcSurfCoeffSymmTensorLaplacianVector);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<vector,sphericalTensor> laplacianVectorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianVectorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,vector,sphericalTensor,name,fvcSurfCoeffSphericalTensorLaplacianVector);

    // tensor

typedef fvcSurfCoeffLaplacianFunctionPlugin<tensor,scalar> laplacianTensorScalar;
defineTemplateTypeNameAndDebug(laplacianTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,tensor,scalar,name,fvcSurfCoeffScalarLaplacianTensor);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<tensor,vector> laplacianTensorVector;
// defineTemplateTypeNameAndDebug(laplacianTensorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,tensor,vector,name,fvcSurfCoeffVectorLaplacianTensor);

typedef fvcSurfCoeffLaplacianFunctionPlugin<tensor,tensor> laplacianTensorTensor;
defineTemplateTypeNameAndDebug(laplacianTensorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,tensor,tensor,name,fvcSurfCoeffTensorLaplacianTensor);

typedef fvcSurfCoeffLaplacianFunctionPlugin<tensor,symmTensor> laplacianTensorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianTensorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,tensor,symmTensor,name,fvcSurfCoeffSymmTensorLaplacianTensor);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<tensor,sphericalTensor> laplacianTensorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianTensorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,tensor,sphericalTensor,name,fvcSurfCoeffSphericalTensorLaplacianTensor);

    // symmTensor

typedef fvcSurfCoeffLaplacianFunctionPlugin<symmTensor,scalar> laplacianSymmTensorScalar;
defineTemplateTypeNameAndDebug(laplacianSymmTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,symmTensor,scalar,name,fvcSurfCoeffScalarLaplacianSymmTensor);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<symmTensor,vector> laplacianSymmTensorVector;
// defineTemplateTypeNameAndDebug(laplacianSymmTensorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,symmTensor,vector,name,fvcSurfCoeffVectorLaplacianSymmTensor);

typedef fvcSurfCoeffLaplacianFunctionPlugin<symmTensor,tensor> laplacianSymmTensorTensor;
defineTemplateTypeNameAndDebug(laplacianSymmTensorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,symmTensor,tensor,name,fvcSurfCoeffTensorLaplacianSymmTensor);

typedef fvcSurfCoeffLaplacianFunctionPlugin<symmTensor,symmTensor> laplacianSymmTensorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianSymmTensorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,symmTensor,symmTensor,name,fvcSurfCoeffSymmTensorLaplacianSymmTensor);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<symmTensor,sphericalTensor> laplacianSymmTensorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianSymmTensorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,symmTensor,sphericalTensor,name,fvcSurfCoeffSphericalTensorLaplacianSymmTensor);

    // sphericalTensor

typedef fvcSurfCoeffLaplacianFunctionPlugin<sphericalTensor,scalar> laplacianSphericalTensorScalar;
defineTemplateTypeNameAndDebug(laplacianSphericalTensorScalar,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,sphericalTensor,scalar,name,fvcSurfCoeffScalarLaplacianSphericalTensor);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<sphericalTensor,vector> laplacianSphericalTensorVector;
// defineTemplateTypeNameAndDebug(laplacianSphericalTensorVector,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,sphericalTensor,vector,name,fvcSurfCoeffVectorLaplacianSphericalTensor);

typedef fvcSurfCoeffLaplacianFunctionPlugin<sphericalTensor,tensor> laplacianSphericalTensorTensor;
defineTemplateTypeNameAndDebug(laplacianSphericalTensorTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,sphericalTensor,tensor,name,fvcSurfCoeffTensorLaplacianSphericalTensor);

typedef fvcSurfCoeffLaplacianFunctionPlugin<sphericalTensor,symmTensor> laplacianSphericalTensorSymmTensor;
defineTemplateTypeNameAndDebug(laplacianSphericalTensorSymmTensor,0);
addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,sphericalTensor,symmTensor,name,fvcSurfCoeffSymmTensorLaplacianSphericalTensor);

// typedef fvcSurfCoeffLaplacianFunctionPlugin<sphericalTensor,sphericalTensor> laplacianSphericalTensorSphericalTensor;
// defineTemplateTypeNameAndDebug(laplacianSphericalTensorSphericalTensor,0);
// addNamedTemplate2ToRunTimeSelectionTable(FieldValuePluginFunction, fvcSurfCoeffLaplacianFunctionPlugin,sphericalTensor,sphericalTensor,name,fvcSurfCoeffSphericalTensorLaplacianSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class T,class GT>
fvcSurfCoeffLaplacianFunctionPlugin<T,GT>::fvcSurfCoeffLaplacianFunctionPlugin(
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
void fvcSurfCoeffLaplacianFunctionPlugin<T,GT>::doEvaluation()
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
            const_cast<fv::laplacianScheme<T,GT>&>(scheme()).fvcLaplacian(coeff_(),original_())
        )
    );

    result().setObjectResult(pInterpol);
}

template<class T,class GT>
void fvcSurfCoeffLaplacianFunctionPlugin<T,GT>::setArgument(
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
void fvcSurfCoeffLaplacianFunctionPlugin<T,GT>::setArgument(
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
