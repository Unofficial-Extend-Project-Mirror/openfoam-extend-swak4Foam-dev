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

#include "CellZoneValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


    CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(const cellZone &zone,const CellZoneValueExpressionDriver& orig)
:
        SubsetValueExpressionDriver(orig),
        cellZone_(zone)
{}

CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(const cellZone &zone)
:
    SubsetValueExpressionDriver(),
    cellZone_(zone)
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CellZoneValueExpressionDriver::~CellZoneValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Field<scalar> *CellZoneValueExpressionDriver::getScalarField(const string &name)
{
    return getFieldInternal<volScalarField,cellZone,scalar>(name,cellZone_);
}

Field<vector> *CellZoneValueExpressionDriver::getVectorField(const string &name)
{
    return getFieldInternal<volVectorField,cellZone,vector>(name,cellZone_);
}

Field<tensor> *CellZoneValueExpressionDriver::getTensorField(const string &name)
{
    return getFieldInternal<volTensorField,cellZone,tensor>(name,cellZone_);
}

Field<symmTensor> *CellZoneValueExpressionDriver::getSymmTensorField(const string &name)
{
    return getFieldInternal<volSymmTensorField,cellZone,symmTensor>(name,cellZone_);
}

vectorField *CellZoneValueExpressionDriver::makePositionField()
{
    notImplemented("CellZoneValueExpressionDriver::makePositionField");
}

Field<sphericalTensor> *CellZoneValueExpressionDriver::getSphericalTensorField(const string &name)
{
    return getFieldInternal<volSphericalTensorField,cellZone,sphericalTensor>(name,cellZone_);
}

// vectorField *CellZoneValueExpressionDriver::makePointField()
// {
//     notImplemented("CellZoneValueExpressionDriver::makePointField");
// }

vectorField *CellZoneValueExpressionDriver::makeFaceNormalField()
{
    notImplemented("CellZoneValueExpressionDriver::makeFaceNormalField");
}

vectorField *CellZoneValueExpressionDriver::makeFaceAreaField()
{
    notImplemented("CellZoneValueExpressionDriver::makeFaceAreaField");
}

// ************************************************************************* //

} // namespace
