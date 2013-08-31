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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxyForReactingParcel.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForReactingParcel<CloudType>::CloudProxyForReactingParcel
(
    const cloud& c
)
:
    CloudProxyForParticle<CloudType>(c)
{
    typedef CloudProxyForParticle<CloudType> baseType;

    this->addScalarFunction(
        "mass0",
        "Initial mass",
        new typename baseType::template ParticleMethodWrapperValue<scalar>(
            &CloudType::particleType::mass0
        )
    );

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
CloudProxyForReactingParcel<CloudType>::~CloudProxyForReactingParcel()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
tmp<Field<scalar> > CloudProxyForReactingParcel<CloudType>::getScalarField(
    const word &name
) const
{
    return CloudProxyForParticle<CloudType>::getScalarField(name);
}

template<class CloudType>
tmp<Field<scalar> > CloudProxyForReactingParcel<CloudType>::weights() const
{
    tmp<Field<scalar> > tWeight(
        new Field<scalar>(this->theCloud().size())
    );
    Field<scalar> &weight=tWeight();
    label i=0;
    forAllConstIter(typename CloudType,this->theCloud(),it)
    {
	const typename CloudProxyForReactingParcel<CloudType>::particleType &p=(*it);
        weight[i]=p.nParticle()*p.mass();
        i++;
    }

    return tWeight;
}


} // namespace end

// ************************************************************************* //
