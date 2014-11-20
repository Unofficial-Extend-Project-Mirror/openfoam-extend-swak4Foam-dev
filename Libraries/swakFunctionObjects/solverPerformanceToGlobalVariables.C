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

#include "swak.H"
#ifdef FOAM_FVMESH_IS_DATAOBJECT

#include "solverPerformanceToGlobalVariables.H"

#include "GlobalVariablesRepository.H"

#include "fvMesh.H"

namespace Foam {
    defineTypeNameAndDebug(solverPerformanceToGlobalVariables,0);
}

Foam::solverPerformanceToGlobalVariables::solverPerformanceToGlobalVariables
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
    :
    obr_(obr),
    fieldNames_(dict.lookup("fieldNames")),
    toGlobalNamespace_(dict.lookup("toGlobalNamespace")),
    noReset_(dict.lookupOrDefault<bool>("noReset",false))
{
    if(debug) {
        Info << "solverPerformanceToGlobalVariables " << name << " created" << endl;
    }

    if(!dict.found("noReset")) {
        WarningIn("solverPerformanceToGlobalVariables::solverPerformanceToGlobalVariables")
            << "No entry 'noReset' in " << dict.name()
                << ". Assumig 'false'"<< endl;

    }

    //    executeAndWriteToGlobal();
}

Foam::solverPerformanceToGlobalVariables::~solverPerformanceToGlobalVariables()
{}

void Foam::solverPerformanceToGlobalVariables::addFieldToData(const word &name)
{
    Dbug << "Getting solver performance for " << name << endl;

    const data &theData=dynamicCast<const fvMesh&>(obr_);
    List<solverPerformance> perf(
        theData.solverPerformanceDict()[name]
    );

    setValue(name+"_nrOfPerformances",perf.size());

    if(perf.size()==0) {
        Dbug << "No performances. Nothing stored" << endl;
        return;
    }

    setValue(name+"_nIterations_first",perf[0].nIterations());
    setValue(name+"_nIterations_last", perf[perf.size()-1].nIterations());
    setValue(name+"_initialResidual_first",perf[0].initialResidual());
    setValue(name+"_initialResidual_last", perf[perf.size()-1].initialResidual());
    setValue(name+"_finalResidual_first",perf[0].finalResidual());
    setValue(name+"_finalResidual_last", perf[perf.size()-1].finalResidual());

    scalarField nIterations(perf.size());
    scalarField initialResidual(perf.size());
    scalarField finalResidual(perf.size());

    for(int i=0;i<perf.size();i++) {
        nIterations[i]=perf[i].nIterations();
        initialResidual[i]=perf[i].initialResidual();
        finalResidual[i]=perf[i].finalResidual();
    }

    setValue(name+"_nIterations",nIterations);
    setValue(name+"_initialResidual",initialResidual);
    setValue(name+"_finalResidual",finalResidual);
}

void Foam::solverPerformanceToGlobalVariables::setValue(
    const word &name,
    scalar value
) {
    Dbug << "Setting " << name << " to " << value << endl;

    ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
            obr_
        ).addValue(
            name,
            toGlobalNamespace_,
            ExpressionResult(value)
        );

    if(noReset_) {
        res.noReset();
    }
}

void Foam::solverPerformanceToGlobalVariables::setValue(
    const word &name,
    const scalarField &value
) {
    Dbug << "Setting " << name << " to " << value << endl;

    ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
            obr_
        ).addValue(
            name,
            toGlobalNamespace_,
            ExpressionResult(value)
        );

    if(noReset_) {
        res.noReset();
    }
}

void Foam::solverPerformanceToGlobalVariables::executeAndWriteToGlobal()
{
    forAll(fieldNames_,i) {
        addFieldToData(fieldNames_[i]);
    }
}

void Foam::solverPerformanceToGlobalVariables::timeSet()
{
    // Do nothing
}

void Foam::solverPerformanceToGlobalVariables::read(const dictionary& dict)
{
    WarningIn("Foam::solverPerformanceToGlobalVariables::read(const dictionary& dict)")
        << "This function object does not honor changes during runtime"
            << endl;
}

void Foam::solverPerformanceToGlobalVariables::write()
{
    executeAndWriteToGlobal();
}


void Foam::solverPerformanceToGlobalVariables::end()
{
}

void Foam::solverPerformanceToGlobalVariables::execute()
{
}

void Foam::solverPerformanceToGlobalVariables::clearData()
{
}

// ************************************************************************* //
#endif
