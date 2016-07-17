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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxyForReactingMultiphaseParcel.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForReactingMultiphaseParcel<CloudType>::CloudProxyForReactingMultiphaseParcel
(
    const cloud& cl
)
:
    CloudProxyForReactingParcel<CloudType>(cl)
{
    typedef CloudProxyForReactingMultiphaseParcel<CloudType> baseType;

    const CloudType &c=this->theCloud();

    const label idGas = c.composition().idGas();
    const wordList& gasNames = c.composition().componentNames(idGas);
    forAll(gasNames,i) {
        const word &name=gasNames[i];
        this->addScalarFunction(
            "Y"+name+"(g)",
            "Mass fraction of "+name+" (gas)",
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::YGas,
                i
            )
        );
        this->addScalarFunction(
            "YGas_"+name,
            "Mass fraction of "+name+" (gas) - alias",
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::YGas,
                i
            )
        );
    }

    const label idLiquid = c.composition().idLiquid();
    const wordList& liquidNames = c.composition().componentNames(idLiquid);
    forAll(liquidNames,i) {
        const word &name=liquidNames[i];
        this->addScalarFunction(
            "Y"+name+"(l)",
            "Mass fraction of "+name+" (liquid)",
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::YLiquid,
                i
            )
        );
        this->addScalarFunction(
            "YLiquid_"+name,
            "Mass fraction of "+name+" (liquid) - alias",
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::YLiquid,
                i
            )
        );
    }

    const label idSolid = c.composition().idSolid();
    const wordList& solidNames = c.composition().componentNames(idSolid);
    forAll(solidNames,i) {
        const word &name=solidNames[i];
        this->addScalarFunction(
            "Y"+name+"(s)",
            "Mass fraction of "+name+" (solid)",
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::YSolid,
                i
            )
        );
        this->addScalarFunction(
            "YSolid_"+name,
            "Mass fraction of "+name+" (solid) - alias",
            new typename baseType::template ParticleMethodWrapperFieldElement<scalar>(
                &CloudType::particleType::YSolid,
                i
            )
        );
    }

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
        "LDevol",
        "Latent heat of devolatilisation (constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.LDevol()
        )
    );
    this->addScalarFunction(
        "hRetentionCoeff",
        "Fraction of enthalpy retained due to surface reactions(constant)",
        new typename baseType::template ParticleMethodWrapperConstant<scalar>(
            constProps.hRetentionCoeff()
        )
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForReactingMultiphaseParcel<CloudType>::~CloudProxyForReactingMultiphaseParcel()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


} // namespace end

// ************************************************************************* //
