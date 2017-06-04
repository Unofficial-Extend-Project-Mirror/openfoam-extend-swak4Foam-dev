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
    2012-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxyForReactingParcel.H"

#include "DebugOStream.H"

#include "swakCloudTypes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForReactingParcelNoComposition<CloudType>::CloudProxyForReactingParcelNoComposition
(
    const cloud& c
)
:
    CloudProxyForThermoParcel<CloudType>(c)
{
    typedef CloudProxyForReactingParcelNoComposition<CloudType> baseType;

    this->addScalarFunction(
        "mass0",
        "Initial mass",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::mass0
        )
    );

    this->addScalarFunction(
        "pc",
        "Owner cell pressure",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::pc
        )
    );

    //- constant Properties
    const typename CloudType::particleType::constantProperties &constProps=
        this->theCloud().constProps();

    this->addScalarFunction(
        "pMin",
        "Minimum pressure (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.pMin()
        )
    );
#ifdef FOAM_PHASECHANGEMODEL_HAS_TVAP

    // TODO: Reimplement. But use a general solution that calls sub-models

#else
    this->addScalarFunction(
        "Tvap",
        "Vaporisation temperature (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.Tvap()
        )
    );
#ifdef FOAM_REACTINGCLOUD_OLD_STYLE
    this->addScalarFunction(
        "Tbp",
        "Boiling point (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.Tbp()
        )
    );
#endif
#endif
}


template<class CloudType>
CloudProxyForReactingParcel<CloudType>::CloudProxyForReactingParcel
(
    const cloud& c
)
:
    CloudProxyForReactingParcelNoComposition<CloudType>(c)
{
    typedef CloudProxyForReactingParcel<CloudType> baseType;

    const wordList& phaseTypes = this->theCloud().composition().phaseTypes();
    forAll(phaseTypes,i) {
        const word &name=phaseTypes[i];
        this->addScalarFunction(
            "Y"+name,
            "Mass fraction of "+name,
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::Y,
                i
            )
        );
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForReactingParcelNoComposition<CloudType>::~CloudProxyForReactingParcelNoComposition()
{}

template<class CloudType>
CloudProxyForReactingParcel<CloudType>::~CloudProxyForReactingParcel()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


} // namespace end

// ************************************************************************* //
