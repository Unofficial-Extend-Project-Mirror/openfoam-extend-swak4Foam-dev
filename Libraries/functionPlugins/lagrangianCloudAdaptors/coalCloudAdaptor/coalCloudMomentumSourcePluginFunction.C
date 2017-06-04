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

#include "coalCloudMomentumSourcePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "swakCloudTypes.H"

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "CoalCloud.H"
#else
#include "coalCloud.H"
#endif

namespace Foam {

defineTypeNameAndDebug(coalCloudMomentumSourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,coalCloudMomentumSourcePluginFunction , name, coalCloudMomentumSource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

coalCloudMomentumSourcePluginFunction::coalCloudMomentumSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    lcsMomentumSourcePluginFunction(
        parentDriver,
        name
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<lcsMomentumSourcePluginFunction::dimVectorField>
coalCloudMomentumSourcePluginFunction::internalEvaluate()
{
    // pick up the first fitting class
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(dimVectorField,constThermoCoalCloud,reactingMultiphaseCloud,SU());
    tryCall(dimVectorField,thermoCoalCloud,reactingMultiphaseCloud,SU());
    tryCall(dimVectorField,icoPoly8ThermoCoalCloud,reactingMultiphaseCloud,SU());
#else
    tryCall(dimVectorField,coalCloud,reactingMultiphaseCloud,UTrans());
#endif

    return lcsMomentumSourcePluginFunction::internalEvaluate();
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
