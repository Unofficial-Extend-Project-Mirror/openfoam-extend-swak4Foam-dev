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
    2012-2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxyForKinematicParcel.H"

#include "DebugOStream.H"

#include "swakCloudTypes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForKinematicParcel<CloudType>::CloudProxyForKinematicParcel
(
    const cloud& c
)
:
    CloudProxyForParticle<CloudType>(c)
{
    typedef CloudProxyForKinematicParcel<CloudType> baseType;

#ifndef FOAM_KINEMTATIC_HAS_NO_ACTIVE_PROPERTY
    this->addBoolFunction(
        "active",
        "Is this parcel active?",
        new typename baseType::template ParticleMethodWrapperValue<bool>(
            &CloudType::particleType::active
        )
    );
#endif
    this->addScalarFunction(
        "typeId",
        "Type ID",
        new typename baseType::template ParticleMethodWrapperValue<scalar,label>(
            &CloudType::particleType::typeId
        )
    );
    this->addScalarFunction(
        "nParticle",
        "Number of particles",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::nParticle
        )
    );
    this->addScalarFunction(
        "d",
        "Diameter",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::d
        )
    );
#ifdef FOAM_KINEMATIC_PARCEL_HAS_DTARGET
    this->addScalarFunction(
        "dTarget",
        "Target diameter",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::dTarget
        )
    );
#endif
    this->addVectorFunction(
        "U",
        "Velocity",
        new typename baseType::template ParticleMethodWrapperReference<vector>(
            &CloudType::particleType::U
        )
    );
    this->addScalarFunction(
        "rho",
        "Density",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::rho
        )
    );
#ifdef FOAM_KINEMATIC_PARCEL_HAS_AGE
    this->addScalarFunction(
        "age",
        "Age of the prticle",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::age
        )
    );
#endif
    this->addScalarFunction(
        "tTurb",
        "Time in turbulent eddy",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::tTurb
        )
    );
    this->addVectorFunction(
        "UTurb",
        "Turbulent velocity fluctuations",
        new typename baseType::template ParticleMethodWrapperReference<vector>(
            &CloudType::particleType::UTurb
        )
    );

    // helper functions
    this->addScalarFunction(
        "mass",
        "Particle mass",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::mass
        )
    );
    this->addScalarFunction(
        "volume",
        "Particle volume",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::volume
        )
    );
    this->addScalarFunction(
        "areaP",
        "Particle projected area",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::areaP
        )
    );
    this->addScalarFunction(
        "areaS",
        "Particle surface area",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::areaS
        )
    );

    //- constant Properties
    const typename CloudType::particleType::constantProperties &constProps=
        this->theCloud().constProps();

    this->addScalarFunction(
        "rhoMin",
        "Minimum density (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.rhoMin()
        )
    );
    this->addScalarFunction(
        "rho0",
        "Particle density (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.rho0()
        )
    );

#ifdef FOAM_KINEMATIC_PARCEL_HAS_MINPARCELMASS
    this->addScalarFunction(
        "minParcelMass",
        "Minimum parcel mass (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.minParcelMass()
        )
    );
#else
    this->addScalarFunction(
        "minParticleMass",
        "Minimum particle mass (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.minParticleMass()
        )
    );
#endif
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForKinematicParcel<CloudType>::~CloudProxyForKinematicParcel()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
tmp<Field<scalar> > CloudProxyForKinematicParcel<CloudType>::weights() const
{
    tmp<Field<scalar> > tWeight(
        new Field<scalar>(this->theCloud().size())
    );
    Field<scalar> &weight=const_cast<Field<scalar>&>(tWeight());
    label i=0;
    forAllConstIter(typename CloudType,this->theCloud(),it)
    {
	const typename CloudProxyForKinematicParcel<CloudType>::particleType &p=(*it);
        weight[i]=p.nParticle()*p.mass();
        i++;
    }

    return tWeight;
}


} // namespace end

// ************************************************************************* //
