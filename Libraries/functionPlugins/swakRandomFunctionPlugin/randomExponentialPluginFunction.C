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

#include "randomExponentialPluginFunction.H"
#include "FieldValueExpressionDriver.H"
#include "FieldValuePluginFunction.H"
#include <Random.H>

#include "addToRunTimeSelectionTable.H"

namespace Foam {

typedef randomExponentialPluginFunction<FieldValuePluginFunction,FieldValueExpressionDriver> randomExponentialPluginFunctionField;
defineTemplateTypeNameAndDebug(randomExponentialPluginFunctionField,0);

addNamedToRunTimeSelectionTable(FieldValuePluginFunction, randomExponentialPluginFunctionField , name, randomExponential);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <typename FType,typename DType>
randomExponentialPluginFunction<FType,DType>::randomExponentialPluginFunction(
    const DType &parentDriver,
    const word &name
):
    FType(
        parentDriver,
        name,
        word("volScalarField"),
        string("seed primitive label,halfLife primitive scalar")
    ),
    halfLife_(1),
    seed_(666)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <typename FType,typename DType>
void randomExponentialPluginFunction<FType,DType>::doEvaluationInternal(
    scalarField &f
) {
    label seed=seed_;

    if(seed<=0) {
        seed=this->mesh().time().timeIndex()-seed;
    }

    Random rand(seed);

    forAll(f,i) {
        f[i]=-log(1-rand.scalar01())*halfLife_;
    }
}

template <typename FType,typename DType>
void randomExponentialPluginFunction<FType,DType>::doEvaluation()
{
    autoPtr<volScalarField> pRandom(
        new volScalarField(
            IOobject(
                "exponentialRandom",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensionedScalar("randomExp",dimless,0),
            "zeroGradient"
        )
    );

    doEvaluationInternal(pRandom->internalField());

    this->result().setObjectResult(pRandom);
}

template <typename FType,typename DType>
void randomExponentialPluginFunction<FType,DType>::setArgument(
    label index,
    const label &val
)
{
    assert(index==0);
    seed_=val;
}

template <typename FType,typename DType>
void randomExponentialPluginFunction<FType,DType>::setArgument(
    label index,
    const scalar &val
)
{
    assert(index==1);
    halfLife_=val;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
