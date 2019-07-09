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
    2013, 2015 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "swak.H"
#ifdef FOAM_FVMESH_IS_DATAOBJECT

#include "solverPerformanceToGlobalVariables.H"

#include "GlobalVariablesRepository.H"

#include "fvMesh.H"

#ifdef FOAM_SOLVER_PERFORMANCE_NOT_MEMBER_OF_DATA
#include "SolverPerformance.H"
#include "Residuals.H"
#endif

#ifdef FOAM_LDUMATRIX_SOLVER_PERFORMANCE
typedef Foam::lduMatrix::solverPerformance solverPerformance;
#endif

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
    scalarFieldNames_(dict.lookup("fieldNames")),
    vectorFieldNames_(dict.lookupOrDefault("vectorFieldNames",wordList())),
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

template<class T>
void Foam::solverPerformanceToGlobalVariables::addFieldToData(const word &name)
{
    Dbug << "Getting solver performance for " << name << endl;

#ifdef FOAM_SOLVER_PERFORMANCE_NOT_MEMBER_OF_DATA
    List<SolverPerformance<T> > perf(
        Residuals<T>::field(
            dynamicCast<const polyMesh&>(obr_),
            name
        )
    );
#else
    const data &theData=dynamicCast<const fvMesh&>(obr_);
    //    Info << theData.solverPerformanceDict() << endl;
    List<SolverPerformance<T> > perf(
        theData.solverPerformanceDict()[name]
    );
#endif

    setValue(name+"_nrOfPerformances",scalar(perf.size()));
    if(perf.size()==0) {
        Dbug << "No performances. Nothing stored" << endl;
        return;
    }

#ifdef FOAM_SOLVERPERFORMANCE_NITERATIONS_NO_VECTOR
    setValue(name+"_nIterations_first",dimensioned<scalar>("nix",dimless,perf[0].nIterations()));
    setValue(name+"_nIterations_last", dimensioned<scalar>("nix",dimless,perf[perf.size()-1].nIterations()));
#else
    setValue(name+"_nIterations_first",dimensioned<T>("nix",dimless,perf[0].nIterations()));
    setValue(name+"_nIterations_last", dimensioned<T>("nix",dimless,perf[perf.size()-1].nIterations()));
#endif
    setValue(name+"_initialResidual_first",dimensioned<T>("nix",dimless,perf[0].initialResidual()));
    setValue(name+"_initialResidual_last", dimensioned<T>("nix",dimless,perf[perf.size()-1].initialResidual()));
    setValue(name+"_finalResidual_first",dimensioned<T>("nix",dimless,perf[0].finalResidual()));
    setValue(name+"_finalResidual_last", dimensioned<T>("nix",dimless,perf[perf.size()-1].finalResidual()));

#ifdef FOAM_SOLVERPERFORMANCE_NITERATIONS_NO_VECTOR
    Field<scalar> nIterations(perf.size());
#else
    Field<T> nIterations(perf.size());
#endif
    Field<T> initialResidual(perf.size());
    Field<T> finalResidual(perf.size());

    for(int i=0;i<perf.size();i++) {
        nIterations[i]=perf[i].nIterations();
        initialResidual[i]=perf[i].initialResidual();
        finalResidual[i]=perf[i].finalResidual();
    }

    setValue(name+"_nIterations",nIterations);
    setValue(name+"_initialResidual",initialResidual);
    setValue(name+"_finalResidual",finalResidual);
}

template<class T>
void Foam::solverPerformanceToGlobalVariables::setValue(
    const word &name,
    T value
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

template<class T>
void Foam::solverPerformanceToGlobalVariables::setValue(
    const word &name,
    const Field<T> &value
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
    forAll(scalarFieldNames_,i) {
        addFieldToData<scalar>(scalarFieldNames_[i]);
    }
    forAll(vectorFieldNames_,i) {
        addFieldToData<vector>(vectorFieldNames_[i]);
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

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
bool
#else
void
#endif
Foam::solverPerformanceToGlobalVariables::write()
{
    executeAndWriteToGlobal();

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
    return true;
#endif
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
