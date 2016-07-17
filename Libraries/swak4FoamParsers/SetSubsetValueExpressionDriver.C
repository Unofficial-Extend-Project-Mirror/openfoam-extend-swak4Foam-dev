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
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "SetSubsetValueExpressionDriver.H"

#include "Random.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(SetSubsetValueExpressionDriver, 0);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


SetSubsetValueExpressionDriver::SetSubsetValueExpressionDriver(const SetSubsetValueExpressionDriver& orig)
:
    SubsetValueExpressionDriver(orig),
    id_(orig.id_),
    origin_(orig.origin_)
{}

SetSubsetValueExpressionDriver::SetSubsetValueExpressionDriver(
    const dictionary& dict,
    const word &id,
    const SetOrigin origin
)
:
    SubsetValueExpressionDriver(dict),
    id_(id),
    origin_(origin)
{}

SetSubsetValueExpressionDriver::SetSubsetValueExpressionDriver(
        const word &id,
        const SetOrigin origin,
        bool autoInterpolate,
        bool warnAutoInterpolate
)
:
    SubsetValueExpressionDriver(autoInterpolate,warnAutoInterpolate),
    id_(id),
    origin_(origin)
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SetSubsetValueExpressionDriver::~SetSubsetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


// ************************************************************************* //

} // namespace
