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

Contributors/Copyright:
    2011-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "removeGlobalVariable.H"

#include "GlobalVariablesRepository.H"

namespace Foam {
    defineTypeNameAndDebug(removeGlobalVariable,0);
}

Foam::removeGlobalVariable::removeGlobalVariable
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
    :
    obr_(obr)
{
    read(dict);
    execute();
}

Foam::removeGlobalVariable::~removeGlobalVariable()
{}

void Foam::removeGlobalVariable::timeSet()
{
    // Do nothing
}

void Foam::removeGlobalVariable::read(const dictionary& dict)
{
    names_=wordList(dict.lookup("globalVariables"));
    scope_=word(dict.lookup("globalScope"));
}

void Foam::removeGlobalVariable::execute()
{
   forAll(names_,i) {
        const word &name=names_[i];

        bool removed=GlobalVariablesRepository::getGlobalVariables(
            obr_
        ).removeValue(
            name,
            scope_
        );
        if(!removed) {
            WarningIn("Foam::removeGlobalVariable::read(const dictionary& dict)")
                << "Variable " << name << " in scope " << scope_
                    << " not removed" << endl;
        }
   }
}


void Foam::removeGlobalVariable::end()
{
}

void Foam::removeGlobalVariable::write()
{
}

void Foam::removeGlobalVariable::clearData()
{
}

// ************************************************************************* //
