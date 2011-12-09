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

#include "solveAreaTransportPDE.H"

#include "polyMesh.H"

#include "volFields.H"

#include "FaFieldValueExpressionDriver.H"

#include "faScalarMatrix.H"

#include "faCFD.H"

namespace Foam {
    defineTypeNameAndDebug(solveAreaTransportPDE,0);
}

Foam::solveAreaTransportPDE::solveAreaTransportPDE
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
    diffusionDimension_(dimless),
    phiDimension_(dimless),
    sourceDimension_(dimless),
    sourceImplicitDimension_(dimless)
{
    if (!isA<polyMesh>(obr))
    {
        active_=false;
        WarningIn("solveAreaTransportPDE::solveAreaTransportPDE")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }

    read(dict);

    if(solveAt_==saStartup) {
        solve();
    }
}

Foam::solveAreaTransportPDE::~solveAreaTransportPDE()
{}

void Foam::solveAreaTransportPDE::read(const dictionary& dict)
{
    solvePDECommonFiniteArea::read(dict);

    if(active_) {
        if(!steady_) {
            dict.lookup("rho") >> rhoExpression_ >> rhoDimension_;
        }
        dict.lookup("diffusion") >> diffusionExpression_ >> diffusionDimension_;
        dict.lookup("source") >> sourceExpression_ >> sourceDimension_;
        if(dict.found("sourceImplicit")) {
            dict.lookup("sourceImplicit") 
                >> sourceImplicitExpression_ >> sourceImplicitDimension_;
        }
        dict.lookup("phi") >> phiExpression_ >> phiDimension_;
    }
}

void Foam::solveAreaTransportPDE::solve()
{
    if(active_) {
        const faMesh& mesh = driver_->aMesh();
        dictionary sol=mesh.solutionDict().subDict(fieldName_+"TransportPDE");
        
        FaFieldValueExpressionDriver &driver=driver_();

        int nCorr=sol.lookupOrDefault<int>("nCorrectors", 0);
        for (int corr=0; corr<=nCorr; corr++) {

            driver.clearVariables();

            driver.parse(diffusionExpression_);
            if(!driver.resultIsTyp<areaScalarField>()) {
                FatalErrorIn("Foam::solveAreaTransportPDE::solve()")
                    << diffusionExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }
            areaScalarField diffusionField(driver.getResult<areaScalarField>());
            diffusionField.dimensions().reset(diffusionDimension_);

            driver.parse(sourceExpression_);
            if(!driver.resultIsTyp<areaScalarField>()) {
                FatalErrorIn("Foam::solveAreaTransportPDE::solve()")
                    << sourceExpression_ << " does not evaluate to a scalar"
                        << endl
                        << exit(FatalError);
            }
            areaScalarField sourceField(driver.getResult<areaScalarField>());
            sourceField.dimensions().reset(sourceDimension_);

            driver.parse(phiExpression_);
            if(!driver.resultIsTyp<edgeScalarField>()) {
                FatalErrorIn("Foam::solveAreaTransportPDE::solve()")
                    << phiExpression_ << " does not evaluate to a edge scalar"
                        << endl
                        << exit(FatalError);
            }

            edgeScalarField phiField(driver.getResult<edgeScalarField>());

            phiField.dimensions().reset(phiDimension_);

            areaScalarField &f=theField_();

            faMatrix<scalar> eq(
                fam::div(phiField,f)
                -fam::laplacian(diffusionField,f,"laplacian(diffusion,"+f.name()+")")
                ==
                sourceField
            );

            if(!steady_) {
                driver.parse(rhoExpression_);
                if(!driver.resultIsTyp<areaScalarField>()) {
                    FatalErrorIn("Foam::solveAreaTransportPDE::solve()")
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
                    FatalErrorIn("Foam::solveAreaTransportPDE::solve()")
                        << sourceImplicitExpression_ << " does not evaluate to a scalar"
                            << endl
                            << exit(FatalError);
                }
                areaScalarField sourceImplicitField(driver.getResult<areaScalarField>());
                sourceImplicitField.dimensions().reset(sourceImplicitDimension_);
            
                eq-=fam::SuSp(sourceImplicitField,f);
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
