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

#include "coalCloudMassSourcePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "swakCloudTypes.H"

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "CoalCloud.H"
#else
#include "coalCloud.H"
#endif

namespace Foam {

defineTypeNameAndDebug(coalCloudMassSourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,coalCloudMassSourcePluginFunction , name, coalCloudMassSource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

coalCloudMassSourcePluginFunction::coalCloudMassSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    lcsMassSourcePluginFunction(
        parentDriver,
        name
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<lcsMassSourcePluginFunction::dimScalarField>
coalCloudMassSourcePluginFunction::internalEvaluate()
{
    // pick up the first fitting class
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(dimScalarField,constThermoCoalCloud,reactingMultiphaseCloud,Srho());
    tryCall(dimScalarField,thermoCoalCloud,reactingMultiphaseCloud,Srho());
    tryCall(dimScalarField,icoPoly8ThermoCoalCloud,reactingMultiphaseCloud,Srho());
#else
    tryCall(dimScalarField,coalCloud,reactingMultiphaseCloud,Srho());
#endif

    return lcsMassSourcePluginFunction::internalEvaluate();
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
