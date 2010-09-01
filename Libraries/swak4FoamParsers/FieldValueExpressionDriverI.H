//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

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
        f=new T(
            IOobject
            (
                name,
                time_,
                mesh_,
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            mesh_
        );
        if(cacheReadFields()) {
            if(debug) {
                Info << "Registering a copy of " << name << " with mesh" << endl;
            }
            T* toCache=new T(*f);
            toCache->store();
        }
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

} // end namespace

#endif
