//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

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

} // end namespace

#endif
