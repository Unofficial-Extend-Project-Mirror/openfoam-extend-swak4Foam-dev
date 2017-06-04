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

Class
    Foam::CloudProxyForReactingMultiphaseParcel

Description

SourceFiles
    CloudProxyForReactingMultiphaseParcel.C

Contributors/Copyright:
    2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudProxyForReactingMultiphaseParcel_H
#define CloudProxyForReactingMultiphaseParcel_H

#include "CloudProxyForReactingParcel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class CloudProxyForReactingMultiphaseParcel Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class CloudProxyForReactingMultiphaseParcelNoComposition
:
    public CloudProxyForReactingParcelNoComposition<CloudType>
{
    //- Disallow default bitwise assignment
    void operator=(const CloudProxyForReactingMultiphaseParcelNoComposition&);

    //- Disallow copy construct
    CloudProxyForReactingMultiphaseParcelNoComposition(const CloudProxyForReactingMultiphaseParcelNoComposition &);

public:

    // Constructors

    //- Construct from components
    CloudProxyForReactingMultiphaseParcelNoComposition
    (
        const cloud &
    );

    // Destructor

    virtual ~CloudProxyForReactingMultiphaseParcelNoComposition();

};

template<class CloudType>
class CloudProxyForReactingMultiphaseParcel
:
    public CloudProxyForReactingMultiphaseParcelNoComposition<CloudType>
{
    //- Disallow default bitwise assignment
    void operator=(const CloudProxyForReactingMultiphaseParcel&);

    //- Disallow copy construct
    CloudProxyForReactingMultiphaseParcel(const CloudProxyForReactingMultiphaseParcel &);

public:

    // Constructors

    //- Construct from components
    CloudProxyForReactingMultiphaseParcel
    (
        const cloud &
    );

    // Destructor

    virtual ~CloudProxyForReactingMultiphaseParcel();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "CloudProxyForReactingMultiphaseParcel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
