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
    void ExpressionResult::setResult(const Field<Type> &val,bool isPoint)
    {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(new Field<Type>(val));

        isPoint_=isPoint;
    }

    template<class Type>
    void ExpressionResult::setResult(Field<Type> *val,bool isPoint)
    {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(val);

        isPoint_=isPoint;
    }

    template<class Type>
    void ExpressionResult::setResult(Type val,label size)
    {
        clearResult();

        valType_=pTraits<Type>::typeName;
        valPtr_=static_cast<void *>(new Field<Type>(size,val));

        isPoint_=false;
    }

    template<class Type>
    tmp<Field<Type> > ExpressionResult::getResult(bool keep)
    {
        if(valType_!=pTraits<Type>::typeName) {
            FatalErrorIn("tmp<Field<Type> > ExpressionResult::getResult()")
                << "The expected return type " << pTraits<Type>::typeName 
                    << " is different from the stored result type "
                    << valType_ << "\n" << endl
                    << abort(FatalError);
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
    ExpressionResult ExpressionResult::getUniformInternal(const label size,bool noWarn)
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

    inline const string &ExpressionResult::type() const
    {
        return valType_;
    }

    inline bool ExpressionResult::isPoint() const
    {
        return isPoint_;
    }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}

// ************************************************************************* //
