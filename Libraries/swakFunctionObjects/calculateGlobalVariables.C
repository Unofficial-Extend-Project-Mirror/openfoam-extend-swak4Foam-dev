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
    2011-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "calculateGlobalVariables.H"

#include "GlobalVariablesRepository.H"

namespace Foam {
    defineTypeNameAndDebug(calculateGlobalVariables,0);
}

Foam::calculateGlobalVariables::calculateGlobalVariables
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
    :
    obr_(obr),
    driver_(
        CommonValueExpressionDriver::New(
            dict,
            refCast<const fvMesh>(obr)
        )
    ),
    toGlobalNamespace_(dict.lookup("toGlobalNamespace")),
    toGlobalVariables_(dict.lookup("toGlobalVariables")),
    noReset_(dict.lookupOrDefault<bool>("noReset",false))
{
    if(debug) {
        Info << "calculateGlobalVariables " << name << " created" << endl;
    }

    if(!dict.found("noReset")) {
        WarningIn("calculateGlobalVariables::calculateGlobalVariables")
            << "No entry 'noReset' in " << dict.name()
                << ". Assumig 'false'"<< endl;

    }

    driver_->createWriterAndRead(name+"_"+type());

    executeAndWriteToGlobal();
}

Foam::calculateGlobalVariables::~calculateGlobalVariables()
{}

void Foam::calculateGlobalVariables::executeAndWriteToGlobal()
{
    // this also sets the variables
    driver_->clearVariables();

    forAll(toGlobalVariables_,i) {
        const word &name=toGlobalVariables_[i];
        if(debug) {
            Info << "Getting variable " << name << endl;
        }

        ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
            obr_
        ).addValue(
            name,
            toGlobalNamespace_,
            const_cast<const CommonValueExpressionDriver&>(
                driver_()
            ).variable(name)
        );

        if(noReset_) {
            res.noReset();
        }

        if(debug) {
            Pout << "Has value "
                << const_cast<const CommonValueExpressionDriver&>(
                    driver_()
                ).variable(name) << endl;
        }
    }
}

void Foam::calculateGlobalVariables::timeSet()
{
    // Do nothing
}

void Foam::calculateGlobalVariables::read(const dictionary& dict)
{
    WarningIn("Foam::calculateGlobalVariables::read(const dictionary& dict)")
        << "This function object does not honor changes during runtime"
            << endl;
}

void Foam::calculateGlobalVariables::write()
{
    executeAndWriteToGlobal();

    // make sure that the stored Variables are consistently written
    driver_->tryWrite();
}


void Foam::calculateGlobalVariables::end()
{
}

void Foam::calculateGlobalVariables::execute()
{
}

void Foam::calculateGlobalVariables::clearData()
{
}

// ************************************************************************* //
