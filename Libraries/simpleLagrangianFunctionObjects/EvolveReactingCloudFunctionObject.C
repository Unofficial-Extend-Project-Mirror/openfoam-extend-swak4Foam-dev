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
    \\  /    A nd           | Copyright  held by original author
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
    2012-2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "EvolveReactingCloudFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    addNamedTemplateToRunTimeSelectionTable
    (
        functionObject,
        EvolveReactingCloudFunctionObject,
        constThermoReactingCloud,
        dictionary,
        evolveConstReactingCloud
    );

    defineTemplateTypeNameAndDebug(EvolveReactingCloudFunctionObject<constThermoReactingCloud>, 0);

    addNamedTemplateToRunTimeSelectionTable
    (
        functionObject,
        EvolveReactingCloudFunctionObject,
        thermoReactingCloud,
        dictionary,
        evolveThermoReactingCloud
    );

    defineTemplateTypeNameAndDebug(EvolveReactingCloudFunctionObject<thermoReactingCloud>, 0);

    addNamedTemplateToRunTimeSelectionTable
    (
        functionObject,
        EvolveReactingCloudFunctionObject,
        icoPoly8ThermoReactingCloud,
        dictionary,
        evolveIcoPoly8ReactingCloud
    );

    defineTemplateTypeNameAndDebug(EvolveReactingCloudFunctionObject<icoPoly8ThermoReactingCloud>, 0);
#else
    defineTypeNameAndDebug(EvolveReactingCloudFunctionObject, 0);

    addNamedToRunTimeSelectionTable
    (
        functionObject,
        EvolveReactingCloudFunctionObject,
        dictionary,
        evolveReactingCloud
    );
#endif

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
template <class CloudType>
EvolveReactingCloudFunctionObject<CloudType>
#else
EvolveReactingCloudFunctionObject
#endif
::EvolveReactingCloudFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    EvolveCloudFunctionObject<
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
    CloudType
#else
    basicReactingCloud
#endif
    >(
        name,
        t,
        dict
    )
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#define TEMPLATE template
template <class CloudType>
bool EvolveReactingCloudFunctionObject<CloudType>
#else
#define TEMPLATE
bool EvolveReactingCloudFunctionObject
#endif
::start()
{
    this->cloud().set(
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
        new CloudType(
#else
        new basicReactingCloud(
#endif
            this->cloudName(),
            this->TEMPLATE getField<volScalarField>("rhoName"),
            this->TEMPLATE getField<volVectorField>("UName"),
            this->g(),
#ifdef FOAM_OLD_THERMOPHYSICS
            const_cast<basicThermo &>(
                this->template getField<basicThermo>("thermoPhysicsName")
            )
#else
            const_cast<SLGThermo &>(
                this->getField<SLGThermo>("SLGThermoPhysicsName")
            )
#endif
        )
    );

    return true;
}


} // namespace Foam

// ************************************************************************* //
