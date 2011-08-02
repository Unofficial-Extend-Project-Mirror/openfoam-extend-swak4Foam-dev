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

#include "addGlobalVariable.H"

#include "GlobalVariablesRepository.H"

namespace Foam {
    defineTypeNameAndDebug(addGlobalVariable,0);
}

Foam::addGlobalVariable::addGlobalVariable
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
{
    read(dict);
    execute();
}

Foam::addGlobalVariable::~addGlobalVariable()
{}

void Foam::addGlobalVariable::read(const dictionary& dict)
{
    GlobalVariablesRepository::getGlobalVariables().addValue(dict);
}

void Foam::addGlobalVariable::execute()
{
}


void Foam::addGlobalVariable::end()
{
}

void Foam::addGlobalVariable::write()
{
}

void Foam::addGlobalVariable::clearData()
{
}

// ************************************************************************* //
