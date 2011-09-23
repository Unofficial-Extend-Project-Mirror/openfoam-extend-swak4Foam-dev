//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "wallFvPatch.H"
#include "fixedValueFvPatchFields.H"
#include "zeroGradientFvPatchFields.H"
#include "calculatedFvPatchFields.H"

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
void FieldValueExpressionDriver::setCalculatedPatches
(
    GeometricField<T,fvPatchField,volMesh> &field
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
                << abort(FatalError);
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
        "zeroGradient"
    );

    return f;
}

} // end namespace

#endif
