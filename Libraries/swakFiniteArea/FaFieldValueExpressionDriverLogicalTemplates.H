//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_LOGICAL_TEMPLATES
#define VALUE_EXPRESSION_LOGICAL_TEMPLATES

namespace Foam {

template<>
areaScalarField *FaFieldValueExpressionDriver::makeLogicalField<areaScalarField>(scalar val)
{
    return makeConstantField<areaScalarField>(val);
}

template<>
edgeScalarField *FaFieldValueExpressionDriver::makeLogicalField<edgeScalarField>(scalar val)
{
    return makeConstantField<edgeScalarField>(val);
}

#define getFaceValue(ding) (ding->boundaryField()[pI][fI])

template <class Op,class Field>
Field *FaFieldValueExpressionDriver::doCompare(Field *a,Op op,Field *b)
{
    Field *res=makeLogicalField<Field>(0.);

    forAll(*res,cellI) {
        if( op((*a)[cellI],(*b)[cellI]) ) {
            (*res)[cellI]=1;
        } else {
            (*res)[cellI]=0;
        }
    }

    forAll(res->boundaryField(),pI) {
        forAll(res->boundaryField()[pI],fI) {
            if( op(getFaceValue(a),getFaceValue(b)) ) {
                getFaceValue(res)=1;
            } else {
                getFaceValue(res)=0;
            }
        }
    }

    return res;
}

template <class Op,class Field>
Field *FaFieldValueExpressionDriver::doLogicalOp(Field *a,Op op,Field *b)
{
    Field *res=makeLogicalField<Field>(0.);

    forAll(*res,cellI) {
        bool av= (*a)[cellI] ? true : false;
        bool bv= (*b)[cellI] ? true : false;

        if( op(av,bv) ) {
            (*res)[cellI]=1;
        } else {
            (*res)[cellI]=0;
        }
    }

    forAll(res->boundaryField(),pI) {
        forAll(res->boundaryField()[pI],fI) {
            bool av= getFaceValue(a) ? true : false;
            bool bv= getFaceValue(b) ? true : false;
            if( op(av,bv) ) {
                getFaceValue(res)=1;
            } else {
                getFaceValue(res)=0;
            }
        }
    }

    return res;
}

template <class T,class Field>
T * FaFieldValueExpressionDriver::doConditional(Field *cond,T* yes,T* no,T* result)
{
    forAll(*result,cellI) {
        bool c= (*cond)[cellI] ? true : false;
        if(c) {
            (*result)[cellI]=(*yes)[cellI];
        } else {
            (*result)[cellI]=(*no)[cellI];
        }
    }

    forAll(result->boundaryField(),pI) {
        forAll(result->boundaryField()[pI],fI) {
            bool c= getFaceValue(cond) ? true : false;
        if(c) {
            getFaceValue(result)=getFaceValue(yes);
        } else {
            getFaceValue(result)=getFaceValue(no);
        }
        }
    }

    return result;
}

template <class Field>
Field *FaFieldValueExpressionDriver::doLogicalNot(Field *a)
{
    Field *res=makeLogicalField<Field>(0.);

    forAll(*res,cellI) {
        bool av= (*a)[cellI] ? true : false;

        if( !av ) {
            (*res)[cellI]=1;
        } else {
            (*res)[cellI]=0;
        }
    }

    forAll(res->boundaryField(),pI) {
        forAll(res->boundaryField()[pI],fI) {
            bool av= getFaceValue(a) ? true : false;
            if( !av ) {
                getFaceValue(res)=1;
            } else {
                getFaceValue(res)=0;
            }
        }
    }

    return res;
}

#undef getFaceValue

} // end namespace

#endif
