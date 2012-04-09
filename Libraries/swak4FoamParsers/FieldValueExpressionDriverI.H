//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "wallFvPatch.H"
#include "wallPointPatch.H"
#include "fixedValueFvPatchFields.H"
#include "fixedValueFvsPatchFields.H"
#include "zeroGradientFvPatchFields.H"
#include "calculatedFvPatchFields.H"
#include "calculatedFvsPatchFields.H"

#include "volPointInterpolation.H"
#include "interpolatePointToCell.H"

#ifdef FOAM_DEV
#include "FixedValuePointPatchField.H"
#include "ZeroGradientPointPatchField.H"
#include "CalculatedPointPatchField.H"
#else
#include "fixedValuePointPatchFields.H"
#include "zeroGradientPointPatchFields.H"
#include "calculatedPointPatchFields.H"
#endif

namespace Foam {

template<class T>
T* FieldValueExpressionDriver::getField(const string &name,bool getOldTime)
{
    return this->getOrReadField<T>(
        name,
        true,  // fail if not found
        getOldTime
    ).ptr();
}

template<class T>
T* FieldValueExpressionDriver::getPointField(const string &name,bool getOldTime)
{
    return this->getOrReadPointField<T>(
        name,
        true,  // fail if not found
        getOldTime
    ).ptr();
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

// code duplication from the template above
template<class T>
void FieldValueExpressionDriver::makePatches
(
    GeometricField<T,pointPatchField,pointMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,pointPatchField,pointMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<pointPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const pointPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(wallPointPatch)
//             || 
//             typeid(patch)==typeid(fvPatch)
        )
    ) {
        if(isValuePatch){
#ifdef FOAM_DEV
            bfNew[patchI]=new FixedValuePointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,T>(patch,field);  
#else
            bfNew[patchI]=new fixedValuePointPatchField<T>(patch,field);  
#endif
        } else {
#ifdef FOAM_DEV
            bfNew[patchI]=new ZeroGradientPointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,T>(patch,field);  
#else
            bfNew[patchI]=new zeroGradientPointPatchField<T>(patch,field);
#endif
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
void FieldValueExpressionDriver::setCalculatedPatches
(
    GeometricField<T,pointPatchField,pointMesh> &field,
    T unusedValue
) {
    typename GeometricField<T,pointPatchField,pointMesh>::GeometricBoundaryField &bf=field.boundaryField();
    forAll(bf,patchI) {
        pointPatchField<T> &pf=bf[patchI];
        
        if(
#ifdef FOAM_DEV
            typeid(pf)==typeid(CalculatedPointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,T>)
#else
            typeid(pf)==typeid(calculatedPointPatchField<T>)
#endif
        ) {
            pf==pf.patchInternalField();
        }
    }
    field.correctBoundaryConditions();
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
void FieldValueExpressionDriver::copyCalculatedPatches
(
    GeometricField<T,pointPatchField,pointMesh> &field,
    const GeometricField<T,pointPatchField,pointMesh> &orig
) {
    typename GeometricField<T,pointPatchField,pointMesh>::GeometricBoundaryField &bf=field.boundaryField();
    List<pointPatchField<T> *>bfNew(bf.size());
    
    forAll(bf,patchI) {
        pointPatchField<T> &pf=bf[patchI];
        
        if(
#ifdef FOAM_DEV
            typeid(pf)==typeid(CalculatedPointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,T>)
#else
            typeid(pf)==typeid(calculatedPointPatchField<T>)
#endif
        ) {
            pf==pf.patchInternalField();
        }
    }
    //    field.correctBoundaryConditions();
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
// Code duplication from above ... maybe there is a better way
template<class T>
void FieldValueExpressionDriver::setValuePatches
(
    GeometricField<T,pointPatchField,pointMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,pointPatchField,pointMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<pointPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const pointPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(wallPointPatch)
//             ||
//             typeid(patch)==typeid(fvPatch
        )
    ) {
        if(
#ifdef FOAM_DEV
            typeid(field.boundaryField()[patchI])==typeid(FixedValuePointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,T>)
#else
            typeid(field.boundaryField()[patchI])==typeid(fixedValuePointPatchField<T>)
#endif
        ) {
            pointPatchField<T> &pf=field.boundaryField()[patchI];
            
            pf==pf.patchInternalField();
        }
    }
  }
}

template<class FType>
inline FType *FieldValueExpressionDriver::makeField(const Field<typename FType::value_type> &val)
{
    return makeFieldInternal<FType>(
        val,
        this->mesh()
    );
}
    
template<class FType>
inline FType *FieldValueExpressionDriver::makePointField(const Field<typename FType::value_type> &val)
{
    return makeFieldInternal<FType>(
        val,
        this->pMesh()
    );
}
    
template<class FType,class Mesh>
inline FType *FieldValueExpressionDriver::makeFieldInternal(
    const Field<typename FType::value_type> &val,
    const Mesh &actualMesh
)
{
    std::ostringstream buff;
    buff << "field" << pTraits<typename FType::value_type>::typeName;

    FType *f=new FType(
        IOobject
        (
            buff.str(),
            this->time(),
            this->mesh(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        actualMesh,
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
inline FType *FieldValueExpressionDriver::makeConstantField(
    const typename FType::value_type &val
)
{
    return makeConstantFieldInternal<FType>(val,mesh_);
}

template<class FType>
inline FType *FieldValueExpressionDriver::makePointConstantField(
    const typename FType::value_type &val
)
{
    return makeConstantFieldInternal<FType>(val,this->pMesh());
}

template<class FType,class Mesh>
inline FType *FieldValueExpressionDriver::makeConstantFieldInternal(
    const typename FType::value_type &val,
    const Mesh &mesh
)
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
        mesh,
        val,
        defaultBC
    );
    if(pTraits<typename FType::PatchFieldType>::typeName=="fvsPatchField") {
        setCalculatedPatches(*f,val);
    }

    return f;
}

template<class T>
void FieldValueExpressionDriver::setResult(T *r,bool isSurfaceField,bool isPointField) 
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
    isPointField_=isPointField;
}

template<class T>
void FieldValueExpressionDriver::setLogicalResult(T *r,bool isSurfaceField,bool isPointField) 
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
    isPointField_=isPointField;
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

template<class Type>
inline autoPtr<GeometricField<Type,pointPatchField,pointMesh> >
FieldValueExpressionDriver::cellToPointInterpolate(
    const GeometricField<Type,fvPatchField,volMesh> &field
) {
    typedef GeometricField<Type,pointPatchField,pointMesh> rType;

    volPointInterpolation interpol(this->mesh());
    return autoPtr<rType>(interpol.interpolate(field).ptr());
}

template<class Type>
inline autoPtr<GeometricField<Type,fvPatchField,volMesh> >
FieldValueExpressionDriver::pointToCellInterpolate(
    const GeometricField<Type,pointPatchField,pointMesh> &field
) {
    typedef GeometricField<Type,fvPatchField,volMesh> rType;
    autoPtr<rType> result(
        this->makeConstantField<rType>(pTraits<Type>::zero)
    );
    forAll(result(),cellI) {
        result()[cellI]=interpolatePointToCell(field,cellI);
    }
    result->correctBoundaryConditions();

    return result;
}


} // end namespace

#endif
