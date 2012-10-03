//  ICE Revision: $Id$

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "fixedValueFaPatchFields.H"
#include "fixedValueFaePatchFields.H"
#include "zeroGradientFaPatchFields.H"

namespace Foam {

template<class T>
autoPtr<T> FaFieldValueExpressionDriver::getField(
    const string &name,
    bool getOldTime
)
{
    if(debug) {
        Info << "FaFieldValueExpressionDriver::getField. Name: " << name
            << " Type: " << T::typeName << endl;
    }

    dimensionSet nullDim(0,0,0,0,0);

    autoPtr<T> f;

    typedef typename T::value_type Type;

    if(
        hasVariable(name)
        ||
        isGlobal<Type>(name,false)
    ) {
        if(debug) {
            Info << "Getting " << name << " from variables" << endl;
        }

        f.set(
            new T(
                IOobject
                (
                    name,
                    this->time(),
                    this->mesh(),
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                aMesh(),
                dimensioned<Type>(name,nullDim,pTraits<Type>::zero)
            )
        );

        Field<Type> vals;
        if(hasVariable(name)) {
            vals=variable(name).getResult<Type>(true);
        } else {
            vals=const_cast<ExpressionResult &>(
                lookupGlobal(name)
            ).getResult<Type>(true);
        }

        if(debug) {
            Pout << "sizes: " << vals.size() << " " << f->size() << endl;
        }
        if(vals.size()==f->size()) {
            f->internalField()=vals;
        } else {
            Type avg=gAverage(vals);

            bool noWarn=false;

            if(!noWarn) {
                Type minVal=gMin(vals);
                Type maxVal=gMax(vals);
                if(mag(minVal-maxVal)>SMALL) {
                    WarningIn("FaFieldValueExpressionDriver::getField(const string &name)")
                        << "The minimum value " << minVal << " and the maximum "
                            << maxVal << " differ. I will use the average " << avg
                            << endl;
                }
            }

            f->internalField()=avg;
        }
        return f;
    }

    if(
        searchInMemory()
        &&
        this->mesh().foundObject<T>(name)
    ) {
        if(debug) {
            Info << "Getting " << name << " from memory" << endl;
        }

        f.set(
            new T(
                this->mesh().lookupObject<T>(name)
            )
        );

        if(getOldTime) {
            if(debug) {
                Info << "Getting oldTime of " << name
                    << " has " << this->mesh().lookupObject<T>(name).nOldTimes()
                    << endl;
            }
            if(
                this->mesh().lookupObject<T>(name).nOldTimes()==0
                &&
                this->prevIterIsOldTime()
            ) {
                if(debug) {
                    Info << "Using the previous iteration, because there is no oldTime" << endl;
                }
                f->oldTime()=this->mesh().lookupObject<T>(name).prevIter();
            }
        }
    } else if(
        searchOnDisc()
    ) {
        if(debug) {
            Info << "Reading " << name << " from disc" << endl;
        }

        f.set(
            this->readAndRegister<T>(name).ptr()
        );
    }

    if(!f.valid()) {
        FatalErrorIn("FaFieldValueExpressionDriver::getField(const string &name)")
            << "Could not find the field " << name
                << " in memory or on disc" << endl
                << exit(FatalError);
    }

    f->dimensions().reset(nullDim);
    if(f->nOldTimes()>0) {
        if(debug) {
            Info << "Removing dimensions of oldTime of " << name
                << " has " << f->nOldTimes()
                << endl;
        }

        // go through ALL old times
        T *fp=f.operator->();
        while(fp->nOldTimes()>0) {
            fp=&(fp->oldTime());
            fp->dimensions().reset(nullDim);
        }
    }

    return f;
}

template<class T>
void FaFieldValueExpressionDriver::makePatches
(
    GeometricField<T,faPatchField,areaMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,faPatchField,areaMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<faPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const faPatch &patch=bf[patchI].patch();

    bool isValuePatch=false;
    forAll(fixedPatches,i) {
      if(fixedPatches[i]==patch.name()) {
	isValuePatch=true;
      }
    }

    if(
        (
            !keepPatches
            ||
            isValuePatch
        )
        &&
        (
            typeid(patch)==typeid(faPatch)
        )
    ) {
        if(isValuePatch){
            bfNew[patchI]=new fixedValueFaPatchField<T>(patch,field);
        } else {
            bfNew[patchI]=new zeroGradientFaPatchField<T>(patch,field);
        }
    } else {
        bfNew[patchI]=bf[patchI].clone().ptr();
    }
  }

  bf.clear();
  bf.setSize(bfNew.size());
  forAll(bf,i) {
      bf.set(i,bfNew[i]);
  }
}

template<class T>
void FaFieldValueExpressionDriver::makePatches
(
    GeometricField<T,faePatchField,edgeMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,faePatchField,edgeMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<faePatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const faPatch &patch=bf[patchI].patch();

    bool isValuePatch=false;
    forAll(fixedPatches,i) {
      if(fixedPatches[i]==patch.name()) {
	isValuePatch=true;
      }
    }

    if(
        (
            !keepPatches
            ||
            isValuePatch
        )
        &&
        (
            typeid(patch)==typeid(faPatch)
        )
    ) {
        if(isValuePatch){
            bfNew[patchI]=new fixedValueFaePatchField<T>(patch,field);
        } else {
            // this is different because there is no zeroGradient for edge fields
            bfNew[patchI]=new calculatedFaePatchField<T>(patch,field);
        }
    } else {
        bfNew[patchI]=bf[patchI].clone().ptr();
    }
  }

  bf.clear();
  bf.setSize(bfNew.size());
  forAll(bf,i) {
      bf.set(i,bfNew[i]);
  }
}


template<class T>
void FaFieldValueExpressionDriver::setCalculatedPatches
(
    GeometricField<T,faPatchField,areaMesh> &field,
    T unusedValue
) {
    typename GeometricField<T,faPatchField,areaMesh>::GeometricBoundaryField &bf=field.boundaryField();

    forAll(bf,patchI) {
        faPatchField<T> &pf=bf[patchI];

        if(
            typeid(pf)==typeid(calculatedFaPatchField<T>)
        ) {
            pf==pf.patchInternalField();
        }
    }
    field.correctBoundaryConditions();
}

template<class T>
void FaFieldValueExpressionDriver::setCalculatedPatches
(
    GeometricField<T,faePatchField,edgeMesh> &field,
    T value
) {
    typename GeometricField<T,faePatchField,edgeMesh>::GeometricBoundaryField &bf=field.boundaryField();

    forAll(bf,patchI) {
        faePatchField<T> &pf=bf[patchI];

        if(
            typeid(pf)==typeid(calculatedFaePatchField<T>)
        ) {
            //            pf==pf.patchInternalField();
            pf==value;
        }
    }
    //     field.correctBoundaryConditions();
}

template<class T>
void FaFieldValueExpressionDriver::copyCalculatedPatches
(
    GeometricField<T,faPatchField,areaMesh> &field,
    const GeometricField<T,faPatchField,areaMesh> &orig
) {
    typename GeometricField<T,faPatchField,areaMesh>::GeometricBoundaryField &bf=field.boundaryField();
    List<faPatchField<T> *>bfNew(bf.size());

    forAll(bf,patchI) {
        faPatchField<T> &pf=bf[patchI];

        if(
            typeid(pf)==typeid(calculatedFaPatchField<T>)
        ) {
            pf==pf.patchInternalField();
        }
    }
    //    field.correctBoundaryConditions();
}

template<class T>
void FaFieldValueExpressionDriver::copyCalculatedPatches
(
    GeometricField<T,faePatchField,edgeMesh> &field,
    const GeometricField<T,faePatchField,edgeMesh> &orig
) {
    typename GeometricField<T,faePatchField,edgeMesh>::GeometricBoundaryField &bf=field.boundaryField();
    const typename GeometricField<T,faePatchField,edgeMesh>::GeometricBoundaryField &bfOrig=orig.boundaryField();

    forAll(bf,patchI) {
        faePatchField<T> &pf=bf[patchI];
        const faePatchField<T> &pfOrig=bfOrig[patchI];

        if(
            typeid(pf)==typeid(calculatedFaePatchField<T>)
            &&
            typeid(pfOrig)==typeid(calculatedFaePatchField<T>)
        ) {
            pf==pfOrig;
        }
    }
}

template<class T>
void FaFieldValueExpressionDriver::setValuePatches
(
    GeometricField<T,faPatchField,areaMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,faPatchField,areaMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<faPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const faPatch &patch=bf[patchI].patch();

    bool isValuePatch=false;
    forAll(fixedPatches,i) {
      if(fixedPatches[i]==patch.name()) {
	isValuePatch=true;
      }
    }

    if(
        (
            !keepPatches
            ||
            isValuePatch
        )
        &&
        (
            typeid(patch)==typeid(faPatch)
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(fixedValueFaPatchField<T>)) {
            faPatchField<T> &pf=field.boundaryField()[patchI];

            pf==pf.patchInternalField();
        }
    }
  }
}

template<class T>
void FaFieldValueExpressionDriver::setValuePatches
(
    GeometricField<T,faePatchField,edgeMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,faePatchField,edgeMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<faePatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const faPatch &patch=bf[patchI].patch();

    bool isValuePatch=false;
    forAll(fixedPatches,i) {
      if(fixedPatches[i]==patch.name()) {
	isValuePatch=true;
      }
    }

    if(
        (
            !keepPatches
            ||
            isValuePatch
        )
        &&
        (
            typeid(patch)==typeid(faPatch)
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(fixedValueFaePatchField<T>)) {
            faePatchField<T> &pf=field.boundaryField()[patchI];
            //            pf==pf.patchInternalField();
            WarningIn(
                "FaFieldValueExpressionDriver::setValuePatches"
                "("
                "GeometricField<T,faePatchField,edgeMesh> &field,"
                "bool keepPatches,"
                "const wordList &fixedPatches"
                ")"
            ) << "There is no patchInternalField() for faePatchField. "
                << "Nothing done for patch " << patch.name()
                << " but setting it to " << pTraits<T>::zero << endl;
            pf==pTraits<T>::zero;
        }
    }
  }
}

template<class FType>
inline FType *FaFieldValueExpressionDriver::makeField(const Field<typename FType::value_type> &val)
{
    std::ostringstream buff;
    buff << "field" << pTraits<typename FType::value_type>::typeName;

    FType *f=new FType(
        IOobject
        (
            buff.str(),
            time(),
            mesh(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        aMesh(),
        pTraits<typename FType::value_type>::zero,
        "zeroGradient"
    );

    if(val.size()!=f->internalField().size()) {
        FatalErrorIn("FaFieldValueExpressionDriver::makeField(const Field<typename FType::value_type> &val)")
            << "Size " << val.size() << " of the assigned field is not the required "
                << f->internalField().size()
                << endl
                << exit(FatalError);
    }

    f->internalField()=val;

    return f;
}

template<class FType>
inline FType *FaFieldValueExpressionDriver::makeConstantField(const typename FType::value_type &val)
{
    std::ostringstream buff;
    buff << "constant" << pTraits<typename FType::value_type>::typeName;

    FType *f=new FType(
        IOobject
        (
            buff.str(),
            time(),
            mesh(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        aMesh(),
        val,
        "zeroGradient"
    );

    return f;
}

template<class T>
void FaFieldValueExpressionDriver::setResult(T *r,bool isSurfaceField)
{
    resultField_.reset(r);

    //    T &result=dynamicCast<T &>(resultField_()); // doesn't work with gcc 4.2

    T &result=dynamic_cast<T &>(resultField_());

    if(!resultDimension_.dimensionless()) {
        result.dimensions().reset(resultDimension_);
    }

    typ_=pTraits<T>::typeName;

    this->result().setResult(result.internalField());

    isLogical_=false;
    isSurfaceField_=isSurfaceField;
}

template<class T>
void FaFieldValueExpressionDriver::setLogicalResult(T *r,bool isSurfaceField)
{
    resultField_.reset(r);

    //    T &result=dynamicCast<T &>(resultField_()); // doesn't work with gcc 4.2

    T &result=dynamic_cast<T &>(resultField_());

    if(!resultDimension_.dimensionless()) {
        result.dimensions().reset(resultDimension_);
    }

    typ_=pTraits<bool>::typeName;

    Field<bool> yesOrNo(result.internalField().size());
    forAll(yesOrNo,i) {
        yesOrNo[i]=mag(result.internalField()[i])>SMALL;
    }

    this->result().setResult(yesOrNo);

    isLogical_=true;
    isSurfaceField_=isSurfaceField;
}

template<class T> struct FaFieldValueExpressionDriver::correctBC {
    inline void operator()(const T &val)
        {
            if(debug) {
                Info << "No way to correct BC for type "
                    << pTraits<T>:: typeName << endl;
            }
            // Doing nothing
        }
};

template<class T> struct FaFieldValueExpressionDriver::correctBC<GeometricField<T,faPatchField,areaMesh> > {

    typedef GeometricField<T,faPatchField,areaMesh> theType;

    inline void operator()(const theType &val)
        {
            if(debug) {
                Info << "Correcting BC for " << val.name() << " of type "
                    << pTraits<theType>::typeName
                    << endl;
            }
            const_cast<theType&>(
                val
            ).correctBoundaryConditions();
        }
};

template<class T>
const T &FaFieldValueExpressionDriver::getResult() const
{
    if(!resultField_.valid()) {
        FatalErrorIn("FaFieldValueExpressionDriver::getResult()")
            << "When asking for a " << pTraits<T>::typeName
                << ": No result present"
                << endl
                << exit(FatalError);
    }

    //    return dynamicCast<const T &>(resultField_()); // doesn't work with gcc 4.2
    const T &result=dynamic_cast<const T &>(resultField_());
    correctBC<T>()(result);

    return result;
}

template<class T>
bool FaFieldValueExpressionDriver::resultIsTyp(bool isLogical)
{
    if(!resultField_.valid()) {
        FatalErrorIn("FaFieldValueExpressionDriver::isTyp(bool isLogical)")
            << "When asking for a " << pTraits<T>::typeName
                << ": No result present"
                << endl
                << exit(FatalError);
    }

    return (
        resultField_().type() == pTraits<T>::typeName
        &&
        isLogical == isLogical_
    );
}

} // end namespace

#endif
