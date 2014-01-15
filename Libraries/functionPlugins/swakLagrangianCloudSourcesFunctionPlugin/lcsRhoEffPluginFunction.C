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

#include "lcsRhoEffPluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "swakCloudTypes.H"

#include "basicKinematicCloud.H"
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "basicThermoCloud.H"
#include "BasicReactingCloud.H"
#include "BasicReactingMultiphaseCloud.H"
#else
#include "basicReactingCloud.H"
#include "basicReactingMultiphaseCloud.H"
#endif

namespace Foam {

defineTypeNameAndDebug(lcsRhoEffPluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsRhoEffPluginFunction , name, lcsRhoEff);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsRhoEffPluginFunction::lcsRhoEffPluginFunction(
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

autoPtr<volScalarField> lcsRhoEffPluginFunction::internalEvaluate()
{
    // pick up the first fitting class
    tryCall(volScalarField,basicKinematicCloud,kinematicCloud,rhoEff());
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(volScalarField,basicThermoCloud,thermoCloud,rhoEff());
    tryCall(volScalarField,constThermoReactingCloud,reactingCloud,rhoEff());
    tryCall(volScalarField,thermoReactingCloud,reactingCloud,rhoEff());
    tryCall(volScalarField,icoPoly8ThermoReactingCloud,reactingCloud,rhoEff());
    tryCall(volScalarField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,rhoEff());
    tryCall(volScalarField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,rhoEff());
    tryCall(volScalarField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,rhoEff());
#else
    tryCall(volScalarField,swakFluidThermoCloudType,thermoCloud,rhoEff());
    tryCall(volScalarField,basicReactingCloud,reactingCloud,rhoEff());
    tryCall(volScalarField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,rhoEff());
#endif

    return autoPtr<volScalarField>();
}

void lcsRhoEffPluginFunction::doEvaluation()
{

    autoPtr<volScalarField> prhoEff=internalEvaluate();

    noCloudFound(prhoEff);

    result().setObjectResult(prhoEff);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
