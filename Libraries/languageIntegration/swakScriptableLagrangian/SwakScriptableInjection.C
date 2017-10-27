/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
*-----------------------------------------------------------------------------*
License
    This file is part of swak4Foam.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

\*---------------------------------------------------------------------------*/

#include "SwakScriptableInjection.H"
#include "mathematicalConstants.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class CloudType>
Foam::label Foam::SwakScriptableInjection<CloudType>::parcelsToInject
(
    const scalar time0,
    const scalar time1
) const
{
    return 0;
}


template<class CloudType>
Foam::scalar Foam::SwakScriptableInjection<CloudType>::volumeToInject
(
    const scalar time0,
    const scalar time1
) const
{
    return 0.0;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::SwakScriptableInjection<CloudType>::SwakScriptableInjection
(
    const dictionary& dict,
    CloudType& owner
)
:
    InjectionModel<CloudType>(dict, owner, typeName)
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::SwakScriptableInjection<CloudType>::~SwakScriptableInjection()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::SwakScriptableInjection<CloudType>::active() const
{
    return true;
}


template<class CloudType>
Foam::scalar Foam::SwakScriptableInjection<CloudType>::timeEnd() const
{
    return this->SOI_;
}


template<class CloudType>
void Foam::SwakScriptableInjection<CloudType>::setPositionAndCell
(
    const label parcelI,
    const label,
    const scalar,
    vector& position,
    label& cellOwner
)
{
    //    position = positions_[parcelI];
    this->findCellAtPosition(cellOwner, position);
}


template<class CloudType>
void Foam::SwakScriptableInjection<CloudType>::setProperties
(
    const label parcelI,
    const label,
    const scalar,
    typename CloudType::parcelType& parcel
)
{
    // set particle velocity
    //    parcel.U() = U0_;

    // set particle diameter
    //    parcel.d() = diameters_[parcelI];
}


template<class CloudType>
bool Foam::SwakScriptableInjection<CloudType>::fullyDescribed() const
{
    return false;
}


template<class CloudType>
bool Foam::SwakScriptableInjection<CloudType>::validInjection(const label)
{
    return true;
}


// ************************************************************************* //
