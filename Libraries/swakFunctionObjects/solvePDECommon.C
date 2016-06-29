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
    2011, 2013-2014, 2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "solvePDECommon.H"

#include "polyMesh.H"

namespace Foam {
    defineTypeNameAndDebug(solvePDECommon,0);
}

template<>
const char* Foam::NamedEnum<Foam::solvePDECommon::solveAt,4>::names[]=
{
    "startup",
    "timestep",
    "write",
    "never"
};

const Foam::NamedEnum<Foam::solvePDECommon::solveAt,4> Foam::solvePDECommon::solveAtNames_;

Foam::solvePDECommon::solvePDECommon
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
#ifdef FOAM_HAS_FVOPTIONS
    dummyOptionList_(
       dynamicCast<const fvMesh&>(obr)
    ),
    warnedAboutMissingOptionList_(
       false
    ),
#endif
    active_(true),
    obr_(obr),
    name_(name),
    steady_(false),
    relaxUnsteady_(false),
    relaxLastIteration_(false),
    restoreNonConvergedSteady_(true)
{
    if (!isA<polyMesh>(obr))
    {
        active_=false;
        WarningIn("solvePDECommon::solvePDECommon")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }
}

Foam::solvePDECommon::~solvePDECommon()
{}

#ifdef FOAM_HAS_FVOPTIONS
Foam::fv::optionList &Foam::solvePDECommon::fvOptions() const
{
    if(obr_.foundObject<fv::optionList>("fvOptions")) {
        return const_cast<fv::optionList&>(
            obr_.lookupObject<fv::optionList>("fvOptions")
        );
    } else {
        if(!warnedAboutMissingOptionList_) {
            const_cast<solvePDECommon&>(*this).warnedAboutMissingOptionList_=true;
            WarningIn("Foam::solvePDECommon::fvOptions()")
                 << "No 'fvOptions' found. Returning dummy (no further warnings)"
                 << endl;
        }
        return const_cast<solvePDECommon&>(*this).dummyOptionList_;
    }
}
#endif

void Foam::solvePDECommon::timeSet()
{
    // Do nothing
}

void Foam::solvePDECommon::readExpressionAndDimension(
    const dictionary &dict,
    const word &name,
    exprString &expr,
    dimensionSet &dim
)
{
    ITstream in(dict.lookup(name));

    expr=exprString(
        in,
        dict
    );

    in >> dim;
}

bool Foam::solvePDECommon::doRelax(bool last)
{
    return
        (steady_ || relaxUnsteady_)
        &&
        (!last || relaxLastIteration_);
}

void Foam::solvePDECommon::read(const dictionary& dict)
{
    if(debug) {
        Info << "Foam::solvePDECommon::read()" << endl;
    }
    if(active_) {
        solveAt_=
            solveAtNames_.read(
                dict.lookup("solveAt")
            );
        fieldName_=word(dict.lookup("fieldName"));

        steady_=readBool(dict.lookup("steady"));
        if(steady_) {
            if(dict.found("restoreNonConvergedSteady")) {
                restoreNonConvergedSteady_=readBool(dict.lookup("restoreNonConvergedSteady"));
            } else {
                WarningIn("solvePDECommon::read(const dictionary& dict)")
                    << "If you don't want to restore a steady solution that didn't converge set "
                        << "'restoreNonConvergedSteady false;' in " << dict.name()
                        << endl;
                restoreNonConvergedSteady_=true;
            }
            relaxUnsteady_=false;
        } else {
            if(dict.found("relaxUnsteady")) {
                relaxUnsteady_=readBool(dict.lookup("relaxUnsteady"));
            } else {
                WarningIn("solvePDECommon::read(const dictionary& dict)")
                    << "If you want the unsteady run to use relaxation set "
                        << "'relaxUnsteady true;' in " << dict.name()
                        << endl;
                relaxUnsteady_=false;
            }
            restoreNonConvergedSteady_=false;
        }
        if(steady_ || relaxUnsteady_) {
            if(dict.found("relaxLastIteration")) {
                relaxLastIteration_=readBool(dict.lookup("relaxLastIteration"));
            } else {
                WarningIn("solvePDECommon::read(const dictionary& dict)")
                    << "If in case of relaxation you want to relax the last "
                        << "iteration as well set "
                        << "'relaxLastIteration true;' in " << dict.name()
                        << endl;
                relaxLastIteration_=false;
            }
        }
        writeBeforeAfter_=dict.lookupOrDefault<bool>("writeBeforeAfter",false);
    }
}

void Foam::solvePDECommon::execute()
{
    if(debug) {
        Info << "Foam::solvePDECommon::execute()" << endl;
    }
    if(solveAt_==saTimestep) {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        solveWrapper();

        // as this is executed after the general write, write the field separately
        if(mesh.time().outputTime()) {
            writeData();
        }
    }
}


void Foam::solvePDECommon::end()
{
    if(debug) {
        Info << "Foam::solvePDECommon::end()" << endl;
    }
    execute();
}

void Foam::solvePDECommon::solveWrapper()
{
    if(debug) {
        Info << "Foam::solvePDECommon::solveWrapper()" << endl;
    }
    const fvMesh& mesh = refCast<const fvMesh>(obr_);

    if(writeBeforeAfter_) {
        Info << "Write " << fieldName_ << " before" << endl;
        this->writeOldField();
    }

    solve();

    if(writeBeforeAfter_ && !mesh.time().outputTime()) {
        Info << "Write " << fieldName_ << " after" << endl;
        this->writeNewField();
    }
}

void Foam::solvePDECommon::write()
{
    if(debug) {
        Info << "Foam::solvePDECommon::write()" << endl;
    }
    const fvMesh& mesh = refCast<const fvMesh>(obr_);
    if(
        solveAt_==saWrite
        &&
        mesh.time().outputTime()
    ) {
        solveWrapper();

        writeData();
    }
}

bool Foam::solvePDECommon::needsRhoField(bool warnIfSteady) const
{
#ifdef FOAM_HAS_FVOPTIONS
    if(
        warnIfSteady
	&&
	steady_
	&&
	fvOptions().size()>0
    ) {
        WarningIn("Foam::solvePDECommon::needsRhoField(bool warnIfSteady) const")
	  << "There are " << fvOptions().size() << " fvOptions defined." << nl
	  << "For technical reason a 'rho' entry is needed in " << name_
	  << endl;
    }
#endif

    return
      !steady_
#ifdef FOAM_HAS_FVOPTIONS
      ||
      fvOptions().size()>0
#endif
      ;
}

// ************************************************************************* //
