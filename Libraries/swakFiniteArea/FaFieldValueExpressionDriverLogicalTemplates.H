/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description


Contributors/Copyright:
    2011, 2013-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef VALUE_EXPRESSION_LOGICAL_TEMPLATES
#define VALUE_EXPRESSION_LOGICAL_TEMPLATES

namespace Foam {

template<>
tmp<areaScalarField>
FaFieldValueExpressionDriver::makeLogicalField<areaScalarField>(
    scalar val
)
{
    return makeConstantField<areaScalarField>(toBool(val));
}

template<>
tmp<edgeScalarField>
FaFieldValueExpressionDriver::makeLogicalField<edgeScalarField>(
    scalar val
)
{
    return makeConstantField<edgeScalarField>(toBool(val));
}

#define getFaceValue(ding) (ding.boundaryField()[pI][fI])

template <class Op,class Field>
tmp<Field>
FaFieldValueExpressionDriver::doCompare(
    const Field &a,
    Op op,
    const Field &b
)
{
    tmp<Field> res=makeLogicalField<Field>(0.);

    forAll(res(),cellI) {
        if( op(a[cellI],b[cellI]) ) {
            const_cast<Field&>(res())[cellI]=TRUE_Value;
        } else {
            const_cast<Field&>(res())[cellI]=FALSE_Value;
        }

    }

    forAll(res->boundaryField(),pI) {
        forAll(res->boundaryField()[pI],fI) {
            if( op(getFaceValue(a),getFaceValue(b)) ) {
                const_cast<typename Field::value_type&>(getFaceValue(res()))=TRUE_Value;
            } else {
                const_cast<typename Field::value_type&>(getFaceValue(res()))=FALSE_Value;
            }
        }
    }

    return res;
}

template <class Op,class Field>
tmp<Field> FaFieldValueExpressionDriver::doLogicalOp(
    const Field &a,
    Op op,
    const Field &b
)
{
    tmp<Field> res=makeLogicalField<Field>(0.);

    forAll(res(),cellI) {
        bool av= toBool(a[cellI]);
        bool bv= toBool(b[cellI]);

        if( op(av,bv) ) {
            const_cast<Field&>(res())[cellI]=TRUE_Value;
        } else {
            const_cast<Field&>(res())[cellI]=FALSE_Value;
        }
    }

    forAll(res->boundaryField(),pI) {
        forAll(res->boundaryField()[pI],fI) {
            bool av= toBool(getFaceValue(a));
            bool bv= toBool(getFaceValue(b));
            if( op(av,bv) ) {
                const_cast<typename Field::value_type&>(getFaceValue(res()))=TRUE_Value;
            } else {
                const_cast<typename Field::value_type&>(getFaceValue(res()))=FALSE_Value;
            }
        }
    }

    return res;
}

template <class T,class Field>
tmp<T>  FaFieldValueExpressionDriver::doConditional(
    const Field &cond,
    const T &yes,
    const T &no
)
{
    tmp<T> result=makeConstantField<T>(
        pTraits<typename T::value_type>::zero
    );

    forAll(result(),cellI) {
        bool c= toBool(cond[cellI]);
        if(c) {
            const_cast<T&>(result())[cellI]=yes[cellI];
        } else {
            const_cast<T&>(result())[cellI]=no[cellI];
        }
    }

    forAll(result->boundaryField(),pI) {
        forAll(result->boundaryField()[pI],fI) {
            bool c= toBool(getFaceValue(cond));
            if(c) {
                const_cast<typename T::value_type&>(getFaceValue(result()))=getFaceValue(yes);
            } else {
                const_cast<typename T::value_type&>(getFaceValue(result()))=getFaceValue(no);
            }
        }
    }

    return result;
}

template <class Field>
tmp<Field> FaFieldValueExpressionDriver::doLogicalNot(const Field &a)
{
    tmp<Field> res=makeLogicalField<Field>(0.);

    forAll(res(),cellI) {
        bool av= toBool(a[cellI]);

        if( !av ) {
            const_cast<typename Field::value_type&>(res()[cellI])=TRUE_Value;
        } else {
            const_cast<typename Field::value_type&>(res()[cellI])=FALSE_Value;
        }
    }

    forAll(res->boundaryField(),pI) {
        forAll(res->boundaryField()[pI],fI) {
            bool av= toBool(getFaceValue(a));
            if( !av ) {
                const_cast<typename Field::value_type&>(getFaceValue(res()))=TRUE_Value;
            } else {
                const_cast<typename Field::value_type&>(getFaceValue(res()))=FALSE_Value;
            }
        }
    }

    return res;
}

#undef getFaceValue

} // end namespace

#endif
