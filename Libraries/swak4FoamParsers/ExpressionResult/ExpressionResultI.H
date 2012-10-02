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

namespace Foam {


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    template<class Type>
    ExpressionResult::ExpressionResult(const Field<Type>  &f)
        :
        valType_("None"),
        valPtr_(NULL),
        isPoint_(false),
        isSingleValue_(true)
    {
        setResult(f);
    }

    template<class Type>
    ExpressionResult::ExpressionResult(autoPtr<Type> o)
        :
        valType_("None"),
        valPtr_(NULL),
        isPoint_(false),
        isSingleValue_(true)
    {
        setObjectResult(o);
    }

    template<class Type>
    ExpressionResult::ExpressionResult(const dimensioned<Type>  &f)
        :
        valType_("None"),
        valPtr_(NULL),
        isPoint_(false),
        isSingleValue_(true)
    {
        setSingleValue(f.value());
    }

    template<class Type>
    ExpressionResult::ExpressionResult(
        const Type  &f,
        typename enable_if_rank0<pTraits<Type>::rank>::type* dummy
    )
        :
        valType_("None"),
        valPtr_(NULL),
        isPoint_(false),
        isSingleValue_(true)
    {
        setSingleValue(f);
    }

    ExpressionResult &ExpressionResult::toBeSet()
    {
        return *this;
    }

    template<class Type>
    void ExpressionResult::setResult(const Field<Type> &val,bool isPoint)
    {
        toBeSet().setResultInternal(val,isPoint);
    }

    template<class Type>
    void ExpressionResult::setResultInternal(
        const Field<Type> &val,bool isPoint
    ) {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(new Field<Type>(val));

        isPoint_=isPoint;
        isSingleValue_=false;
    }

    template <class T>
    void ExpressionResult::setObjectResult(autoPtr<T> o)
    {
        toBeSet().setObjectResultInternal(o);
    }

    template <class T>
    void ExpressionResult::setObjectResultInternal(autoPtr<T> o)
    {
        //        valType_=pTraits<T>::typeName;
        valType_=o().typeName;

        generalContent_.set(o.ptr());

        isPoint_=false;
        isSingleValue_=false;
    }

    template<class Type>
    void ExpressionResult::setResult(Field<Type> *val,bool isPoint)
    {
        toBeSet().setResultInternal(val,isPoint);
    }

    template<class Type>
    void ExpressionResult::setResultInternal(Field<Type> *val,bool isPoint)
    {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(val);

        isPoint_=isPoint;
        isSingleValue_=false;
    }

    template<class Type>
    void ExpressionResult::setResult(Type val,label size)
    {
        toBeSet().setResultInternal(val,size);
    }

    template<class Type>
    void ExpressionResult::setResultInternal(Type val,label size)
    {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(new Field<Type>(size,val));

        isPoint_=false;
        isSingleValue_=false;
    }

    template<class Type>
    void ExpressionResult::setSingleValue(Type val)
    {
        toBeSet().setSingleValueInternal(val);
    }

    template<class Type>
    void ExpressionResult::setSingleValueInternal(Type val)
    {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(new Field<Type>(1,val));

        isPoint_=false;
        isSingleValue_=true;
    }

    template<class Type>
    inline tmp<Field<Type> > ExpressionResult::getResult(bool keep)
    {
        if(valType_!=pTraits<Type>::typeName) {
            FatalErrorIn("tmp<Field<Type> > ExpressionResult::getResult()")
                << "The expected return type " << pTraits<Type>::typeName
                    << " is different from the stored result type "
                    << valType_ << "\n" << endl
                    << exit(FatalError);
        }

        tmp<Field<Type> > result;

        result=tmp<Field<Type> > (
            static_cast<Field<Type>*>(valPtr_)
        );

        if(keep) {
            valPtr_=new Field<Type>(*(static_cast<Field<Type>*>(valPtr_)));
        } else {
            valPtr_=NULL;
            clearResult();
        }

        return result;
    }

    template<class Type>
    inline tmp<Type> ExpressionResult::getObjectResult(bool keep)
    {
        if(debug) {
            Info << "ExpressionResult: getObjectResult: " << keep << endl;
        }

        if(valType_!=pTraits<Type>::typeName) {
            FatalErrorIn("tmp<Type> ExpressionResult::getObjectResult()")
                << "The expected return type " << pTraits<Type>::typeName
                    << " is different from the stored result type "
                    << valType_ << "\n" << endl
                    << exit(FatalError);
        }

        tmp<Type> result(
            dynamic_cast<Type *>(
                generalContent_.ptr()
            )
        );

        if(keep) {
            if(debug) {
                Info << "ExpressionResult: getObjectResult: make copy" << endl;
            }
            generalContent_.set(
                new Type(
                    dynamicCast<const Type &>(
                        result()
                    )
                )
            );
            if(debug) {
                Info << "ExpressionResult: getObjectResult: make copy - done" << endl;
            }
        } else {
            clearResult();
        }

        if(debug) {
            Info << "ExpressionResult: getObjectResult - done" << endl;
        }
        return result;
    }

    template<class Type>
    ExpressionResult ExpressionResult::getUniformInternal(
        const label size,
        bool noWarn
    ) const
    {
        ExpressionResult result;
        const Field<Type> &vals=*static_cast<Field<Type>*>(valPtr_);
        Type avg=gAverage(vals);

        if(!noWarn) {
            Type minVal=gMin(vals);
            Type maxVal=gMax(vals);
            if(mag(minVal-maxVal)>SMALL) {
                WarningIn("ExpressionResult::getUniformInternal(const label size,bool noWarn)")
                    << "The minimum value " << minVal << " and the maximum "
                        << maxVal << " differ. I will use the average " << avg
                        << endl;
            }
        }
        result.setResult(avg,size);
        return result;
    }

    template<template<class> class BinOp,class Type>
    inline Type ExpressionResult::getReduced(BinOp<Type> op,Type initial)
    {
        if(valType_!=pTraits<Type>::typeName) {
            FatalErrorIn("ExpressionResult::getReduced(BinOp<Type> op,Type initial)")
                << "The expected return type " << pTraits<Type>::typeName
                    << " is different from the stored result type "
                    << valType_ << "\n" << endl
                    << exit(FatalError);
        }

        Type result=initial;
        const Field<Type> &vals=*static_cast<Field<Type>*>(valPtr_);
        forAll(vals,i) {
            result=op(result,vals[i]);
        }

        reduce(result,op);

        return result;
    }

    inline const string &ExpressionResult::valueType() const
    {
        return valType_;
    }

    inline bool ExpressionResult::isPoint() const
    {
        return isPoint_;
    }

    inline bool ExpressionResult::isSingleValue() const
    {
        return isSingleValue_;
    }

    inline bool ExpressionResult::isObject() const
    {
        return generalContent_.valid();
    }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}

// ************************************************************************* //
