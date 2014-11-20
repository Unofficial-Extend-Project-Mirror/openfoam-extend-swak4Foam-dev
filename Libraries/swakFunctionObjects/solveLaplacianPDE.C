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
    2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "solveLaplacianPDE.H"

#include "polyMesh.H"

#include "volFields.H"

#include "FieldValueExpressionDriver.H"

#include "fvScalarMatrix.H"

#include "fvm.H"

namespace Foam {
    defineTypeNameAndDebug(solveLaplacianPDE,0);
}

Foam::solveLaplacianPDE::solveLaplacianPDE
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
    lambdaDimension_(dimless),
    sourceDimension_(dimless),
    sourceImplicitDimension_(dimless)
{
    if (!isA<polyMesh>(obr))
    {
        active_=false;
        WarningIn("solveLaplacianPDE::solveLaplacianPDE")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }

    read(dict);

    if(solveAt_==saStartup) {
        solveWrapper();
    }
}

Foam::solveLaplacianPDE::~solveLaplacianPDE()
{}

void Foam::solveLaplacianPDE::read(const dictionary& dict)
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
            "lambda",
            lambdaExpression_,
            lambdaDimension_
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
        } else {
            if(sourceExpression_!="0") {
                WarningIn("Foam::solveLaplacianPDE::read(const dictionary& dict)")
                    << "Source expression " << sourceExpression_ << " set. "
                        << "Consider factoring out parts to 'sourceImplicit'\n"
                        << endl;

            }
        }
    }
}

void Foam::solveLaplacianPDE::solve()
{
    if(active_) {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        dictionary sol=mesh.solutionDict().subDict(fieldName_+"LaplacianPDE");

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

            driver.parse(lambdaExpression_);
            if(!driver.resultIsTyp<volScalarField>()) {
                FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                    << lambdaExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }
            volScalarField lambdaField(driver.getResult<volScalarField>());
            lambdaField.dimensions().reset(lambdaDimension_);

            driver.parse(sourceExpression_);
            if(!driver.resultIsTyp<volScalarField>()) {
                FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                    << sourceExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }
            volScalarField sourceField(driver.getResult<volScalarField>());
            sourceField.dimensions().reset(sourceDimension_);

            volScalarField &f=theField();

            fvMatrix<scalar> eq(
                -fvm::laplacian(lambdaField,f,"laplacian(lambda,"+f.name()+")")
                ==
                sourceField
            );

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

#ifdef FOAM_HAS_FVOPTIONS
	    if(needsRhoField()) {
	      eq-=fvOptions()(rhoField(),f);
	    }
#endif
            if(!steady_) {
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
  		   eq+=rhoField()*ddtMatrix;
                }
            }

            if(sourceImplicitExpression_!="") {
                driver.parse(sourceImplicitExpression_);
                if(!driver.resultIsTyp<volScalarField>()) {
                    FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                        << sourceImplicitExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }
                volScalarField sourceImplicitField(driver.getResult<volScalarField>());
                sourceImplicitField.dimensions().reset(sourceImplicitDimension_);

                eq-=fvm::Sp(sourceImplicitField,f);
            }

            if(doRelax(corr==nCorr)) {
                eq.relax();
            }

#ifdef FOAM_HAS_FVOPTIONS
            fvOptions().constrain(eq);
#endif

            int nNonOrthCorr=sol.lookupOrDefault<int>("nNonOrthogonalCorrectors", 0);
            for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
            {
                eq.solve();
            }

#ifdef FOAM_HAS_FVOPTIONS
            fvOptions().correct(f);
#endif
        }
    }
}

// ************************************************************************* //
