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

#include "solveAreaLaplacianPDE.H"

#include "polyMesh.H"

#include "FaFieldValueExpressionDriver.H"

#include "faScalarMatrix.H"

#include "fam.H"

namespace Foam {
    defineTypeNameAndDebug(solveAreaLaplacianPDE,0);
}

Foam::solveAreaLaplacianPDE::solveAreaLaplacianPDE
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    solvePDECommonFiniteArea(
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
        WarningIn("solveAreaLaplacianPDE::solveAreaLaplacianPDE")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }

    read(dict);

    if(solveAt_==saStartup) {
        solveWrapper();
    }
}

Foam::solveAreaLaplacianPDE::~solveAreaLaplacianPDE()
{}

void Foam::solveAreaLaplacianPDE::read(const dictionary& dict)
{
    solvePDECommonFiniteArea::read(dict);

    if(active_) {
        if(!steady_) {
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
                WarningIn("Foam::solveAreaLaplacianPDE::read(const dictionary& dict)")
                    << "Source expression " << sourceExpression_ << " set. "
                        << "Consider factoring out parts to 'sourceImplicit'\n"
                        << endl;

            }
        }
    }
}

void Foam::solveAreaLaplacianPDE::solve()
{
    if(active_) {
        const faMesh& mesh = driver_->aMesh();
        dictionary sol=mesh.solutionDict().subDict(fieldName_+"LaplacianPDE");

        FaFieldValueExpressionDriver &driver=driver_();

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
            if(!driver.resultIsTyp<areaScalarField>()) {
                FatalErrorIn("Foam::solveAreaLaplacianPDE::solve()")
                    << lambdaExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }

            areaScalarField lambdaField(driver.getResult<areaScalarField>());
            lambdaField.dimensions().reset(lambdaDimension_);

            driver.parse(sourceExpression_);
            if(!driver.resultIsTyp<areaScalarField>()) {
                FatalErrorIn("Foam::solveAreaLaplacianPDE::solve()")
                    << sourceExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }

            areaScalarField sourceField(driver.getResult<areaScalarField>());
            sourceField.dimensions().reset(sourceDimension_);

            areaScalarField &f=theField();

            faMatrix<scalar> eq(
                -fam::laplacian(lambdaField,f,"laplacian(lambda,"+f.name()+")")
                ==
                sourceField
            );

            if(!steady_) {
                driver.parse(rhoExpression_);
                if(!driver.resultIsTyp<areaScalarField>()) {
                    FatalErrorIn("Foam::solveAreaLaplacianPDE::solve()")
                        << rhoExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }

                areaScalarField rhoField(driver.getResult<areaScalarField>());
                rhoField.dimensions().reset(rhoDimension_);

                faMatrix<scalar> ddtMatrix=fam::ddt(f);
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
                if(!driver.resultIsTyp<areaScalarField>()) {
                    FatalErrorIn("Foam::solveAreaLaplacianPDE::solve()")
                        << sourceImplicitExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }

                areaScalarField sourceImplicitField(driver.getResult<areaScalarField>());
                sourceImplicitField.dimensions().reset(sourceImplicitDimension_);

                eq-=fam::Sp(sourceImplicitField,f);
            }

            if(doRelax(corr==nCorr)) {
                eq.relax();
            }

            int nNonOrthCorr=sol.lookupOrDefault<int>("nNonOrthogonalCorrectors", 0);
            for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
            {
                eq.solve();
            }
        }
    }
}

// ************************************************************************* //
