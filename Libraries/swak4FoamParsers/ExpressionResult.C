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

#include "ExpressionResult.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

ExpressionResult::ExpressionResult()
:
    valType_("None"),
    valPtr_(NULL),
    isPoint_(false),
    isSingleValue_(true)
{
    clearResult();
}

ExpressionResult::ExpressionResult(const ExpressionResult &rhs)
:
    valType_("None"),
    valPtr_(NULL),
    isPoint_(false),
    isSingleValue_(true)
{
    (*this)=rhs;
}

ExpressionResult::ExpressionResult(
    const dictionary &dict,
    bool isSingleValue
)
:
    valType_(dict.lookupOrDefault<word>("valueType","None")),
    valPtr_(NULL),
    isPoint_(dict.lookupOrDefault<bool>("isPoint",false)),
    isSingleValue_(
        dict.lookupOrDefault<bool>("isSingleValue",isSingleValue)
    )
{
    if(
        dict.found("value")
    ) {
        if(isSingleValue_) {
            if(valType_==pTraits<scalar>::typeName) {
                valPtr_=new scalarField(1,pTraits<scalar>(dict.lookup("value")));
            } else if(valType_==pTraits<vector>::typeName) {
                valPtr_=new Field<vector>(1,pTraits<vector>(dict.lookup("value")));
            } else if(valType_==pTraits<tensor>::typeName) {
                valPtr_=new Field<tensor>(1,pTraits<tensor>(dict.lookup("value")));
            } else if(valType_==pTraits<symmTensor>::typeName) {
                valPtr_=new Field<symmTensor>(1,pTraits<symmTensor>(dict.lookup("value")));
            } else if(valType_==pTraits<sphericalTensor>::typeName) {
                valPtr_=new Field<sphericalTensor>(1,pTraits<sphericalTensor>(dict.lookup("value")));
            } else {
                FatalErrorIn("ExpressionResult::ExpressionResult(const dictionary &dict)")
                    << "Don't know how to read data type " << valType_ 
                        << " as a single value" << endl
                        << abort(FatalError);
            }
        } else {
            if(valType_==pTraits<scalar>::typeName) {
                valPtr_=new scalarField(dict.lookup("value"));
            } else if(valType_==vector::typeName) {
                valPtr_=new Field<vector>(dict.lookup("value"));
            } else if(valType_==tensor::typeName) {
                valPtr_=new Field<tensor>(dict.lookup("value"));
            } else if(valType_==symmTensor::typeName) {
                valPtr_=new Field<symmTensor>(dict.lookup("value"));
            } else if(valType_==sphericalTensor::typeName) {
                valPtr_=new Field<sphericalTensor>(dict.lookup("value"));
            } else if(valType_==pTraits<bool>::typeName) {
                valPtr_=new Field<bool>(dict.lookup("value"));
            } else {
                FatalErrorIn("ExpressionResult::ExpressionResult(const dictionary &dict)")
                    << "Don't know how to read data type " << valType_ << endl
                        << abort(FatalError);
            }
        }
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

ExpressionResult::~ExpressionResult()
{
    uglyDelete();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool ExpressionResult::hasValue() const
{
    return valType_!="None" && valPtr_!=NULL;
}

void ExpressionResult::clearResult()
{
    uglyDelete();
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void ExpressionResult::uglyDelete()
{
    if( valPtr_ ) {
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
            WarningIn("ExpressionResult::uglyDelete()")
                << "Unknown type " << valType_ << " propable memory loss" << endl;
            delete valPtr_;
        }
    }
    valType_="Void";
    valPtr_=NULL;
}

ExpressionResult ExpressionResult::getUniform(const label size,bool noWarn) const
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
            FatalErrorIn("ExpressionResult::getUniformInternal<bool>(const label size,bool noWarn)")
                << "This specialisation is not implemented"
                    << endl << abort(FatalError);

            return ExpressionResult(); // makes warnings go away
        } else {
            FatalErrorIn("ExpressionResult::getUniform()")
                << "Unknown type " << valType_ << endl
                    << abort(FatalError);

            return ExpressionResult(); // makes warnings go away
        }
    } else {
        FatalErrorIn("ExpressionResult::getUniform()")
            << "Not set. Can't construct an uniform value" << endl
                << abort(FatalError);

        return ExpressionResult(); // makes warnings go away
    }
}

void ExpressionResult::operator=(const ExpressionResult& rhs)
{
    // Check for assignment to self
    if (this == &rhs)
    {
        FatalErrorIn("ExpressionResult::operator=(const ExpressionResult&)")
            << "Attempted assignment to self"
            << abort(FatalError);
    }

    clearResult();

    valType_=rhs.valType_;
    isPoint_=rhs.isPoint_;
    isSingleValue_=rhs.isSingleValue_;

    if( rhs.valPtr_ ) {
        if(valType_==pTraits<scalar>::typeName) {
            valPtr_=new scalarField(*static_cast<scalarField*>(rhs.valPtr_));
        } else if(valType_==pTraits<vector>::typeName) {
            valPtr_=new Field<vector>(*static_cast<Field<vector>*>(rhs.valPtr_));
        } else if(valType_==pTraits<tensor>::typeName) {
            valPtr_=new Field<tensor>(*static_cast<Field<tensor>*>(rhs.valPtr_));
        } else if(valType_==pTraits<symmTensor>::typeName) {
            valPtr_=new Field<symmTensor>(*static_cast<Field<symmTensor>*>(rhs.valPtr_));
        } else if(valType_==pTraits<sphericalTensor>::typeName) {
            valPtr_=new Field<sphericalTensor>(*static_cast<Field<sphericalTensor>*>(rhs.valPtr_));
        } else if(valType_==pTraits<bool>::typeName) {
            valPtr_=new Field<bool>(*static_cast<Field<bool>*>(rhs.valPtr_));
        } else {
            FatalErrorIn("ExpressionResult::operator=(const ExpressionResult& rhs)")
                << " Type " << valType_ << " can not be copied"
                    << endl
                    << abort(FatalError);
        }
    } else {
        valPtr_=rhs.valPtr_;
    }

//     const_cast<ExpressionResult &>(rhs).valPtr_=NULL;
//     const_cast<ExpressionResult &>(rhs).clearResult();
}


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

// I have NO idea why this is necessary, but since the introduction of the 
// enable_if_rank0-stuff the function below does not compile without it

template<>
class pTraits<token::punctuationToken> 
{};

template<int N>
class pTraits<char [N]> 
{};

template<>
class pTraits<Ostream&(Ostream&)> 
{};

template<>
class pTraits<char> 
{};

template<>
class pTraits<const char *> 
{};

Ostream & operator<<(Ostream &out,const ExpressionResult &data) 
{
    out << token::BEGIN_BLOCK << endl;

    if( data.valPtr_ ) {
        out.writeKeyword("valueType");
        out << word(data.valType_) << token::END_STATEMENT << nl;

        out.writeKeyword("isPoint");
        out << data.isPoint_ << token::END_STATEMENT << nl;

        out.writeKeyword("isSingleValue");
        out << data.isSingleValue_ << token::END_STATEMENT << nl;

        out.writeKeyword("value");
        if(data.valType_==pTraits<scalar>::typeName) {
            out << *static_cast<scalarField*>(data.valPtr_);
        } else if(data.valType_==vector::typeName) {
            out << *static_cast<Field<vector>*>(data.valPtr_);
        } else if(data.valType_==tensor::typeName) {
            out << *static_cast<Field<tensor>*>(data.valPtr_);
        } else if(data.valType_==symmTensor::typeName) {
            out << *static_cast<Field<symmTensor>*>(data.valPtr_);
        } else if(data.valType_==sphericalTensor::typeName) {
            out << *static_cast<Field<sphericalTensor>*>(data.valPtr_);
        } else if(data.valType_==pTraits<bool>::typeName) {
            out << *static_cast<Field<bool>*>(data.valPtr_);
        } else {
            out << "ExpressionResult: unknown data type " << data.valType_ << endl;
        }
        out << token::END_STATEMENT << nl;
    } else {
        out << "ExpressionResult: not data defined";
    }

    out << token::END_BLOCK << endl;

    return out;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
