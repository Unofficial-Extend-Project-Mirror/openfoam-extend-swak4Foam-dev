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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "SubsetValueExpressionDriver.H"

#include "Random.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(SubsetValueExpressionDriver, 0);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


SubsetValueExpressionDriver::SubsetValueExpressionDriver(const SubsetValueExpressionDriver& orig)
:
    CommonValueExpressionDriver(orig),
    autoInterpolate_(orig.autoInterpolate_),
    warnAutoInterpolate_(orig.warnAutoInterpolate_)
{}

SubsetValueExpressionDriver::SubsetValueExpressionDriver(const dictionary& dict)
:
    CommonValueExpressionDriver(dict),
    autoInterpolate_(dict.lookupOrDefault("autoInterpolate",false)),
    warnAutoInterpolate_(dict.lookupOrDefault("warnAutoInterpolate",true))
{}

SubsetValueExpressionDriver::SubsetValueExpressionDriver(
        bool autoInterpolate,
        bool warnAutoInterpolate
)
:
    CommonValueExpressionDriver(),
    autoInterpolate_(autoInterpolate),
    warnAutoInterpolate_(warnAutoInterpolate)
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SubsetValueExpressionDriver::~SubsetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void SubsetValueExpressionDriver::parse (const std::string& f)
{
    content_ = f;
    scan_begin ();
    parserSubset::SubsetValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing_);
    parser.parse ();
    scan_end ();
}

scalarField *SubsetValueExpressionDriver::makeIdField()
{
    scalarField *ids=new scalarField(this->size());
    forAll(*ids,i) {
        (*ids)[i]=i;
    }
    return ids;
}

vectorField *SubsetValueExpressionDriver::makePositionField()
{
    notImplemented("SubsetValueExpressionDriver::makePositionField");

    return new vectorField(0);
}

// vectorField *SubsetValueExpressionDriver::makePointField()
// {
//     notImplemented("SubsetValueExpressionDriver::makePointField");
// }

vectorField *SubsetValueExpressionDriver::makeFaceNormalField()
{
    notImplemented("SubsetValueExpressionDriver::makeFaceNormalField");

    return new vectorField(0);
}

vectorField *SubsetValueExpressionDriver::makeFaceAreaField()
{
    notImplemented("SubsetValueExpressionDriver::makeFaceAreaField");

    return new vectorField(0);
}

// ************************************************************************* //

} // namespace
