#include "word.H"
#include "objectRegistry.H"

namespace Foam {

/*

  Modification of objectRegistry::lookupObject

  The lookupObject() function in objectRegistry class cannot resolve the ::typeName or the parent()
  for some classes.  In particular those associated with Lagrangian clouds.

*/

template<class Type>
const Type& lookupObject(const Foam::objectRegistry& obr, const word& name)
{
    objectRegistry::const_iterator iter = obr.find(name); //objectRegistry::find(name);

    if (iter != obr.end() )
    {
        const Type* vpsiPtr_ = dynamic_cast<const Type*>(iter());

        if (vpsiPtr_)
        {
            return *vpsiPtr_;
        }

        FatalErrorIn
        (
            "objectRegistry::lookupObject<Type>(const word&) const"
        )   << nl
            << "    lookup of " << name << " from objectRegistry "
            << obr.name()
            << " successful\n    but it is not the correct type " 
            << ", it is a " << iter()->type()
            << abort(FatalError);
    }
    else
    {
        /*
        if ( obr.parentNotTime())
        {
            return obr.parent().lookupObject<Type>(name);
        }
        */
        FatalErrorIn
        (
            "objectRegistry::lookupObject<Type>(const word&) const"
        )   << nl
            << "    request for Type::typeName "
            << " " << name << " from objectRegistry " << obr.name()
            << " failed\n    available objects of type " << "Type::typeName"
            << " are" << nl
            << obr.names<Type>()
            << abort(FatalError);
    }

    return *reinterpret_cast< const Type* >(0);
}

}
