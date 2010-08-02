//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_LOGICAL_TEMPLATES
#define VALUE_EXPRESSION_LOGICAL_TEMPLATES

template<>
Foam::volScalarField *FieldValueExpressionDriver::makeLogicalField<Foam::volScalarField>(Foam::scalar val)
{
    return makeScalarField(val);
}

template<>
Foam::surfaceScalarField *FieldValueExpressionDriver::makeLogicalField<Foam::surfaceScalarField>(Foam::scalar val)
{
    return makeSurfaceScalarField(val);
}

// template<class M>
// Foam::label upperLimit(M &f);

// template<>
// inline Foam::label upperLimit(Foam::volScalarField &f)
// {
//     return f.mesh().nCells();
// }

// template<>
// inline Foam::label upperLimit(Foam::surfaceScalarField &f)
// {
//     return f.mesh().nFaces();
// }

// template<>
// inline Foam::label upperLimit(Foam::volVectorField &f)
// {
//     return f.mesh().nCells();
// }

// template<>
// inline Foam::label upperLimit(Foam::surfaceVectorField &f)
// {
//     return f.mesh().nFaces();
// }

#define getFaceValue(ding) (ding->boundaryField()[pI][fI])

template <class Op,class Field>
Field *FieldValueExpressionDriver::doCompare(Field *a,Op op,Field *b)
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
Field *FieldValueExpressionDriver::doLogicalOp(Field *a,Op op,Field *b)
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
T * FieldValueExpressionDriver::doConditional(Field *cond,T* yes,T* no,T* result)
{
    forAll(*result,cellI) {
    //    for(Foam::label cellI=0;cellI<upperLimit(*result);cellI++) {
        bool c= (*cond)[cellI] ? true : false;
        if(c) {
            (*result)[cellI]=(*yes)[cellI];
        } else {
            (*result)[cellI]=(*no)[cellI];
        }
    }
    //    (*result)=(*yes)*pos((*cond)-0.5)+(*no)*((*cond)-0.5);

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
Field *FieldValueExpressionDriver::doLogicalNot(Field *a)
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


#endif
