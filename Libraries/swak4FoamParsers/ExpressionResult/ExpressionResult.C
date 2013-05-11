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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "ExpressionResult.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#include "addToRunTimeSelectionTable.H"

// #include "swakPTraitsSpecialization.H"

namespace Foam {

defineTypeNameAndDebug(ExpressionResult,0);

defineRunTimeSelectionTable(ExpressionResult, dictionary);
defineRunTimeSelectionTable(ExpressionResult, nothing);

addToRunTimeSelectionTable(ExpressionResult, ExpressionResult, dictionary);
addToRunTimeSelectionTable(ExpressionResult, ExpressionResult, nothing);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

ExpressionResult::ExpressionResult()
:
    refCount(),
    valType_("None"),
    valPtr_(NULL),
    isPoint_(false),
    isSingleValue_(true),
    objectSize_(-1),
    noReset_(false),
    needsReset_(false)
{
    if(debug) {
        Info << "ExpressionResult::ExpressionResult()" << endl;
    }
    clearResult();
}

ExpressionResult::ExpressionResult(const ExpressionResult &rhs)
:
    refCount(),
    valType_("None"),
    valPtr_(NULL),
    isPoint_(false),
    isSingleValue_(true),
    objectSize_(-1),
    noReset_(false),
    needsReset_(false)
{
    if(debug) {
        Info << "ExpressionResult::ExpressionResult(const ExpressionResult &rhs)" << endl;
        // if(rhs.type()!="ExpressionResult") {
        //     FatalErrorIn("Hepp")
        //         << "Hey"
        //             << abort(FatalError);
        // }
        Info << "Rhs: " << rhs << endl;
    }

    (*this)=rhs;
}

ExpressionResult::ExpressionResult(
    const dictionary &dict,
    bool isSingleValue,
    bool needsValue
)
:
    refCount(),
    valType_(dict.lookupOrDefault<word>("valueType","None")),
    valPtr_(NULL),
    isPoint_(dict.lookupOrDefault<bool>("isPoint",false)),
    isSingleValue_(
        dict.lookupOrDefault<bool>("isSingleValue",isSingleValue)
    ),
    objectSize_(-1),
    noReset_(
        dict.lookupOrDefault<bool>("noReset",false)
    ),
    needsReset_(false)
{
    if(debug) {
        Info << "ExpressionResult::ExpressionResult(const dictionary &dict,bool isSingleValue)" << endl;
    }

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
                        << exit(FatalError);
            }
        } else {
            label fs=readLabel(dict.lookup("fieldSize"));

            if(valType_==pTraits<scalar>::typeName) {
                valPtr_=new scalarField("value",dict,fs);
            } else if(valType_==vector::typeName) {
                valPtr_=new Field<vector>("value",dict,fs);
            } else if(valType_==tensor::typeName) {
                valPtr_=new Field<tensor>("value",dict,fs);
            } else if(valType_==symmTensor::typeName) {
                valPtr_=new Field<symmTensor>("value",dict,fs);
            } else if(valType_==sphericalTensor::typeName) {
                valPtr_=new Field<sphericalTensor>("value",dict,fs);
            } else if(valType_==pTraits<bool>::typeName) {
                valPtr_=new Field<bool>("value",dict,fs);
            } else {
                FatalErrorIn("ExpressionResult::ExpressionResult(const dictionary &dict)")
                    << "Don't know how to read data type " << valType_ << endl
                        << exit(FatalError);
            }
        }
    } else if(needsValue) {
        FatalErrorIn("ExpressionResult::ExpressionResult(const dictionary &dict,bool isSingleValue)")
            << "No entry 'value' defined in " << dict.name() << endl
                << dict
                << endl
                << exit(FatalError);

    }
}

autoPtr<ExpressionResult> ExpressionResult::New
(
    const dictionary& dict
)
{
    word resultType("ExpressionResult");
    if(dict.found("resultType")) {
        resultType=word(dict.lookup("resultType"));
    }
    bool unset=dict.lookupOrDefault<bool>("unsetValue",false);

    if(unset) {
        nothingConstructorTable::iterator cstrIter =
            nothingConstructorTablePtr_->find(resultType);

        if (cstrIter == nothingConstructorTablePtr_->end())
        {
            FatalErrorIn
                (
                    "autoPtr<ExpressionResult> ExpressionResult::New"
                )   << "Unknown  ExpressionResult type " << resultType
                    << endl << endl
                    << "Valid resultTypes are :" << endl
#ifdef FOAM_HAS_SORTED_TOC
                    << nothingConstructorTablePtr_->sortedToc() // does not work in 1.6
#else
                    << nothingConstructorTablePtr_->toc()
#endif
                    << exit(FatalError);
        }

        if(debug) {
            Pout << "Creating unset result of type " << resultType << endl;
        }

        return autoPtr<ExpressionResult>
            (
                cstrIter()()
            );
    } else {
        dictionaryConstructorTable::iterator cstrIter =
            dictionaryConstructorTablePtr_->find(resultType);

        if (cstrIter == dictionaryConstructorTablePtr_->end())
        {
            FatalErrorIn
                (
                    "autoPtr<ExpressionResult> ExpressionResult::New"
                )   << "Unknown  ExpressionResult type " << resultType
                    << endl << endl
                    << "Valid resultTypes are :" << endl
#ifdef FOAM_HAS_SORTED_TOC
                    << dictionaryConstructorTablePtr_->sortedToc() // does not work in 1.6
#else
                    << dictionaryConstructorTablePtr_->toc()
#endif
                    << exit(FatalError);
        }

        if(debug) {
            Pout << "Creating result of type " << resultType << endl;
        }

        return autoPtr<ExpressionResult>
            (
                cstrIter()(dict)
            );
    }
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

ExpressionResult::~ExpressionResult()
{
    if(debug) {
        Info << "ExpressionResult::~ExpressionResult()" << endl;
    }

    uglyDelete();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool ExpressionResult::hasValue() const
{
    return valType_!="None" && valPtr_!=NULL;
}

void ExpressionResult::resetInternal() {
    clearResult();
}

bool ExpressionResult::reset(bool force) {
    if(
        force
        ||
        !noReset_
        ||
        needsReset_
    ) {
        this->resetInternal();

        return true;
    } else {
        return false;
    }
}

void ExpressionResult::clearResult()
{
    if(debug) {
        Info << "ExpressionResult: Clearing result" << endl;
    }

    uglyDelete();

    if(debug) {
        Info << "ExpressionResult: Clearing object" << endl;
    }
    generalContent_.reset();
    objectSize_=-1;

    if(debug) {
        Info << "ExpressionResult: Clearing result - done" << endl;
    }
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void ExpressionResult::uglyDelete()
{
    if(debug) {
        Info << "ExpressionResult: uglyDelete" << endl;
    }

    if( valPtr_ ) {
        if(debug) {
            Info << "ExpressionResult: uglyDelete - clearing pointer" << endl;
        }

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
                << "Unknown type " << valType_
                    << " propable memory loss" << endl;
            delete valPtr_;
        }
    }
    valType_="Void";
    valPtr_=NULL;

    if(debug) {
        Info << "ExpressionResult: uglyDelete - done" << endl;
    }
}

ExpressionResult ExpressionResult::getUniform(
    const label size,
    bool noWarn
) const
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
                    << endl << exit(FatalError);

            return ExpressionResult(); // makes warnings go away
        } else {
            FatalErrorIn("ExpressionResult::getUniform()")
                << "Unknown type " << valType_ << endl
                    << exit(FatalError);

            return ExpressionResult(); // makes warnings go away
        }
    } else {
        FatalErrorIn("ExpressionResult::getUniform()")
            << "Not set. Can't construct an uniform value" << endl
                << exit(FatalError);

        return ExpressionResult(); // makes warnings go away
    }
}

label ExpressionResult::size() const {
    if(valPtr_) {
        if(valType_==pTraits<scalar>::typeName) {
            return static_cast<scalarField*>(valPtr_)->size();
        } else if(valType_==vector::typeName) {
            return static_cast<Field<vector>*>(valPtr_)->size();
        } else if(valType_==tensor::typeName) {
            return static_cast<Field<tensor>*>(valPtr_)->size();
        } else if(valType_==symmTensor::typeName) {
            return static_cast<Field<symmTensor>*>(valPtr_)->size();
        } else if(valType_==sphericalTensor::typeName) {
            return static_cast<Field<sphericalTensor>*>(valPtr_)->size();
        } else if(valType_==pTraits<bool>::typeName) {
            return static_cast<Field<bool>*>(valPtr_)->size();
        } else {
            WarningIn("ExpressionResult::size()")
                << "Unknown type " << valType_ << endl;
            return -1;
        }
    } else {
        // this is an object
        return objectSize_;
    }
}

void ExpressionResult::operator=(const ExpressionResult& rhs)
{
    if(debug) {
        Info << "ExpressionResult::operator=(const ExpressionResult& rhs)"
            << endl;
        Info << "Rhs: " << rhs << endl;
    }
    // Check for assignment to self
    if (this == &rhs)
    {
        FatalErrorIn("ExpressionResult::operator=(const ExpressionResult&)")
            << "Attempted assignment to self"
            << exit(FatalError);
    }

    clearResult();

    valType_=rhs.valType_;
    isPoint_=rhs.isPoint_;
    isSingleValue_=rhs.isSingleValue_;
    objectSize_=-1;

    if( rhs.valPtr_ ) {
        if(valType_==pTraits<scalar>::typeName) {
            valPtr_=new scalarField(*static_cast<scalarField*>(rhs.valPtr_));
        } else if(valType_==pTraits<vector>::typeName) {
            valPtr_=new Field<vector>(
                *static_cast<Field<vector>*>(
                    rhs.valPtr_
                )
            );
        } else if(valType_==pTraits<tensor>::typeName) {
            valPtr_=new Field<tensor>(
                *static_cast<Field<tensor>*>(
                    rhs.valPtr_
                )
            );
        } else if(valType_==pTraits<symmTensor>::typeName) {
            valPtr_=new Field<symmTensor>(
                *static_cast<Field<symmTensor>*>(
                    rhs.valPtr_
                )
            );
        } else if(valType_==pTraits<sphericalTensor>::typeName) {
            valPtr_=new Field<sphericalTensor>(
                *static_cast<Field<sphericalTensor>*>(
                    rhs.valPtr_
                )
            );
        } else if(valType_==pTraits<bool>::typeName) {
            valPtr_=new Field<bool>(
                *static_cast<Field<bool>*>(
                    rhs.valPtr_
                )
            );
        } else {
            FatalErrorIn("ExpressionResult::operator=(const ExpressionResult& rhs)")
                << " Type " << valType_ << " can not be copied"
                    << endl
                    << exit(FatalError);
        }
    } else {
        valPtr_=rhs.valPtr_;
        if(generalContent_.valid()) {
            FatalErrorIn("ExpressionResult::operator=(const ExpressionResult& rhs)")
                << "Assignment with general content not possible"
                    << endl
                    << exit(FatalError);

        }
    }

//     const_cast<ExpressionResult &>(rhs).valPtr_=NULL;
//     const_cast<ExpressionResult &>(rhs).clearResult();
}


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

Ostream & operator<<(Ostream &out,const ExpressionResult &data)
{
    out << token::BEGIN_BLOCK << endl;

    out.writeKeyword("resultType");
    out << word(data.type()) << token::END_STATEMENT << nl;

    out.writeKeyword("noReset");
    out << data.noReset_ << token::END_STATEMENT << nl;

    if( data.valPtr_ ) {
        out.writeKeyword("valueType");
        out << word(data.valType_) << token::END_STATEMENT << nl;

        out.writeKeyword("isPoint");
        out << data.isPoint_ << token::END_STATEMENT << nl;

        out.writeKeyword("isSingleValue");
        out << data.isSingleValue_ << token::END_STATEMENT << nl;

        out.writeKeyword("fieldSize");
        out << data.size() << token::END_STATEMENT << nl;

        if(data.isSingleValue_) {
            out.writeKeyword("value");
            if(data.valType_==pTraits<scalar>::typeName) {
                out << (*static_cast<Field<scalar>*>(data.valPtr_))[0];
            } else if(data.valType_==vector::typeName) {
                out << (*static_cast<Field<vector>*>(data.valPtr_))[0];
            } else if(data.valType_==tensor::typeName) {
                out << (*static_cast<Field<tensor>*>(data.valPtr_))[0];
            } else if(data.valType_==symmTensor::typeName) {
                out << (*static_cast<Field<symmTensor>*>(data.valPtr_))[0];
            } else if(data.valType_==sphericalTensor::typeName) {
                out << (*static_cast<Field<sphericalTensor>*>(data.valPtr_))[0];
            } else if(data.valType_==pTraits<bool>::typeName) {
                out << (*static_cast<Field<bool>*>(data.valPtr_))[0];
            } else {
                out << "ExpressionResult: unknown data type " << data.valType_ << endl;
            }
            out << token::END_STATEMENT << nl;
        } else {
            out.writeKeyword("value");
            if(data.valType_==pTraits<scalar>::typeName) {
                out << "nonuniform " << (*static_cast<Field<scalar>*>(data.valPtr_));
                //                static_cast<Field<scalar>*>(data.valPtr_)->writeEntry("value",out);
            } else if(data.valType_==vector::typeName) {
                out << "nonuniform " << (*static_cast<Field<vector>*>(data.valPtr_));
                //                static_cast<Field<vector>*>(data.valPtr_)->writeEntry("value",out);
            } else if(data.valType_==tensor::typeName) {
                out << "nonuniform " << (*static_cast<Field<tensor>*>(data.valPtr_));
                //                static_cast<Field<tensor>*>(data.valPtr_)->writeEntry("value",out);
            } else if(data.valType_==symmTensor::typeName) {
                out << "nonuniform " << (*static_cast<Field<symmTensor>*>(data.valPtr_));
                //                static_cast<Field<symmTensor>*>(data.valPtr_)->writeEntry("value",out);
            } else if(data.valType_==sphericalTensor::typeName) {
                out << "nonuniform " << (*static_cast<Field<sphericalTensor>*>(data.valPtr_));
                //                static_cast<Field<sphericalTensor>*>(data.valPtr_)->writeEntry("value",out);
            } else if(data.valType_==pTraits<bool>::typeName) {
                out << "nonuniform " << (*static_cast<Field<bool>*>(data.valPtr_));
                //                static_cast<Field<bool>*>(data.valPtr_)->writeEntry("value",out);
            } else {
                out << "ExpressionResult: unknown data type " << data.valType_ << endl;
            }
            out << token::END_STATEMENT << nl;
        }
    } else {
        out.writeKeyword("unsetValue");
        out << true << token::END_STATEMENT << nl;
    }

    out << token::END_BLOCK << endl;

    return out;
}

Istream & operator>>(Istream &in,ExpressionResult &data)
{
    if(ExpressionResult::debug) {
        Info << "operator>>(Istream &in,ExpressionResult &data)" << endl;
    }

    dictionary dict(in);

    data=ExpressionResult(dict);

    return in;
}

ExpressionResult operator*(const scalar &f,const ExpressionResult &orig)
{
    if(orig.isObject()) {
        FatalErrorIn("operator*(const scalar &f,const ExpressionResult &orig)")
            << "Can only multiply Field-type ExpressionResult. Not if they're"
                << orig.valType_
                << endl
                << exit(FatalError);
    }
    ExpressionResult result(orig);

    if(result.valType_==pTraits<scalar>::typeName) {
        (*static_cast<scalarField*>(result.valPtr_))*=f;
    } else if(result.valType_==vector::typeName) {
        (*static_cast<Field<vector>*>(result.valPtr_))*=f;
    } else if(result.valType_==tensor::typeName) {
        (*static_cast<Field<tensor>*>(result.valPtr_))*=f;
    } else if(result.valType_==symmTensor::typeName) {
        (*static_cast<Field<symmTensor>*>(result.valPtr_))*=f;
    } else if(result.valType_==sphericalTensor::typeName) {
        (*static_cast<Field<sphericalTensor>*>(result.valPtr_))*=f;
        //    } else if(result.valType_==pTraits<bool>::typeName) {
    } else {
        FatalErrorIn("operator*(const scalar &f,const ExpressionResult &orig)")
            << "Can not multiply Field-type ExpressionResult of type"
                << result.valType_
                << endl
                << exit(FatalError);
    }
    return result;
}

ExpressionResult operator+(
    const ExpressionResult &a,const ExpressionResult &b
) {
    if(a.valType_!=b.valType_) {
        FatalErrorIn("operator+(const ExpressionResult &a,const ExpressionResult &b)")
            << "Different types "
                << a.valType_ << " and " << b.valType_
                << endl
                << exit(FatalError);
    }
    if(a.isObject()) {
        FatalErrorIn("operator+(const ExpressionResult &a,const ExpressionResult &b)")
            << "Can only multiply Field-type ExpressionResult. Not if they're"
                << a.valType_
                << endl
                << exit(FatalError);
    }
    if(a.size()!=b.size()) {
        FatalErrorIn("operator+(const ExpressionResult &a,const ExpressionResult &b)")
            << "Different sizes "
                << a.size() << " and " << b.size()
                << endl
                << exit(FatalError);
    }

    ExpressionResult result(a);

    if(result.valType_==pTraits<scalar>::typeName) {
        (*static_cast<scalarField*>(result.valPtr_))+=
            (*static_cast<scalarField*>(b.valPtr_));
    } else if(result.valType_==vector::typeName) {
        (*static_cast<Field<vector>*>(result.valPtr_))+=
            (*static_cast<Field<vector>*>(b.valPtr_));
    } else if(result.valType_==tensor::typeName) {
        (*static_cast<Field<tensor>*>(result.valPtr_))+=
            (*static_cast<Field<tensor>*>(b.valPtr_));
    } else if(result.valType_==symmTensor::typeName) {
        (*static_cast<Field<symmTensor>*>(result.valPtr_))+=
            (*static_cast<Field<symmTensor>*>(b.valPtr_));
    } else if(result.valType_==sphericalTensor::typeName) {
        (*static_cast<Field<sphericalTensor>*>(result.valPtr_))+=
            (*static_cast<Field<sphericalTensor>*>(b.valPtr_));
        //    } else if(result.valType_==pTraits<bool>::typeName) {
    } else {
        FatalErrorIn("operator+(const ExpressionResult &a,const ExpressionResult &b)")
            << "Can not add Field-type ExpressionResult of type"
                << result.valType_
                << endl
                << exit(FatalError);
    }
    return result;
}

word ExpressionResult::getAddressAsDecimal() const
{
    std::ostringstream makeDec;
     if(valType_==pTraits<scalar>::typeName) {
         makeDec << (*static_cast<scalarField*>(valPtr_)).cdata();
     } else if(valType_==pTraits<vector>::typeName) {
         makeDec << (*static_cast<Field<vector>*>(valPtr_)).cdata();
     } else if(valType_==pTraits<tensor>::typeName) {
         makeDec << (*static_cast<Field<tensor>*>(valPtr_)).cdata();
     } else if(valType_==pTraits<symmTensor>::typeName) {
         makeDec << (*static_cast<Field<symmTensor>*>(valPtr_)).cdata();
     } else if(valType_==pTraits<sphericalTensor>::typeName) {
         makeDec << (*static_cast<Field<sphericalTensor>*>(valPtr_)).cdata();
     } else {
         FatalErrorIn("ExpressionResult::getAddressAsDecimal")
             << "Unsupported type"
                 << valType_
                 << endl
                 << exit(FatalError);
     }

    return word(makeDec.str());
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
