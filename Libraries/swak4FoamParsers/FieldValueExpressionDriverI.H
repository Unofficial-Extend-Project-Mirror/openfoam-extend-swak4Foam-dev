//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_I_H
#define VALUE_EXPRESSION_DRIVER_I_H

template<class T>
T *FieldValueExpressionDriver::getField(const Foam::string &name)
{
    if(debug) {
        Foam::Info << "FieldValueExpressionDriver::getField. Name: " << name 
            << " Type: " << T::typeName << Foam::endl;
    }

    Foam::dimensionSet nullDim(0,0,0,0,0);

    T *f=NULL;

    if(
        searchInMemory_
        &&
        mesh_.foundObject<T>(name)
    ) {
        if(debug) {
            Foam::Info << "Getting " << name << " from memory" << Foam::endl;
        }

        f=new T(
            mesh_.lookupObject<T>(name)
        );
    } else if(
        searchOnDisc_
    ) {
        if(debug) {
            Foam::Info << "Reading " << name << " from disc" << Foam::endl;
        }
        f=new T(
            Foam::IOobject
            (
                name,
                time_,
                mesh_,
                Foam::IOobject::MUST_READ,
                Foam::IOobject::NO_WRITE
            ),
            mesh_
        );
        if(cacheReadFields_) {
            if(debug) {
                Foam::Info << "Registering a copy of " << name << " with mesh" << Foam::endl;
            }
            T* toCache=new T(*f);
            toCache->store();
        }
    }

    if(f==NULL) {
        Foam::FatalErrorIn("FieldValueExpressionDriver::getField(const Foam::string &name)")
            << "Could not find the field " << name 
                << " in memory or on disc" << Foam::endl
                << abort(Foam::FatalError);
    }
    
    f->dimensions().reset(nullDim);

    return f;
}

template<class T>
bool FieldValueExpressionDriver::FieldValueExpressionDriver::isThere(const Foam::string &name)
{
    if(debug) {
        Foam::Info << "FieldValueExpressionDriver::isThere. Name: " << name 
            << " Type: " << T::typeName << Foam::endl;
    }
    if(searchInMemory_) {
        if(mesh_.foundObject<T>(name)) {
            if(debug) {
                Foam::Info << "Found " << name << " in memory" << Foam::endl;
            }
            return true;
        }
    }

    if(
        searchOnDisc_
        &&
        getTypeOfField(name)==T::typeName
    ) {
        if(debug) {
            Foam::Info << "Found " << name << " on disc" << Foam::endl;
        }
        return true;
    } else {
        if(debug) {
            Foam::Info << name << " not found" << Foam::endl;
        }
        return false;
    }
}


#endif
