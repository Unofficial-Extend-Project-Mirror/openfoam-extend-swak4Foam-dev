/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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

Contributors/Copyright:
    2012-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "GlobalVariablesRepository.H"

#include "objectRegistry.H"
#include "swakTime.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(GlobalVariablesRepository, 0);

GlobalVariablesRepository *GlobalVariablesRepository::repositoryInstance(NULL);

ExpressionResult GlobalVariablesRepository::zero_;

GlobalVariablesRepository::GlobalVariablesRepository(
    const objectRegistry &obr
)
    :
    regIOobject(
        IOobject(
            "global_variables",
            obr.time().timeName(),
            "swak4Foam",
            obr.time(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        )
    ),
    lastTimeIndex_(obr.time().timeIndex())
{
    Pbug << "GlobalVariablesRepository at "
        << objectPath() << " created" << endl;

    if(headerOk()) {
        Pbug << "Found a file " <<  objectPath() << endl;

        readData(readStream("GlobalVariablesRepository"));
    }
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

GlobalVariablesRepository::~GlobalVariablesRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool GlobalVariablesRepository::writeData(Ostream &os) const
{
    Pbug << "GlobalVariablesRepository at " << objectPath()
            << " writing" << endl;

    // enforce ASCII because of a problem with HashTable written in BINARY
    IOstream::streamFormat old=os.format(IOstream::ASCII);

    os << globalVariables_;

    os.format(old);

    return os.good();
}

bool GlobalVariablesRepository::readData(Istream &is)
{
    Pbug << "GlobalVariablesRepository at " << objectPath()
        << " reading" << endl;

    // enforce ASCII because of a problem with HashTable written in BINARY
    IOstream::streamFormat old=is.format(IOstream::ASCII);

    is >> globalVariables_;

    is.format(old);

    Pbug << "GlobalVariablesRepository reading finished" << endl;

    return !is.bad();
}

GlobalVariablesRepository &GlobalVariablesRepository::getGlobalVariables(
    const objectRegistry &obr
)
{
    GlobalVariablesRepository*  ptr=repositoryInstance;

    if(debug) {
        Info << "GlobalVariablesRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Pout << "swak4Foam: Allocating new repository for sampledGlobalVariables\n";

        ptr=new GlobalVariablesRepository(obr);
    }

    repositoryInstance=ptr;

    if(
        repositoryInstance->lastTimeIndex_
        !=
        obr.time().timeIndex()
    ) {
        if(debug) {
            Info << "Resetting variables" << endl;
        }

        ResultTableTable &all=repositoryInstance->globalVariables_;
        forAllIter(ResultTableTable,all,table) {
            forAllIter(ResultTable,(*table),iter) {
                (*iter)->reset();
                // if(!(*iter)->hasValue()) {
                //     (*table).erase(iter);
                // }
            }
        }
        repositoryInstance->lastTimeIndex_=obr.time().timeIndex();
    }
    return *repositoryInstance;
}


GlobalVariablesRepository::ResultTable &GlobalVariablesRepository::getNamespace(const word &name)
{
    return globalVariables_[name];
}

const ExpressionResult &GlobalVariablesRepository::get(
    const word &name,
    const wordList &scopes
)
{
    forAll(scopes,scopeI) {
        const word &scopeName=scopes[scopeI];

        if(!globalVariables_.found(scopeName)) {
            WarningIn("GlobalVariablesRepository::get")
                << "There is no scope " << scopeName
                    << " in the list of global scopes "
                    << globalVariables_.toc()
                    << " when looking for " << name << endl;
            continue;
        }

        const ResultTable &scope=globalVariables_[scopeName];
        if(scope.found(name)) {
            Pbug << name << " ( " << scopeName << " )= "
                << *scope[name] << endl;

            return *scope[name];
        }
    }

    return zero_;
}

ExpressionResult& GlobalVariablesRepository::addValue(
    const dictionary &dict,
    const word scopeIn,
    const bool overwrite
) {
    word name(dict.lookup("globalName"));
    word scope;

    if(scopeIn!="") {
        scope=scopeIn;
    } else {
        scope=word(dict.lookup("globalScope"));
    }

    if(dict.found("resultType")) {
        return addValue(
            name,
            scope,
            ExpressionResult::New(dict),
            overwrite
        );
    } else {
        return addValue(
            name,
            scope,
            ExpressionResult(dict,true),
            overwrite
        );
    }
}

GlobalVariablesRepository::ResultTable
&GlobalVariablesRepository::getScope(const word &scope)
{
    if(!globalVariables_.found(scope)) {
        Pbug << "Creating global scope " << scope << endl;

        globalVariables_.insert(scope,ResultTable());
    }
    return globalVariables_[scope];
}

ExpressionResult& GlobalVariablesRepository::addValue(
    const word &name,
    const word &scope,
    const ExpressionResult &value,
    const bool overwrite
)
{
    Pbug << "Adding " << name << " to global scope "
        << scope << " Size: " << value.size() << endl;

    ResultTable &theScope=getScope(scope);

    if(!theScope.found(name)) {
        Dbug << name << " is new. Inserting" << endl;

        theScope.set(name,new ExpressionResult(value));
    } else if(overwrite) {
        Dbug << name << " is already there. Setting" << endl;

        (*theScope[name])=value;
    }

    Dbug << "Added: " << (*theScope[name]) << endl;

    return (*theScope[name]);
}

bool GlobalVariablesRepository::removeValue(
    const word &name,
    const word &scope
)
{
    Pbug << "Removing " << name << " to global scope "
        << scope << endl;

    ResultTable &theScope=getScope(scope);

    if(!theScope.found(name)) {
        Dbug << name << " is not there." << endl;

        return false;
    } else {
        Dbug << name << " is there. Removing" << endl;

        ResultTable::iterator iter=theScope.find(name);
        return theScope.erase(iter);
    }
}

ExpressionResult& GlobalVariablesRepository::addValue(
    const word &name,
    const word &scope,
    autoPtr<ExpressionResult> value,
    const bool overwrite
)
{
    Pbug << "Adding autoPtr " << name << " to global scope "
        << scope << " Size: " << value->size() << endl;

    ResultTable &theScope=getScope(scope);

    if(
        overwrite
        ||
        !theScope.found(name)
    ) {
        theScope.set(name,value.ptr());
    }

    return (*theScope[name]);
}

GlobalVariablesRepository::ResultTable::ResultTable(const ResultTable &r)
    :
    HashPtrTable<ExpressionResult,word>()
{
    for(const_iterator iter=r.begin();iter!=r.end();iter++) {
        this->insert(iter.key(),(**iter).clone().ptr());
    }
}

GlobalVariablesRepository::ResultTable::ResultTable(Istream &in)
    :
    HashPtrTable<ExpressionResult,word>(in)
{
}

GlobalVariablesRepository::ResultTable::ResultTable()
    :
    HashPtrTable<ExpressionResult,word>()
{
}
// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
