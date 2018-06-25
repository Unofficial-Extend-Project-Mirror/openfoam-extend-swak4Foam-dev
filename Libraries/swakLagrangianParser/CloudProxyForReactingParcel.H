/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::CloudProxyForReactingParcel

Description

SourceFiles
    CloudProxyForReactingParcel.C

Contributors/Copyright:
    2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudProxyForReactingParcel_H
#define CloudProxyForReactingParcel_H

#include "CloudProxyForThermoParcel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class CloudProxyForReactingParcel Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class CloudProxyForReactingParcelNoComposition
:
    public CloudProxyForThermoParcel<CloudType>
{
    //- Disallow default bitwise assignment
    void operator=(const CloudProxyForReactingParcelNoComposition&);

    //- Disallow copy construct
    CloudProxyForReactingParcelNoComposition(const CloudProxyForReactingParcelNoComposition &);

public:

    // Constructors

    //- Construct from components
    CloudProxyForReactingParcelNoComposition
    (
        const cloud &
    );

    // Destructor

    virtual ~CloudProxyForReactingParcelNoComposition();

};

template<class CloudType>
class CloudProxyForReactingParcel
:
    public CloudProxyForReactingParcelNoComposition<CloudType>
{
    //- Disallow default bitwise assignment
    void operator=(const CloudProxyForReactingParcel&);

    //- Disallow copy construct
    CloudProxyForReactingParcel(const CloudProxyForReactingParcel &);

public:

    // Constructors

    //- Construct from components
    CloudProxyForReactingParcel
    (
        const cloud &
    );

    // Destructor

    virtual ~CloudProxyForReactingParcel();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "CloudProxyForReactingParcel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
