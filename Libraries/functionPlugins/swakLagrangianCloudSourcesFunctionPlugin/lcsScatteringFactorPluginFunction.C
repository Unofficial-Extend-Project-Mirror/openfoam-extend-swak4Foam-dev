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

#include "lcsScatteringFactorPluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "swakCloudTypes.H"

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "basicThermoCloud.H"
#include "BasicReactingCloud.H"
#include "BasicReactingMultiphaseCloud.H"
#else
#include "basicReactingCloud.H"
#include "basicReactingMultiphaseCloud.H"
#endif

namespace Foam {

defineTypeNameAndDebug(lcsScatteringFactorPluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsScatteringFactorPluginFunction , name, lcsScatteringFactor);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsScatteringFactorPluginFunction::lcsScatteringFactorPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    LagrangianCloudSourcePluginFunction(
        parentDriver,
        name,
        "volScalarField"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<volScalarField> lcsScatteringFactorPluginFunction::internalEvaluate()
{
    // pick up the first fitting class
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(volScalarField,basicThermoCloud,thermoCloud,sigmap());
    tryCall(volScalarField,constThermoReactingCloud,reactingCloud,sigmap());
    tryCall(volScalarField,thermoReactingCloud,reactingCloud,sigmap());
    tryCall(volScalarField,icoPoly8ThermoReactingCloud,reactingCloud,sigmap());
    tryCall(volScalarField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());
    tryCall(volScalarField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());
    tryCall(volScalarField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());
#else
    tryCall(volScalarField,swakFluidThermoCloudType,thermoCloud,sigmap());
    tryCall(volScalarField,basicReactingCloud,reactingCloud,sigmap());
    tryCall(volScalarField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,sigmap());
#endif

    return autoPtr<volScalarField>();
}

void lcsScatteringFactorPluginFunction::doEvaluation()
{

   autoPtr<volScalarField> psigmap=internalEvaluate();

    noCloudFound(psigmap);

    result().setObjectResult(psigmap);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
