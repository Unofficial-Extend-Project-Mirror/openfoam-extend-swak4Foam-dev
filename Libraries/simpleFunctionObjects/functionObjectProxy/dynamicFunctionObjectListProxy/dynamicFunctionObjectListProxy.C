//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "dynamicFunctionObjectListProxy.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(dynamicFunctionObjectListProxy, 0);
    addToRunTimeSelectionTable
    (
        functionObject,
        dynamicFunctionObjectListProxy,
        dictionary
    );

    defineTypeNameAndDebug(dynamicFunctionObjectListProxy::dynamicDictionaryProvider, 0);
    defineRunTimeSelectionTable(dynamicFunctionObjectListProxy::dynamicDictionaryProvider,dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

dynamicFunctionObjectListProxy::dynamicFunctionObjectListProxy
(
    const word& name,
    const Time& t,
    const dictionary& dict,
    const char *providerNameStr
)
:
    functionObjectListProxy(
        name,
        t,
        dict,
        false
    )
{
    word providerName(providerNameStr);
    if(providerName.size()==0) {
        providerName=word(dict.lookup("dictionaryProvider"));
    }
    provider_=dynamicDictionaryProvider::New(
        providerName,
        dict,
        (*this)
    );

    if(
        readBool(dict.lookup("readDuringConstruction"))
    ) {
        if(writeDebug()) {
            Info << this->name() << " list initialized during construction" << endl;
        }
        read(dict);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void dynamicFunctionObjectListProxy::initFunctions()
{
    string text(provider_->getDictionaryText());
    if(Pstream::parRun()) {
        string localText=text;
        Pstream::scatter(text);
        if(text!=localText) {
            Pout << "WARNING: In dynamicFunctionObjectListProxy::initFunctions() "
                << "for " << name() 
                << " the text of the dictionary is different from the master"
                << endl
                << " Overwritten local version with master";
        }
    }
    {
        fileName fName=obr_.time().path()/word(this->name()+".dictionaryText");
        OFstream o(fName);
        o << text.c_str();
    }

    IStringStream inStream(
        text
    );

    dynamicDict_.set(
        new dictionary(inStream)
    );

    {
        fileName fName=obr_.time().path()/word(this->name()+".dictionaryDump");
        OFstream o(fName);
        o << dynamicDict_();
    }

    if(!dynamicDict_->found("functions")) {
        FatalErrorIn("dynamicFunctionObjectListProxy::initFunctions()")
            << "Dictionary for" << this->name()
                << " does not have an entry 'functions'"
                << endl
                << exit(FatalError);

    }

    functions_.set(
        new functionObjectList(
            time(),
            dynamicDict_()
        )
    );

    if(writeDebug()) {
        Info << this->name() << " list initialized with "
            << functions_->size() << " FOs" << endl;
    }
}

dynamicFunctionObjectListProxy::dynamicDictionaryProvider::dynamicDictionaryProvider(
    const dictionary& dict,
    const dynamicFunctionObjectListProxy &owner
) :
    owner_(owner)
{
}

autoPtr<dynamicFunctionObjectListProxy::dynamicDictionaryProvider>
dynamicFunctionObjectListProxy::dynamicDictionaryProvider::New(
    const word& type,
    const dictionary& dict,
    const dynamicFunctionObjectListProxy &owner
){
    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(type);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "autoPtr<dynamicFunctionObjectListProxy::dynamicDictionaryProvider> dynamicFunctionObjectListProxy::dynamicDictionaryProvider::New"
        )   << "Unknown dynamicFunctionObjectListProxy::dynamicDictionaryProvider type " << type
            << endl << endl
            << "Valid types are :" << endl
#ifdef FOAM_DEV
            << dictionaryConstructorTablePtr_->toc()
#else
            << dictionaryConstructorTablePtr_->sortedToc()
#endif
            << exit(FatalError);
    }

    if(debug) {
        Pout << "Creating dictionary provider of type " << type << endl;
    }

    return autoPtr<dynamicFunctionObjectListProxy::dynamicDictionaryProvider>
    (
        cstrIter()(dict,owner)
    );
}

} // namespace Foam

// ************************************************************************* //
