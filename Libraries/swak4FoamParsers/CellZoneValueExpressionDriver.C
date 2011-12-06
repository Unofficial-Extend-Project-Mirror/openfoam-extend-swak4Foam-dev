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

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CellZoneValueExpressionDriver, 0);

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellZoneValueExpressionDriver, dictionary, cellZone);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellZoneValueExpressionDriver, idName, cellZone);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

label getCellZoneID(const fvMesh &mesh,const word &name)
{
    label result=mesh.cellZones().findZoneID(name);
    if(result<0) {
        FatalErrorIn("getCellZoneID(const fvMesh &mesh,const word &name)")
            << "The cellZone " << name << " was not found in "
                << mesh.cellZones().names()
                << endl
                << exit(FatalError);

    }
    return result;
}


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

CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    SubsetValueExpressionDriver(dict),
    cellZone_(
        regionMesh(
            dict,
            mesh,
            searchOnDisc()
        ).cellZones()[
            getCellZoneID(
                regionMesh(
                    dict,
                    mesh,
                    searchOnDisc()
                ),
                dict.lookup(
                    "zoneName"
                )                
            )
        ]
    )
{
}

CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(const word& id,const fvMesh&mesh)
 :
    SubsetValueExpressionDriver(),
    cellZone_(
        mesh.cellZones()[
            getCellZoneID(
                mesh,
                id
            )
        ]
    )
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CellZoneValueExpressionDriver::~CellZoneValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<>
inline label SubsetValueExpressionDriver::getIndexFromIterator(const cellZone::const_iterator &it) 
{
    return *it;
}

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

Field<sphericalTensor> *CellZoneValueExpressionDriver::getSphericalTensorField(const string &name)
{
    return getFieldInternal<volSphericalTensorField,cellZone,sphericalTensor>(name,cellZone_);
}

vectorField *CellZoneValueExpressionDriver::makePositionField()
{
    return getFromFieldInternal(this->mesh().C(),cellZone_);
}

scalarField *CellZoneValueExpressionDriver::makeCellVolumeField()
{
    return getFromFieldInternal(this->mesh().V(),cellZone_);
}


// vectorField *CellZoneValueExpressionDriver::makePointField()
// {
//     notImplemented("CellZoneValueExpressionDriver::makePointField");
// }

scalarField *CellZoneValueExpressionDriver::makeFaceAreaMagField()
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceAreaField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return new scalarField(0);
}

vectorField *CellZoneValueExpressionDriver::makeFaceNormalField()
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceNormalField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return new vectorField(0);
}

scalarField *CellZoneValueExpressionDriver::makeFaceFlipField()
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceFlipField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return new scalarField(0);
}

vectorField *CellZoneValueExpressionDriver::makeFaceAreaField()
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceAreaField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return new vectorField(0);
}

// ************************************************************************* //

} // namespace
