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

#include "lcsMomentumSourcePluginFunction.H"

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

defineTypeNameAndDebug(lcsMomentumSourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsMomentumSourcePluginFunction , name, lcsMomentumSource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsMomentumSourcePluginFunction::lcsMomentumSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    LagrangianCloudSourcePluginFunction(
        parentDriver,
        name,
        "volVectorField"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<lcsMomentumSourcePluginFunction::dimVectorField>
lcsMomentumSourcePluginFunction::internalEvaluate()
{
    // pick up the first fitting class
#ifdef FOAM_KINEMATICCLOUD_OLD_STYLE
    tryCall(dimVectorField,basicKinematicCloud,kinematicCloud,SU());
    tryCall(dimVectorField,basicThermoCloud,thermoCloud,SU());
    tryCall(dimVectorField,constThermoReactingCloud,reactingCloud,SU());
    tryCall(dimVectorField,thermoReactingCloud,reactingCloud,SU());
    tryCall(dimVectorField,icoPoly8ThermoReactingCloud,reactingCloud,SU());
    tryCall(dimVectorField,constThermoReactingMultiphaseCloud,reactingMultiphaseCloud,SU());
    tryCall(dimVectorField,thermoReactingMultiphaseCloud,reactingMultiphaseCloud,SU());
    tryCall(dimVectorField,icoPoly8ThermoReactingMultiphaseCloud,reactingMultiphaseCloud,SU());
#else
    tryCall(dimVectorField,basicKinematicCloud,kinematicCloud,UTrans());
    tryCall(dimVectorField,swakFluidThermoCloudType,thermoCloud,UTrans());
    tryCall(dimVectorField,basicReactingCloud,reactingCloud,UTrans());
    tryCall(dimVectorField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,UTrans());
#endif

    return autoPtr<dimVectorField>();
}

void lcsMomentumSourcePluginFunction::doEvaluation()
{
    autoPtr<dimVectorField> pSU=internalEvaluate();

    noCloudFound(pSU);

    const dimVectorField &SU=pSU();

    autoPtr<volVectorField> pSource(
        new volVectorField(
            IOobject(
                cloudName()+"MomentumSource",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
#ifdef FOAM_KINEMATICCLOUD_OLD_STYLE
            SU.dimensions(),
#else
            SU.dimensions()/(dimTime*dimVolume),
#endif
            "zeroGradient"
        )
    );

#ifdef FOAM_KINEMATICCLOUD_OLD_STYLE
    pSource->internalField()=SU.field();
#else
#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
    const_cast<vectorField&>(pSource->internalField().field())
#else
    pSource->internalField()
#endif
        =SU.field()/(mesh().time().deltaT().value()*mesh().V());
#endif

    result().setObjectResult(pSource);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
