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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "GlobalVariablesRepository.H"

namespace Foam {
 
// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(GlobalVariablesRepository, 0);

GlobalVariablesRepository *GlobalVariablesRepository::repositoryInstance(NULL);

ExpressionResult GlobalVariablesRepository::zero_;

GlobalVariablesRepository::GlobalVariablesRepository()
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

GlobalVariablesRepository::~GlobalVariablesRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

GlobalVariablesRepository &GlobalVariablesRepository::getGlobalVariables()
{
    GlobalVariablesRepository*  ptr=repositoryInstance;
    
    if(debug) {
        Pout << "GlobalVariablesRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Pout << "swak4Foam: Allocating new repository for sampledGlobalVariables\n";

        ptr=new GlobalVariablesRepository();
    }
    
    repositoryInstance=ptr;

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
            if(debug) {
                Pout << name << " ( " << scopeName << " )= " << scope[name] << endl;
            }
            return scope[name];
        }
    }

    return zero_;
}

void GlobalVariablesRepository::addValue(
    const dictionary &dict
) {
    word name(dict.lookup("globalName"));
    word scope(dict.lookup("globalScope"));

    addValue(
        name,
        scope,
        ExpressionResult(dict,true)
    );
}

void GlobalVariablesRepository::addValue(
    const word &name,
    const word &scope,
    const ExpressionResult &value
)
{
    if(!globalVariables_.found(scope)) {
        if(debug) {
            Pout << "Creating global scope " << scope << endl;
        }
        globalVariables_.insert(scope,ResultTable());
    }

    ResultTable &theScope=globalVariables_[scope];

    theScope.set(name,value);
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
