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

#include "fracSmallerThanPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "SimpleDistribution.H"

namespace Foam {

defineTypeNameAndDebug(fracSmallerThanPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, fracSmallerThanPluginFunction , name, fracSmallerThan);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fracSmallerThanPluginFunction::fracSmallerThanPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        string("field internalField volScalarField,weight internalField volScalarField,mask internalField volLogicalField,binNumber primitive label")
    ),
    binNumber_(1000)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void fracSmallerThanPluginFunction::setArgument(
    label index,
    const label &value
) {
    assert(index==3);
    if(value<1) {
        FatalErrorIn("fracSmallerThanPluginFunction::setArgument")
            << "Bin number is " << value << nl
                << "Should be bigger than 0 in "
                << helpText()
                << endl
                << exit(FatalError);
    }
    binNumber_=value;
}


void fracSmallerThanPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0 || index==1 || index==2);

    if(index==0) {
        field_.set(
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

void fracSmallerThanPluginFunction::doEvaluation()
{
    const scalarField &field=field_->internalField();
    SimpleDistribution<scalar> dist(
        gMin(field),
        gMax(field),
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
        field,
        weight_->internalField(),
        mask
    );

    autoPtr<volScalarField> pSmaller(
        new volScalarField(
            IOobject(
                "fracSmallerThanField",
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
        if(mask[cellI]) {
            smaller[cellI]=dist.smaller(field[cellI]);
        } else {
            smaller[cellI]=-1;
        }
    }

    result().setObjectResult(pSmaller);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
