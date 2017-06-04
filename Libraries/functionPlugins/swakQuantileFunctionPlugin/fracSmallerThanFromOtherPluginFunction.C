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

#include "fracSmallerThanFromOtherPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "SimpleDistribution.H"

namespace Foam {

defineTypeNameAndDebug(fracSmallerThanFromOtherPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, fracSmallerThanFromOtherPluginFunction , name, fracSmallerThanFromOther);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fracSmallerThanFromOtherPluginFunction::fracSmallerThanFromOtherPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        string("other internalField volScalarField,weight internalField volScalarField,mask internalField volLogicalField,binNumber primitive label,field internalField volScalarField")
    ),
    binNumber_(1000)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void fracSmallerThanFromOtherPluginFunction::setArgument(
    label index,
    const label &value
) {
    assert(index==3);
    if(value<1) {
        FatalErrorIn("fracSmallerThanFromOtherPluginFunction::setArgument")
            << "Bin number is " << value << nl
                << "Should be bigger than 0 in "
                << helpText()
                << endl
                << exit(FatalError);
    }
    binNumber_=value;
}


void fracSmallerThanFromOtherPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0 || index==1 || index==2 || index==4);

    if(index==4) {
        field_.set(
            new volScalarField(
                //            dynamicCast<const FieldValueExpressionDriver &>(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<volScalarField>()
            )
        );
    } else if(index==0) {
        other_.set(
            new volScalarField(
                //            dynamicCast<const FieldValueExpressionDriver &>(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<volScalarField>()
            )
        );
    } else if(index==1) {
        weight_.set(
            new volScalarField(
                //            dynamicCast<const FieldValueExpressionDriver &>(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<volScalarField>()
            )
        );
    } else {
        mask_.set(
            new volScalarField(
                //            dynamicCast<const FieldValueExpressionDriver &>(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<volScalarField>()
            )
        );
    }
}

void fracSmallerThanFromOtherPluginFunction::doEvaluation()
{
    const scalarField &field=field_->internalField();
    const scalarField &other=other_->internalField();
    SimpleDistribution<scalar> dist(
        gMin(other),
        gMax(other),
        binNumber_
    );

    Field<bool> mask(mask_->internalField().size(),true);
    forAll(mask_->internalField(),cellI){
        if(
            mask_->internalField()[cellI]>0.5
            ||
            mask_->internalField()[cellI]<-0.5
        ) {
            mask[cellI]=true;
        } else {
            mask[cellI]=false;
        }
    }
    dist.calcScalarWeight(
        other,
        weight_->internalField(),
        mask
    );

    autoPtr<volScalarField> pSmaller(
        new volScalarField(
            IOobject(
                "fracSmallerThanFromOtherField",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("nonOr",dimless,0),
            "zeroGradient"
        )
    );

    volScalarField &smaller=pSmaller();

    forAll(smaller,cellI) {
        if(field[cellI]<dist.min()) {
            smaller[cellI]=-1;
        } else if(field[cellI]>dist.max()) {
            smaller[cellI]=2;
        } else {
            smaller[cellI]=dist.smaller(field[cellI]);
        }
    }

    result().setObjectResult(pSmaller);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
