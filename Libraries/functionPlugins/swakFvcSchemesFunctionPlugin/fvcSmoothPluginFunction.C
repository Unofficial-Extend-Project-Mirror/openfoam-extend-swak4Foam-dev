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
    2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"

#ifdef  FOAM_FV_HAS_SMOOTH_SWEEP_SPREAD

#include "fvcSmoothPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "fvcSmooth.H"

namespace Foam {

defineTypeNameAndDebug(fvcSmoothPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, fvcSmoothPluginFunction , name, fvcSmooth);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fvcSmoothPluginFunction::fvcSmoothPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("volScalarField"),
        string("originalField internalField volScalarField"
        ",coffRelative primitive scalar")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void fvcSmoothPluginFunction::doEvaluation()
{
    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "smoothed",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensionedScalar("smoothed",dimless,0),
            "zeroGradient"
        )
    );
    volScalarField &result=pResult();

    result==field_();

    fvc::smooth(
        result,
        coeff_
    );

    this->result().setObjectResult(pResult);
}

void fvcSmoothPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0);

    this->field_.set(
        new volScalarField(
            //                dynamicCast<const FieldValueExpressionDriver &>(
            dynamic_cast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<volScalarField>()
        )
    );
}

void fvcSmoothPluginFunction::setArgument(
    label index,
    const scalar &val
)
{
    assert(index==1);
    coeff_=val;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

#endif

// ************************************************************************* //
