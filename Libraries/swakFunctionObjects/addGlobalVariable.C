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
    2011-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
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
    :
    obr_(obr)
{
    read(dict);
    execute();
}

Foam::addGlobalVariable::~addGlobalVariable()
{}

void Foam::addGlobalVariable::timeSet()
{
    // Do nothing
}

void Foam::addGlobalVariable::read(const dictionary& dict)
{
    if(dict.found("globalVariables")) {
        const dictionary variables(dict.subDict("globalVariables"));
        const word scope(dict.lookup("globalScope"));

        wordList names(variables.toc());
        forAll(names,i) {
            const word &name=names[i];
            const dictionary &dict=variables.subDict(name);

            ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
                obr_
            ).addValue(
                name,
                scope,
                ExpressionResult(dict,true,true),
                false
            );
            res.noReset();
        }
    } else {
        ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
            obr_
        ).addValue(
            dict,
            "",
            false
        );
        res.noReset();
    }
}

void Foam::addGlobalVariable::execute()
{
}


void Foam::addGlobalVariable::end()
{
}

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
bool
#else
void
#endif
Foam::addGlobalVariable::write()
{
#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
    return true;
#endif
}

void Foam::addGlobalVariable::clearData()
{
}

// ************************************************************************* //
