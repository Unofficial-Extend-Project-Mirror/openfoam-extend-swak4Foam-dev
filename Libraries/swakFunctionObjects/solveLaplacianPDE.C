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

#include "solveLaplacianPDE.H"

#include "SetsRepository.H"

#include "polyMesh.H"

#include "volFields.H"

#include "FieldValueExpressionDriver.H"

#include "fvScalarMatrix.H"

#include "fvm.H"

namespace Foam {
    defineTypeNameAndDebug(solveLaplacianPDE,0);
}

template<>
const char* Foam::NamedEnum<Foam::solveLaplacianPDE::solveAt,3>::names[]=
{
    "startup",
    "timestep",
    "write"
};

const Foam::NamedEnum<Foam::solveLaplacianPDE::solveAt,3> Foam::solveLaplacianPDE::solveAtNames_;

Foam::solveLaplacianPDE::solveLaplacianPDE
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    active_(true),
    obr_(obr),
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
        solve();
    }
}

Foam::solveLaplacianPDE::~solveLaplacianPDE()
{}

void Foam::solveLaplacianPDE::read(const dictionary& dict)
{
    if(active_) {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        
        solveAt_=
            solveAtNames_.read(
                dict.lookup("solveAt")
            );
        fieldName_=word(dict.lookup("fieldName"));
        if(
            theField_.valid()
            &&
            fieldName_!=theField_->name()
        ) {
            WarningIn("Foam::solveLaplacianPDE::read(const dictionary& dict)")
                << "Throwing out field " << theField_->name()
                    << " and loading " << fieldName_ << ". "
                    << "This might lead to unpredicatable behaviour" << endl;
            theField_.clear();
        }
        if(!theField_.valid()) {
            theField_.set(
                new volScalarField(
                    IOobject (
                        fieldName_,
                        mesh.time().timeName(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::AUTO_WRITE
                    ),
                    mesh
                )
            );
        }

        driver_.set(
            new FieldValueExpressionDriver(
                mesh.time().timeName(),
                mesh.time(),
                mesh,
                false, // no caching. No need
                true,  // search fields in memory
                false  // don't look up files in memory
            )
        );

        driver_->readVariablesAndTables(dict);
        
        steady_=readBool(dict.lookup("steady"));
        if(!steady_) {
            dict.lookup("rho") >> rhoExpression_ >> rhoDimension_;
        }
        dict.lookup("lambda") >> lambdaExpression_ >> lambdaDimension_;
        dict.lookup("source") >> sourceExpression_ >> sourceDimension_;
        if(dict.found("sourceImplicit")) {
            dict.lookup("sourceImplicit") 
                >> sourceImplicitExpression_ >> sourceImplicitDimension_;
        }
    }
}

void Foam::solveLaplacianPDE::execute()
{
    if(solveAt_==saTimestep) {
        solve();
    }
}


void Foam::solveLaplacianPDE::end()
{
}

void Foam::solveLaplacianPDE::solve()
{
    if(active_) {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        dictionary sol=mesh.solutionDict().subDict(fieldName_+"LaplacianPDE");
        
        FieldValueExpressionDriver &driver=driver_();

        int nCorr=sol.lookupOrDefault<int>("nCorrectors", 0);
        for (int corr=0; corr<=nCorr; corr++) {

            driver.clearVariables();

            driver.parse(lambdaExpression_);
            if(!driver.resultIsScalar()) {
                FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                    << lambdaExpression_ << " does not evaluate to a scalar"
                        << endl
                        << abort(FatalError);
            }
            volScalarField lambdaField(driver.getScalar());
            lambdaField.dimensions().reset(lambdaDimension_);

            driver.parse(sourceExpression_);
            if(!driver.resultIsScalar()) {
                FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                    << sourceExpression_ << " does not evaluate to a scalar"
                        << endl
                        << abort(FatalError);
            }
            volScalarField sourceField(driver.getScalar());
            sourceField.dimensions().reset(sourceDimension_);

            volScalarField &f=theField_();

            fvMatrix<scalar> eq(
                -fvm::laplacian(lambdaField,f,"laplacian(lambda,"+f.name()+")")
                ==
                sourceField
            );

            if(!steady_) {
                driver.parse(rhoExpression_);
                if(!driver.resultIsScalar()) {
                    FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                        << rhoExpression_ << " does not evaluate to a scalar"
                            << endl
                            << abort(FatalError);
                }
                volScalarField rhoField(driver.getScalar());
                rhoField.dimensions().reset(rhoDimension_);
            
                eq+=rhoField*fvm::ddt(f);
            }

            if(sourceImplicitExpression_!="") {
                driver.parse(sourceImplicitExpression_);
                if(!driver.resultIsScalar()) {
                    FatalErrorIn("Foam::solveLaplacianPDE::solve()")
                        << sourceImplicitExpression_ << " does not evaluate to a scalar"
                            << endl
                            << abort(FatalError);
                }
                volScalarField sourceImplicitField(driver.getScalar());
                sourceImplicitField.dimensions().reset(sourceImplicitDimension_);
            
                eq-=fvm::SuSp(sourceImplicitField,f);
            }

            int nNonOrthCorr=sol.lookupOrDefault<int>("nNonOrthogonalCorrectors", 0);
            for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
            {
                eq.solve();
            }
        }
    }
}

void Foam::solveLaplacianPDE::write()
{
    if(solveAt_==saWrite) {
        solve();
    }
}

// void Foam::solveLaplacianPDE::clearData()
// {
// }

// ************************************************************************* //
