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
    2016 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "solveTransportPDE.H"

#include "polyMesh.H"

#include "volFields.H"

#include "FieldValueExpressionDriver.H"

#include "fvScalarMatrix.H"
#include "dynamicFvMesh.H"

#include "fvm.H"
#include "fvcMeshPhi.H"

namespace Foam {
    defineTypeNameAndDebug(solveTransportPDE,0);
}

Foam::solveTransportPDE::solveTransportPDE
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    solvePDECommonFiniteVolume(
        name,
        obr,
        dict,
        loadFromFiles
    ),
    rhoDimension_(dimless),
    diffusionDimension_(dimless),
    phiDimension_(dimless),
    sourceDimension_(dimless),
    sourceImplicitDimension_(dimless),
    sourceImplicitUseSuSp_(false),
    makePhiRelative_(false),
    velocityDimension_(dimless),
    velocityName_("none")
{
    if (!isA<polyMesh>(obr))
    {
        active_=false;
        WarningIn("solveTransportPDE::solveTransportPDE")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }
    if(isA<dynamicFvMesh>(obr)) {
        Info << name << ": Dynamic mesh. Is the phi relative?" << endl;
        makePhiRelative_=readBool(dict.lookup("makePhiRelative"));
    }
    read(dict);

    if(solveAt_==saStartup) {
        solveWrapper();
    }
}

Foam::solveTransportPDE::~solveTransportPDE()
{}

void Foam::solveTransportPDE::read(const dictionary& dict)
{
    solvePDECommonFiniteVolume::read(dict);

    if(active_) {
        if(needsRhoField(true)) {
            readExpressionAndDimension(
                dict,
                "rho",
                rhoExpression_,
                rhoDimension_
            );
        }
        readExpressionAndDimension(
            dict,
            "diffusion",
            diffusionExpression_,
            diffusionDimension_
        );
        readExpressionAndDimension(
            dict,
            "source",
            sourceExpression_,
            sourceDimension_
        );
        if(dict.found("sourceImplicit")) {
            readExpressionAndDimension(
                dict,
                "sourceImplicit",
                sourceImplicitExpression_,
                sourceImplicitDimension_
            );
            if(dict.found("sourceImplicitUseSuSp")) {
                sourceImplicitUseSuSp_=readBool(dict.lookup("sourceImplicitUseSuSp"));
            } else {
                WarningIn("Foam::solveLaplacianPDE::read(const dictionary& dict)")
                    << "'sourceImplicitUseSuSp' not set in " << dict.name()
                        << " assuming 'false'" << endl;
             }
        } else {
            if(sourceExpression_!="0") {
                WarningIn("Foam::solveTransportPDE::read(const dictionary& dict)")
                    << "Source expression " << sourceExpression_ << " set. "
                        << "Consider factoring out parts to 'sourceImplicit'\n"
                        << endl;

            }
        }
        readExpressionAndDimension(
            dict,
            "phi",
            phiExpression_,
            phiDimension_
        );
        if(makePhiRelative_) {
            if(
                !dict.found("velocityName")
                &&
                !dict.found("velocityExpression")
            ) {
                FatalErrorIn("Foam::solveTransportPDE::read(const dictionary& dict)")
                    << "Dynamic mesh. Either specify 'velocityName' or 'velocityExpression' in "
                        << dict.name()
                        << endl
                        << exit(FatalError);
            }
            if(
                dict.found("velocityName")
                &&
                dict.found("velocityExpression")
            ) {
                FatalErrorIn("Foam::solveTransportPDE::read(const dictionary& dict)")
                    << "Dynamic mesh. Either specify 'velocityName' or 'velocityExpression' in "
                        << dict.name() << " but not both"
                        << endl
                        << exit(FatalError);
            }
            if(dict.found("velocityName")) {
                velocityName_=word(dict.lookup("velocityName"));
            } else {
                readExpressionAndDimension(
                    dict,
                    "velocityExpression",
                    velocityExpression_,
                    velocityDimension_
                );
            }
        }
    }
}

void Foam::solveTransportPDE::solve()
{
    if(active_) {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        dictionary sol=mesh.solutionDict().subDict(fieldName_+"TransportPDE");

        FieldValueExpressionDriver &driver=driver_();

        int nCorr=sol.lookupOrDefault<int>("nCorrectors", 0);
        if(
            nCorr==0
            &&
            steady_
        ) {
            WarningIn("Foam::solveTransportPDE::solve()")
                << name_ << " is steady. It is recommended to have in "
                    << sol.name() << " a nCorrectors>0"
                    << endl;

        }

        for (int corr=0; corr<=nCorr; corr++) {

            driver.clearVariables();

            driver.parse(diffusionExpression_);
            if(!driver.resultIsTyp<volScalarField>()) {
                FatalErrorIn("Foam::solveTransportPDE::solve()")
                    << diffusionExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }
            volScalarField diffusionField(driver.getResult<volScalarField>());
            diffusionField.dimensions().reset(diffusionDimension_);

            driver.parse(sourceExpression_);
            if(!driver.resultIsTyp<volScalarField>()) {
                FatalErrorIn("Foam::solveTransportPDE::solve()")
                    << sourceExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }
            volScalarField sourceField(driver.getResult<volScalarField>());
            sourceField.dimensions().reset(sourceDimension_);

            driver.parse(phiExpression_);
            if(!driver.resultIsTyp<surfaceScalarField>()) {
                FatalErrorIn("Foam::solveTransportPDE::solve()")
                    << phiExpression_ << " does not evaluate to a surface scalar"
                        << endl
                        << exit(FatalError);
            }
            surfaceScalarField phiField(driver.getResult<surfaceScalarField>());
            phiField.dimensions().reset(phiDimension_);

	    autoPtr<volScalarField> rhoField;
	    if(needsRhoField()) {
                driver.parse(rhoExpression_);
                if(!driver.resultIsTyp<volScalarField>()) {
                    FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                        << rhoExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }
                rhoField.set(
                    new volScalarField(
                        driver.getResult<volScalarField>()
                    )
                );
                rhoField().dimensions().reset(rhoDimension_);
	    }

            if(
                makePhiRelative_
                &&
                !steady_
            ) {
                if(velocityName_!="none") {
                    if(needsRhoField()) {
                        fvc::makeRelative(
                            phiField,
                            rhoField(),
                            obr_.lookupObject<volVectorField>(velocityName_)
                        );
                    } else {
                        fvc::makeRelative(
                            phiField,
                            obr_.lookupObject<volVectorField>(velocityName_)
                        );
                    }
                } else {
                    driver.parse(velocityExpression_);
                    if(!driver.resultIsTyp<volVectorField>()) {
                        FatalErrorIn("Foam::solveTransportPDE::solve()")
                            << velocityExpression_ << " does not evaluate to a volume vector"
                                << endl
                                << exit(FatalError);
                    }
                    volVectorField UField(driver.getResult<volVectorField>());
                    if(needsRhoField()) {
                        fvc::makeRelative(
                            phiField,
                            rhoField(),
                            UField
                        );
                    } else {
                        fvc::makeRelative(
                            phiField,
                            UField
                        );
                    }
                }
            }
            volScalarField &f=theField();

            fvMatrix<scalar> eq(
                fvm::div(phiField,f)
                -fvm::laplacian(diffusionField,f,"laplacian(diffusion,"+f.name()+")")
                ==
                sourceField
            );

#ifdef FOAM_HAS_FVOPTIONS
	    if(needsRhoField()) {
 	       eq-=fvOptions()(rhoField(),f);
	    }
#endif

            if(!steady_) {
                driver.parse(rhoExpression_);
                if(!driver.resultIsTyp<volScalarField>()) {
                    FatalErrorIn("Foam::solveTransportPDE::solve()")
                        << rhoExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }
                volScalarField rhoField(driver.getResult<volScalarField>());
                rhoField.dimensions().reset(rhoDimension_);

                fvMatrix<scalar> ddtMatrix(fvm::ddt(f));
                if(
                    !ddtMatrix.diagonal()
                    &&
                    !ddtMatrix.symmetric()
                    &&
                    !ddtMatrix.asymmetric()
                ) {
                    // Adding would fail
                } else {
                    eq+=rhoField*ddtMatrix;
                }
            }

            if(sourceImplicitExpression_!="") {
                driver.parse(sourceImplicitExpression_);
                if(!driver.resultIsTyp<volScalarField>()) {
                    FatalErrorIn("Foam::solveTransportPDE::solve()")
                        << sourceImplicitExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }
                volScalarField sourceImplicitField(driver.getResult<volScalarField>());
                sourceImplicitField.dimensions().reset(sourceImplicitDimension_);

                if(sourceImplicitUseSuSp_) {
                    eq-=fvm::SuSp(sourceImplicitField,f);
                } else {
                    eq-=fvm::Sp(sourceImplicitField,f);
                }
            }

            if(doRelax(corr==nCorr)) {
                eq.relax();
            }

#ifdef FOAM_HAS_FVOPTIONS
            fvOptions().constrain(eq);
#endif

            int nNonOrthCorr=sol.lookupOrDefault<int>("nNonOrthogonalCorrectors", 0);
            bool converged=true;
            for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
            {
                volScalarField fallback(
                    "fallback"+f.name(),
                    f
                );
#ifdef FOAM_LDUMATRIX_SOLVERPERFORMANCE
		lduMatrix::solverPerformance perf=eq.solve();
#elif defined(FOAM_LDUSOLVERPERFORMANCE)
		lduSolverPerformance  perf=eq.solve();
#else
		solverPerformance perf=eq.solve();
#endif		
                if(
                    !perf.converged()
                    &&
                    restoreNonConvergedSteady()
                ) {
                    WarningIn("Foam::solveTransportPDE::solve()")
                        << "Solution for " << f.name()
                            << " not converged. Restoring"
                            << endl;
                    f=fallback;
                    converged=false;
                    break;
                }
            }

#ifdef FOAM_HAS_FVOPTIONS
            fvOptions().correct(f);
#endif
            if(!converged) {
                break;
            }
        }
    }
}

// ************************************************************************* //
