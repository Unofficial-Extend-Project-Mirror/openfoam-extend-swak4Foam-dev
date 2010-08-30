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

#include "FaceSetValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(FaceSetValueExpressionDriver, 0);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceSetValueExpressionDriver, dictionary, faceSet);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


    FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(const faceSet &set,const FaceSetValueExpressionDriver& orig)
:
        SubsetValueExpressionDriver(orig),
        faceSet_(
            dynamicCast<const fvMesh&>(set.db()),
            set.name()+"_copy",
            set
        )
{}

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(const faceSet &set)
:
    SubsetValueExpressionDriver(),
    faceSet_(
            dynamicCast<const fvMesh&>(set.db()),
            set.name()+"_copy",
            set
    )
{}

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    SubsetValueExpressionDriver(dict),
    faceSet_(
            regionMesh(dict,mesh),
            dict.lookup("setName"),
            IOobject::MUST_READ
    )
{
}
// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

FaceSetValueExpressionDriver::~FaceSetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
inline label SubsetValueExpressionDriver::getIndexFromIterator(const faceSet::const_iterator &it) 
{
    return it.key();
}

Field<scalar> *FaceSetValueExpressionDriver::getScalarField(const string &name)
{
    return getFieldInternal<volScalarField,faceSet,scalar>(name,faceSet_);
}

Field<vector> *FaceSetValueExpressionDriver::getVectorField(const string &name)
{
    return getFieldInternal<volVectorField,faceSet,vector>(name,faceSet_);
}

Field<tensor> *FaceSetValueExpressionDriver::getTensorField(const string &name)
{
    return getFieldInternal<volTensorField,faceSet,tensor>(name,faceSet_);
}

Field<symmTensor> *FaceSetValueExpressionDriver::getSymmTensorField(const string &name)
{
    return getFieldInternal<volSymmTensorField,faceSet,symmTensor>(name,faceSet_);
}

Field<sphericalTensor> *FaceSetValueExpressionDriver::getSphericalTensorField(const string &name)
{
    return getFieldInternal<volSphericalTensorField,faceSet,sphericalTensor>(name,faceSet_);
}

vectorField *FaceSetValueExpressionDriver::makePositionField()
{
    return getFromFieldInternal(this->mesh().Cf(),faceSet_);
}

scalarField *FaceSetValueExpressionDriver::makeCellVolumeField()
{
    FatalErrorIn("FaceSetValueExpressionDriver::makeCellVolumeField()")
        << "faceSet knows nothing about cells"
            << endl
            << abort(FatalError);
    return new scalarField(0);
}


// vectorField *FaceSetValueExpressionDriver::makePointField()
// {
//     notImplemented("FaceSetValueExpressionDriver::makePointField");
// }

scalarField *FaceSetValueExpressionDriver::makeFaceAreaMagField()
{
    return getFromFieldInternal(this->mesh().magSf(),faceSet_);
}

vectorField *FaceSetValueExpressionDriver::makeFaceNormalField()
{
    autoPtr<vectorField> sf(this->makeFaceAreaField());
    autoPtr<scalarField> magSf(this->makeFaceAreaMagField());

    return new vectorField(sf()/magSf());
}

vectorField *FaceSetValueExpressionDriver::makeFaceAreaField()
{
    return getFromFieldInternal(this->mesh().Sf(),faceSet_);
}

// ************************************************************************* //

} // namespace
