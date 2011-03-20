//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

#include "fixedValueFaPatchFields.H"
#include "zeroGradientFaPatchFields.H"

namespace Foam {

template<class T>
T *FaFieldValueExpressionDriver::getField(const string &name)
{
    if(debug) {
        Info << "FaFieldValueExpressionDriver::getField. Name: " << name 
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
                this->time(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
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

        f=new T(
            this->mesh().lookupObject<T>(name)
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
        FatalErrorIn("FaFieldValueExpressionDriver::getField(const string &name)")
            << "Could not find the field " << name 
                << " in memory or on disc" << endl
                << abort(FatalError);
    }
    
    f->dimensions().reset(nullDim);

    return f;
}

inline label FaFieldValueExpressionDriver::size() const
{
    return mesh_().nFaces();
}

inline label FaFieldValueExpressionDriver::pointSize() const
{
    return mesh_().nPoints();
}

inline const faMesh &FaFieldValueExpressionDriver::aMesh() const
{
    return mesh_;
}

inline const fvMesh &FaFieldValueExpressionDriver::mesh() const
{
    return dynamic_cast<const fvMesh&>(mesh_.thisDb());
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
            typeid(patch)==typeid(fvPatch)
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
            typeid(patch)==typeid(fvPatch)
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(fixedValueFaPatchField<T>)) {
            faPatchField<T> &pf=field.boundaryField()[patchI];
            
            pf==pf.patchInternalField();
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
                << abort(FatalError);
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

} // end namespace

#endif
