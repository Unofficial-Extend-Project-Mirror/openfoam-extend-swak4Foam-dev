//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "wallFvPatch.H"
#include "fixedValueFvPatchFields.H"
#include "zeroGradientFvPatchFields.H"

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
            typeid(patch)==typeid(fvPatch
            )
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

} // end namespace

#endif
