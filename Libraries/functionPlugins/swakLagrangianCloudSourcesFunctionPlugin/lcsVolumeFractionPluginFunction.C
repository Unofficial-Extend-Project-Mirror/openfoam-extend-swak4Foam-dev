/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "lcsVolumeFractionPluginFunction.H"

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

defineTypeNameAndDebug(lcsVolumeFractionPluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsVolumeFractionPluginFunction , name, lcsVolumeFraction);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsVolumeFractionPluginFunction::lcsVolumeFractionPluginFunction(
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

autoPtr<volScalarField> lcsVolumeFractionPluginFunction::internalEvaluate()
{
    // pick up the first fitting class
    tryCall(volScalarField,basicKinematicCloud,kinematicCloud,theta());
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(volScalarField,basicThermoCloud,thermoCloud,theta());
    tryCall(volScalarField,constThermoReactingCloud,reactingCloud,theta());
    tryCall(volScalarField,thermoReactingCloud,reactingCloud,theta());
    tryCall(volScalarField,icoPoly8ThermoReactingCloud,reactingCloud,theta());
    tryCall(volScalarField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,theta());
    tryCall(volScalarField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,theta());
    tryCall(volScalarField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,theta());
#else
    tryCall(volScalarField,swakFluidThermoCloudType,thermoCloud,theta());
    tryCall(volScalarField,basicReactingCloud,reactingCloud,theta());
    tryCall(volScalarField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,theta());
#endif

    return autoPtr<volScalarField>();
}

void lcsVolumeFractionPluginFunction::doEvaluation()
{
    autoPtr<volScalarField> ptheta=internalEvaluate();

    noCloudFound(ptheta);

    result().setObjectResult(ptheta);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
