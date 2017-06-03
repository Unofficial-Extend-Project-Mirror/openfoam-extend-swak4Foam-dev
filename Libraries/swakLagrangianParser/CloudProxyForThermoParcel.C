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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxyForThermoParcel.H"

#include "DebugOStream.H"

#include "swakCloudTypes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForThermoParcel<CloudType>::CloudProxyForThermoParcel
(
    const cloud& c
)
:
    CloudProxyForKinematicParcel<CloudType>(c)
{
    typedef CloudProxyForThermoParcel<CloudType> baseType;

    this->addScalarFunction(
        "T",
        "Temperature",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::T
        )
    );
    this->addScalarFunction(
        "cp",
        "Specific heat capacity",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
#ifdef FOAM_THERMOCLOUD_OLD_STYLE
            &CloudType::particleType::cp
#else
            &CloudType::parcelType::Cp
#endif
        )
    );

    //- constant Properties
    const typename CloudType::particleType::constantProperties &constProps=
        this->theCloud().constProps();

    this->addScalarFunction(
        "T0",
        "Initial temperature (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.T0()
        )
    );
    this->addScalarFunction(
        "TMin",
        "Minimum temperature (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.TMin()
        )
    );
    this->addScalarFunction(
        "cp0",
        "Specific heat capacity (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
#ifdef FOAM_THERMOCLOUD_OLD_STYLE
            constProps.cp0()
#else
            constProps.Cp0()
#endif
        )
    );
    this->addScalarFunction(
        "epsilon0",
        "Particle emissivity (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.epsilon0()
        )
    );
    this->addScalarFunction(
        "f0",
        "Particle scattering factor (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.f0()
        )
    );
#ifdef FOAM_THERMOCLOUD_OLD_STYLE
    this->addScalarFunction(
        "Pr",
        "Default carrier Prandtl number (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.Pr()
        )
    );
#endif

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForThermoParcel<CloudType>::~CloudProxyForThermoParcel()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace end

// ************************************************************************* //
