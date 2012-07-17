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

#include "fvcInterpolationFunctionPlugin.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "surfaceInterpolationScheme.H"

namespace Foam {

defineTypeNameAndDebug(fvcInterpolationFunctionPlugin,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, fvcInterpolationFunctionPlugin , name, fvcInterpolation);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fvcInterpolationFunctionPlugin::fvcInterpolationFunctionPlugin(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("surfaceScalarField"),
        string("original internalField volScalarField;specString primitive string")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void fvcInterpolationFunctionPlugin::doEvaluation()
{
    IStringStream spec(specString_);

    tmp<surfaceInterpolationScheme<scalar> > scheme(
        surfaceInterpolationScheme<scalar>::New(
            mesh(),
            spec
        )
    );

    autoPtr<surfaceScalarField> pInterpol(
        new surfaceScalarField(
            IOobject(
                "fvcInterpolated",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            scheme().interpolate(original_())
        )
    );

    result().setObjectResult(pInterpol);
}

void fvcInterpolationFunctionPlugin::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
)
{
    assert(index==0);
    original_.set(
        new volScalarField(
            dynamicCast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<volScalarField>()
        )
    );
}

void fvcInterpolationFunctionPlugin::setArgument(
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
