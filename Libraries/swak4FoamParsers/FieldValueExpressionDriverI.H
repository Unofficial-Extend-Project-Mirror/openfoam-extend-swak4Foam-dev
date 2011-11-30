//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "wallFvPatch.H"
#include "fixedValueFvPatchFields.H"
#include "fixedValueFvsPatchFields.H"
#include "zeroGradientFvPatchFields.H"
#include "calculatedFvPatchFields.H"
#include "calculatedFvsPatchFields.H"

namespace Foam {

template<class T>
T* FieldValueExpressionDriver::getField(const string &name)
{
    return this->getOrReadField<T>(name).ptr();
}

inline label FieldValueExpressionDriver::size() const
{
    return mesh_.nCells();
}

inline label FieldValueExpressionDriver::pointSize() const
{
    return mesh_.nPoints();
}

inline const fvMesh &FieldValueExpressionDriver::mesh() const
{
    return mesh_;
}

template<class T>
void FieldValueExpressionDriver::makePatches
(
    GeometricField<T,fvPatchField,volMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,fvPatchField,volMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<fvPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const fvPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(wallFvPatch)
            || 
            typeid(patch)==typeid(fvPatch)
        )
    ) {
        if(isValuePatch){
            bfNew[patchI]=new fixedValueFvPatchField<T>(patch,field);  
        } else {
            bfNew[patchI]=new zeroGradientFvPatchField<T>(patch,field);      
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

// code duplication from the template above
template<class T>
void FieldValueExpressionDriver::makePatches
(
    GeometricField<T,fvsPatchField,surfaceMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,fvsPatchField,surfaceMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<fvsPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const fvPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(wallFvPatch)
            || 
            typeid(patch)==typeid(fvPatch)
        )
    ) {
        if(isValuePatch){
            bfNew[patchI]=new fixedValueFvsPatchField<T>(patch,field);  
        } else {
            // this is the only line that is REALLY different
            bfNew[patchI]=new calculatedFvsPatchField<T>(patch,field);
            (*bfNew[patchI])==bf[patchI];
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
void FieldValueExpressionDriver::setCalculatedPatches
(
    GeometricField<T,fvPatchField,volMesh> &field,
    T unusedValue
) {
    typename GeometricField<T,fvPatchField,volMesh>::GeometricBoundaryField &bf=field.boundaryField();
    List<fvPatchField<T> *>bfNew(bf.size());
    
    forAll(bf,patchI) {
        fvPatchField<T> &pf=bf[patchI];
        
        if(
            typeid(pf)==typeid(calculatedFvPatchField<T>)
        ) {
            pf==pf.patchInternalField();
        }
    }
    field.correctBoundaryConditions();
}

template<class T>
void FieldValueExpressionDriver::setCalculatedPatches
(
    GeometricField<T,fvsPatchField,surfaceMesh> &field,
    T value
) {
    typename GeometricField<T,fvsPatchField,surfaceMesh>::GeometricBoundaryField &bf=field.boundaryField();
    List<fvsPatchField<T> *>bfNew(bf.size());
    
    forAll(bf,patchI) {
        fvsPatchField<T> &pf=bf[patchI];
        
        if(
            typeid(pf)==typeid(calculatedFvsPatchField<T>)
        ) {
            //            pf==pf.patchInternalField();
            pf==value;
        }
    }
    //     field.correctBoundaryConditions();
}

template<class T>
void FieldValueExpressionDriver::copyCalculatedPatches
(
    GeometricField<T,fvPatchField,volMesh> &field,
    const GeometricField<T,fvPatchField,volMesh> &orig
) {
    typename GeometricField<T,fvPatchField,volMesh>::GeometricBoundaryField &bf=field.boundaryField();
    List<fvPatchField<T> *>bfNew(bf.size());
    
    forAll(bf,patchI) {
        fvPatchField<T> &pf=bf[patchI];
        
        if(
            typeid(pf)==typeid(calculatedFvPatchField<T>)
        ) {
            pf==pf.patchInternalField();
        }
    }
    //    field.correctBoundaryConditions();
}

template<class T>
void FieldValueExpressionDriver::copyCalculatedPatches
(
    GeometricField<T,fvsPatchField,surfaceMesh> &field,
    const GeometricField<T,fvsPatchField,surfaceMesh> &orig
) {
    typename GeometricField<T,fvsPatchField,surfaceMesh>::GeometricBoundaryField &bf=field.boundaryField();
    const typename GeometricField<T,fvsPatchField,surfaceMesh>::GeometricBoundaryField &bfOrig=orig.boundaryField();

    forAll(bf,patchI) {
        fvsPatchField<T> &pf=bf[patchI];
        const fvsPatchField<T> &pfOrig=bfOrig[patchI];
        
        if(
            typeid(pf)==typeid(calculatedFvsPatchField<T>)
            &&
            typeid(pfOrig)==typeid(calculatedFvsPatchField<T>)
        ) {
            pf==pfOrig;
        }
    }
}

template<class T>
void FieldValueExpressionDriver::setValuePatches
(
    GeometricField<T,fvPatchField,volMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,fvPatchField,volMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<fvPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const fvPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(wallFvPatch)
            ||
            typeid(patch)==typeid(fvPatch
            )
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(fixedValueFvPatchField<T>)) {
            fvPatchField<T> &pf=field.boundaryField()[patchI];
            
            pf==pf.patchInternalField();
        }
    }
  }
}

// Code duplication from above ... maybe there is a better way
template<class T>
void FieldValueExpressionDriver::setValuePatches
(
    GeometricField<T,fvsPatchField,surfaceMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,fvsPatchField,surfaceMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<fvsPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const fvPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(wallFvPatch)
            ||
            typeid(patch)==typeid(fvPatch
            )
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(fixedValueFvsPatchField<T>)) {
            fvsPatchField<T> &pf=field.boundaryField()[patchI];
            //            pf==pf.patchInternalField();
            WarningIn(
                "FieldValueExpressionDriver::setValuePatches"
                "("
                "GeometricField<T,fvsPatchField,surfaceMesh> &field,"
                "bool keepPatches,"
                "const wordList &fixedPatches"
                ")" 
            ) << "There is no patchInternalField() for fvsPatchField. " 
                << "Nothing done for patch " << patch.name() 
                << " but setting it to " << pTraits<T>::zero << endl;
            pf==pTraits<T>::zero;
        }
    }
  }
}

template<class FType>
inline FType *FieldValueExpressionDriver::makeField(const Field<typename FType::value_type> &val)
{
    std::ostringstream buff;
    buff << "field" << pTraits<typename FType::value_type>::typeName;

    FType *f=new FType(
        IOobject
        (
            buff.str(),
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        pTraits<typename FType::value_type>::zero,
        "zeroGradient"
    );

    if(val.size()!=f->internalField().size()) {
        FatalErrorIn("FieldValueExpressionDriver::makeField(const Field<typename FType::value_type> &val)")
            << "Size " << val.size() << " of the assigned field is not the required "
                << f->internalField().size()
                << endl
                << exit(FatalError);
    }

    f->internalField()=val;

    correctField(*f);

    return f;
}

template<class FType>
inline FType *FieldValueExpressionDriver::makeConstantField(const typename FType::value_type &val)
{
    std::ostringstream buff;
    buff << "constant" << pTraits<typename FType::value_type>::typeName;

    word defaultBC="calculated";
    if(pTraits<typename FType::PatchFieldType>::typeName=="fvPatchField") {
        defaultBC="zeroGradient";
    }

    FType *f=new FType(
        IOobject
        (
            buff.str(),
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        val,
        defaultBC
    );
    if(pTraits<typename FType::PatchFieldType>::typeName=="fvsPatchField") {
        setCalculatedPatches(*f,val);
    }

    return f;
}

template<class T>
void FieldValueExpressionDriver::setResult(T *r,bool isSurfaceField) 
{
    resultField_.reset(r);

    //    T &result=dynamicCast<T &>(resultField_()); // doesn't work with gcc 4.2

    T &result=dynamic_cast<T &>(resultField_());

    if(!resultDimension_.dimensionless()) {
        result.dimensions().reset(resultDimension_);
    }

    typ_=pTraits<T>::typeName;

    result_.setResult(result.internalField());

    isLogical_=false;
    isSurfaceField_=isSurfaceField;
}

template<class T>
void FieldValueExpressionDriver::setLogicalResult(T *r,bool isSurfaceField) 
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

    result_.setResult(yesOrNo);

    isLogical_=true;
    isSurfaceField_=isSurfaceField;
}

template<class T>
const T &FieldValueExpressionDriver::getResult() 
{
    if(!resultField_.valid()) {
        FatalErrorIn("FieldValueExpressionDriver::getResult()")
            << "When asking for a " << pTraits<T>::typeName
                << ": No result present"
                << endl
                << exit(FatalError);
    }

    //    return dynamicCast<const T &>(resultField_()); // doesn't work with gcc 4.2
    return dynamic_cast<const T &>(resultField_());
}

template<class T>
bool FieldValueExpressionDriver::resultIsTyp(bool isLogical) 
{
    if(!resultField_.valid()) {
        FatalErrorIn("FieldValueExpressionDriver::isTyp(bool isLogical)")
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
