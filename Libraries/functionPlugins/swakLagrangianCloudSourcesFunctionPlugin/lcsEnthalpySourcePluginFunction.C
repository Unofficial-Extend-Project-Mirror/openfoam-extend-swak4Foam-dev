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

#include "lcsEnthalpySourcePluginFunction.H"

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

defineTypeNameAndDebug(lcsEnthalpySourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsEnthalpySourcePluginFunction , name, lcsEnthalpySource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsEnthalpySourcePluginFunction::lcsEnthalpySourcePluginFunction(
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

autoPtr<lcsEnthalpySourcePluginFunction::dimScalarField>
lcsEnthalpySourcePluginFunction::internalEvaluate()
{
    // pick up the first fitting class
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(dimScalarField,basicThermoCloud,thermoCloud,Sh());
    tryCall(dimScalarField,constThermoReactingCloud,reactingCloud,Sh());
    tryCall(dimScalarField,thermoReactingCloud,reactingCloud,Sh());
    tryCall(dimScalarField,icoPoly8ThermoReactingCloud,reactingCloud,Sh());
    tryCall(dimScalarField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,Sh());
    tryCall(dimScalarField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,Sh());
    tryCall(dimScalarField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,Sh());
#else
    tryCall(dimScalarField,swakFluidThermoCloudType,thermoCloud,hsTrans());
    tryCall(dimScalarField,basicReactingCloud,reactingCloud,hsTrans());
    tryCall(dimScalarField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,hsTrans());
#endif

    return autoPtr<dimScalarField>();
}

void lcsEnthalpySourcePluginFunction::doEvaluation()
{
    autoPtr<dimScalarField> pSh=internalEvaluate();

    noCloudFound(pSh);

    const dimScalarField &Sh=pSh();

    autoPtr<volScalarField> pSource(
        new volScalarField(
            IOobject(
                cloudName()+"EnthalpySource",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
#ifdef FOAM_THERMOCLOUD_OLD_STYLE
            Sh.dimensions(),
#else
            Sh.dimensions()/(dimTime*dimVolume),
#endif
            "zeroGradient"
        )
    );

#ifdef FOAM_THERMOCLOUD_OLD_STYLE
    pSource->internalField()=Sh.field();
#else
    pSource->internalField()=Sh.field()/(mesh().V()*mesh().time().deltaT().value());
#endif

    result().setObjectResult(pSource);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
