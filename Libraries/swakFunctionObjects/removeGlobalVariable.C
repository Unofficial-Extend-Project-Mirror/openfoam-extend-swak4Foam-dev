/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2011-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
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

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
bool
#else
void
#endif
Foam::removeGlobalVariable::write()
{
#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
    return true;
#endif
}

void Foam::removeGlobalVariable::clearData()
{
}

// ************************************************************************* //
