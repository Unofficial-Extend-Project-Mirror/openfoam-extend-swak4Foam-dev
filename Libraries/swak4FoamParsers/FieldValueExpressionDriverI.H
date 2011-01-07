//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "wallFvPatch.H"
#include "fixedValueFvPatchFields.H"
#include "zeroGradientFvPatchFields.H"

namespace Foam {

template<class T>
T *FieldValueExpressionDriver::getField(const string &name)
{
    if(debug) {
        Info << "FieldValueExpressionDriver::getField. Name: " << name 
            << " Type: " << T::typeName << endl;
    }

    dimensionSet nullDim(0,0,0,0,0);

    T *f=NULL;

    if(variables_.found(name)) {
        if(debug) {
            Info << "Getting " << name << " from variables" << endl;
        }

        typedef typename T::value_type Type;

        f=new T(
            IOobject
            (
                name,
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh_,
            dimensioned<Type>(name,nullDim,pTraits<Type>::zero)
        );

        Field<Type> vals=variables_[name].getResult<Type>(true);

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
                    WarningIn("FieldValueExpressionDriver::getField(const string &name)")
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
        mesh_.foundObject<T>(name)
    ) {
        if(debug) {
            Info << "Getting " << name << " from memory" << endl;
        }

        f=new T(
            mesh_.lookupObject<T>(name)
        );
    } else if(
        searchOnDisc()
    ) {
        if(debug) {
            Info << "Reading " << name << " from disc" << endl;
        }
        
        f=this->readAndRegister<T>(name).ptr();
    }

    if(f==NULL) {
        FatalErrorIn("FieldValueExpressionDriver::getField(const string &name)")
            << "Could not find the field " << name 
                << " in memory or on disc" << endl
                << abort(FatalError);
    }
    
    f->dimensions().reset(nullDim);

    return f;
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
