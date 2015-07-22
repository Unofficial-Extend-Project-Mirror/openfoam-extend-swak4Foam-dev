#ifndef objectRegistryUtility_H
#define objectRegistryUtility_H

#include "objectRegistry.H"
#include "word.H"

namespace foam
{

template<class Type>
const Type& lookupObject(const Foam::objectRegistry& obr, const Foam::word& name);

}

#ifdef NoRepository
#   include "objectRegistryUtilityTemplates.C"
#endif

#endif
