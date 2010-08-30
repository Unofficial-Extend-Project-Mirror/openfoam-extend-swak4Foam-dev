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

#include "CellSetValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CellSetValueExpressionDriver, 0);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellSetValueExpressionDriver, dictionary, cellSet);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


    CellSetValueExpressionDriver::CellSetValueExpressionDriver(const cellSet &set,const CellSetValueExpressionDriver& orig)
:
        SubsetValueExpressionDriver(orig),
        cellSet_(
            dynamicCast<const fvMesh&>(set.db()),
            set.name()+"_copy",
            set
        )
{}

CellSetValueExpressionDriver::CellSetValueExpressionDriver(const cellSet &set)
:
    SubsetValueExpressionDriver(),
    cellSet_(
            dynamicCast<const fvMesh&>(set.db()),
            set.name()+"_copy",
            set
    )
{}

CellSetValueExpressionDriver::CellSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    SubsetValueExpressionDriver(dict),
    cellSet_(
            regionMesh(dict,mesh),
            dict.lookup("setName"),
            IOobject::MUST_READ
    )
{
}
// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CellSetValueExpressionDriver::~CellSetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
label SubsetValueExpressionDriver::getIndexFromIterator(const cellSet::const_iterator &it) 
{
    return it.key();
}

Field<scalar> *CellSetValueExpressionDriver::getScalarField(const string &name)
{
    return getFieldInternal<volScalarField,cellSet,scalar>(name,cellSet_);
}

Field<vector> *CellSetValueExpressionDriver::getVectorField(const string &name)
{
    return getFieldInternal<volVectorField,cellSet,vector>(name,cellSet_);
}

Field<tensor> *CellSetValueExpressionDriver::getTensorField(const string &name)
{
    return getFieldInternal<volTensorField,cellSet,tensor>(name,cellSet_);
}

Field<symmTensor> *CellSetValueExpressionDriver::getSymmTensorField(const string &name)
{
    return getFieldInternal<volSymmTensorField,cellSet,symmTensor>(name,cellSet_);
}

Field<sphericalTensor> *CellSetValueExpressionDriver::getSphericalTensorField(const string &name)
{
    return getFieldInternal<volSphericalTensorField,cellSet,sphericalTensor>(name,cellSet_);
}

vectorField *CellSetValueExpressionDriver::makePositionField()
{
    return getFromFieldInternal(this->mesh().C(),cellSet_);
}

scalarField *CellSetValueExpressionDriver::makeCellVolumeField()
{
    return getFromFieldInternal(this->mesh().V(),cellSet_);
}


// vectorField *CellSetValueExpressionDriver::makePointField()
// {
//     notImplemented("CellSetValueExpressionDriver::makePointField");
// }

scalarField *CellSetValueExpressionDriver::makeFaceAreaMagField()
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceAreaField()")
        << "cellSet knows nothing about faces"
            << endl
            << abort(FatalError);
    return new scalarField(0);
}

vectorField *CellSetValueExpressionDriver::makeFaceNormalField()
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceNormalField()")
        << "cellSet knows nothing about faces"
            << endl
            << abort(FatalError);
    return new vectorField(0);
}

vectorField *CellSetValueExpressionDriver::makeFaceAreaField()
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceAreaField()")
        << "cellSet knows nothing about faces"
            << endl
            << abort(FatalError);
    return new vectorField(0);
}

// ************************************************************************* //

} // namespace
