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

#include "PatchResult.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

PatchResult::PatchResult()
:
    valType_("None"),
    valPtr_(NULL),
    isPoint_(false)
{
    clearResult();
}

PatchResult::PatchResult(const PatchResult &rhs)
:
    valType_("None"),
    valPtr_(NULL)
{
    (*this)=rhs;
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

PatchResult::~PatchResult()
{
    uglyDelete();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void PatchResult::clearResult()
{
    uglyDelete();
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void PatchResult::uglyDelete()
{
    if(valPtr_) {
        if(valType_==pTraits<scalar>::typeName) {
            delete static_cast<scalarField*>(valPtr_);
        } else if(valType_==vector::typeName) {
            delete static_cast<Field<vector>*>(valPtr_);
        } else if(valType_==tensor::typeName) {
            delete static_cast<Field<tensor>*>(valPtr_);
        } else if(valType_==symmTensor::typeName) {
            delete static_cast<Field<symmTensor>*>(valPtr_);
        } else if(valType_==sphericalTensor::typeName) {
            delete static_cast<Field<sphericalTensor>*>(valPtr_);
        } else if(valType_==pTraits<bool>::typeName) {
            delete static_cast<Field<bool>*>(valPtr_);
        } else {
            WarningIn("PatchResult::uglyDelete()")
                << "Unknown type " << valType_ << " propable memory loss" << endl;
            delete valPtr_;
        }
    }
    valType_="Void";
    valPtr_=NULL;
}

PatchResult PatchResult::getUniform(const label size,bool noWarn)
{
    if(valPtr_) {
        if(valType_==pTraits<scalar>::typeName) {
            return getUniformInternal<scalar>(size,noWarn);
        } else if(valType_==vector::typeName) {
            return getUniformInternal<vector>(size,noWarn);
        } else if(valType_==tensor::typeName) {
            return getUniformInternal<tensor>(size,noWarn);
        } else if(valType_==symmTensor::typeName) {
            return getUniformInternal<symmTensor>(size,noWarn);
        } else if(valType_==sphericalTensor::typeName) {
            return getUniformInternal<sphericalTensor>(size,noWarn);
        } else if(valType_==pTraits<bool>::typeName) {
            FatalErrorIn("PatchResult::getUniformInternal<bool>(const label size,bool noWarn)")
                << "This specialisation is not implemented"
                    << endl << abort(FatalError);

            return PatchResult(); // makes warnings go away
        } else {
            FatalErrorIn("PatchResult::getUniform()")
                << "Unknown type " << valType_ << endl
                    << abort(FatalError);

            return PatchResult(); // makes warnings go away
        }
    } else {
        FatalErrorIn("PatchResult::getUniform()")
            << "Not set. Can't construct an uniform value" << endl
                << abort(FatalError);

        return PatchResult(); // makes warnings go away
    }
}

void PatchResult::operator=(const PatchResult& rhs)
{
    // Check for assignment to self
    if (this == &rhs)
    {
        FatalErrorIn("PatchResult::operator=(const PatchResult&)")
            << "Attempted assignment to self"
            << abort(FatalError);
    }

    valPtr_=rhs.valPtr_;
    valType_=rhs.valType_;
    isPoint_=rhs.isPoint_;

    const_cast<PatchResult &>(rhs).valPtr_=NULL;
    const_cast<PatchResult &>(rhs).clearResult();
}


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
