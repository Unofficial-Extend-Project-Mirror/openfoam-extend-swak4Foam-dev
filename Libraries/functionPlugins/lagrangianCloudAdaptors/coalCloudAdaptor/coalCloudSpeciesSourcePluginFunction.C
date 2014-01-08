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

#include "coalCloudSpeciesSourcePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "swakCloudTypes.H"

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "CoalCloud.H"
#else
#include "coalCloud.H"
#endif

namespace Foam {

defineTypeNameAndDebug(coalCloudSpeciesSourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,coalCloudSpeciesSourcePluginFunction , name, coalCloudSpeciesSource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

coalCloudSpeciesSourcePluginFunction::coalCloudSpeciesSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    lcsSpeciesSourcePluginFunction(
        parentDriver,
        name
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

label coalCloudSpeciesSourcePluginFunction::getIndex(wordList &speciesList)
{
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    getSpeciesIndex(constThermoCoalCloud,reactingMultiphaseCloud);
    getSpeciesIndex(thermoCoalCloud,reactingMultiphaseCloud);
    getSpeciesIndex(icoPoly8ThermoCoalCloud,reactingMultiphaseCloud);
#else
    getSpeciesIndex(coalCloud,reactingMultiphaseCloud);
#endif

    return lcsSpeciesSourcePluginFunction::getIndex(speciesList);
}

autoPtr<lcsSpeciesSourcePluginFunction::dimScalarField>
coalCloudSpeciesSourcePluginFunction::internalEvaluate(const label index)
{
    // pick up the first fitting class
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    tryCall(dimScalarField,constThermoCoalCloud,reactingMultiphaseCloud,Srho(index));
    tryCall(dimScalarField,thermoCoalCloud,reactingMultiphaseCloud,Srho(index));
    tryCall(dimScalarField,icoPoly8ThermoCoalCloud,reactingMultiphaseCloud,Srho(index));
#else
    tryCall(dimScalarField,coalCloud,reactingMultiphaseCloud,Srho(index));
#endif

    return lcsSpeciesSourcePluginFunction::internalEvaluate(index);
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
