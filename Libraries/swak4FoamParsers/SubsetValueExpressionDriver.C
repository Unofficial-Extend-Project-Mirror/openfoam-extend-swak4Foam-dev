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

#include "FieldValueExpressionDriver.H"

#include "PatchValueExpressionDriver.H"

#include "Random.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


SubsetValueExpressionDriver::SubsetValueExpressionDriver(const SubsetValueExpressionDriver& orig)
:
    CommonValueExpressionDriver(orig),
    patch_(orig.patch_)
{}

SubsetValueExpressionDriver::SubsetValueExpressionDriver(const fvPatch& patch)
:
    CommonValueExpressionDriver(),
    patch_(patch)
{}

SubsetValueExpressionDriver::SubsetValueExpressionDriver(const fvPatch& patch,const SubsetValueExpressionDriver& old)
:
    CommonValueExpressionDriver(old),
    patch_(patch)
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

vectorField *SubsetValueExpressionDriver::makePositionField()
{
    return new vectorField(patch_.Cf());
}

vectorField *SubsetValueExpressionDriver::makePointField()
{
    return new vectorField(patch_.patch().localPoints());
}

vectorField *SubsetValueExpressionDriver::makeFaceNormalField()
{
    return new vectorField(patch_.nf());
}

vectorField *SubsetValueExpressionDriver::makeFaceAreaField()
{
    return new vectorField(patch_.Sf());
}

vectorField *SubsetValueExpressionDriver::makeCellNeighbourField()
{
    return new vectorField(patch_.Cn());
}

vectorField *SubsetValueExpressionDriver::makeDeltaField()
{
    return new vectorField(patch_.delta());
}

scalarField *SubsetValueExpressionDriver::makeWeightsField()
{
    return new scalarField(patch_.weights());
}

scalarField *SubsetValueExpressionDriver::makeFaceIdField()
{
    scalarField *result=new scalarField(patch_.size());
    forAll(*result,i) {
        (*result)[i]=i;
    }
    return result;
}

const fvMesh &SubsetValueExpressionDriver::mesh() const
{
    return patch_.boundaryMesh().mesh();
}

label SubsetValueExpressionDriver::size() const
{
    return patch_.size();
}

label SubsetValueExpressionDriver::pointSize() const
{
    return patch_.patch().nPoints();
}

// ************************************************************************* //

} // namespace
