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
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $ 
\*---------------------------------------------------------------------------*/

#include "manipulateFaField.H"

#include "FaFieldValueExpressionDriver.H"

namespace Foam {
    defineTypeNameAndDebug(manipulateFaField,0);
}

Foam::manipulateFaField::manipulateFaField
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    active_(true),
    obr_(obr),
    dict_(dict)
{
    driver_->createWriterAndRead(name+"_"+type());

    if (!isA<fvMesh>(obr_))
    {
        active_=false;
        WarningIn("manipulateFaField::manipulateFaField")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    execute();
}

Foam::manipulateFaField::~manipulateFaField()
{}

template<class T,class TMask>
void Foam::manipulateFaField::manipulate(
    const T &data,
    const TMask &mask
)
{
    T &original=const_cast<T &>(obr_.lookupObject<T>(name_));

    forAll(original,cellI) {
        if(mask[cellI]>SMALL) {
            original[cellI]=data[cellI];
        }
    }
    original.correctBoundaryConditions();
}

template<class T,class TMask>
void Foam::manipulateFaField::manipulateEdge(
    const T &data,
    const TMask &mask
)
{
    T &original=const_cast<T &>(obr_.lookupObject<T>(name_));

    forAll(original,cellI) {
        if(mask[cellI]>SMALL) {
            original[cellI]=data[cellI];
        }
    }
    //    original.correctBoundaryConditions();
}

void Foam::manipulateFaField::read(const dictionary& dict)
{
    if(active_) {
        name_=word(dict.lookup("fieldName"));
        expression_=string(dict.lookup("expression"));
        maskExpression_=string(dict.lookup("mask"));

        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        
        driver_.set(
            new FaFieldValueExpressionDriver(
                mesh,
                false, // no caching. No need
                true,  // search fields in memory
                false  // don't look up files in memory
            )
        );

        driver_->readVariablesAndTables(dict_);

        driver_->createWriterAndRead(name_+"_"+type());
    }
}

void Foam::manipulateFaField::execute()
{
    if(active_) {
        FaFieldValueExpressionDriver &driver=driver_();

        driver.clearVariables();

        driver.parse(maskExpression_);

        if(!driver.isLogical()) {
            FatalErrorIn("manipulateFaField::execute()")
                << maskExpression_ << " does not evaluate to a logical expression"
                    << endl
                    << exit(FatalError);
        }
        
        if(driver.resultIsTyp<areaScalarField>(true)) {
            areaScalarField conditionField(driver.getResult<areaScalarField>());

            driver.parse(expression_);

            if(driver.resultIsTyp<areaVectorField>()) {
                manipulate(
                    driver.getResult<areaVectorField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<areaScalarField>()) {
                manipulate(
                    driver.getResult<areaScalarField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<areaTensorField>()) {
                manipulate(
                    driver.getResult<areaTensorField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<areaSymmTensorField>()) {
                manipulate(
                    driver.getResult<areaSymmTensorField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<areaSphericalTensorField>()) {
                manipulate(
                    driver.getResult<areaSphericalTensorField>(),
                    conditionField
                );
            } else {
                WarningIn("Foam::manipulateFaField::execute()")
                    << "Expression '" << expression_ 
                        << "' evaluated to an unsupported type"
                        << driver.typ() << " that is incompatible with a mask defined on areas"
                        << endl;
            }
        } else if(driver.resultIsTyp<edgeScalarField>(true)) {
            edgeScalarField conditionField(driver.getResult<edgeScalarField>());

            driver.parse(expression_);

            if(driver.resultIsTyp<edgeVectorField>()) {
                manipulateEdge(
                    driver.getResult<edgeVectorField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<edgeScalarField>()) {
                manipulateEdge(
                    driver.getResult<edgeScalarField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<edgeTensorField>()) {
                manipulateEdge(
                    driver.getResult<edgeTensorField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<edgeSymmTensorField>()) {
                manipulateEdge(
                    driver.getResult<edgeSymmTensorField>(),
                    conditionField
                );
            } else if(driver.resultIsTyp<edgeSphericalTensorField>()) {
                manipulateEdge(
                    driver.getResult<edgeSphericalTensorField>(),
                    conditionField
                );
            } else {
                WarningIn("Foam::manipulateFaField::execute()")
                    << "Expression '" << expression_ 
                        << "' evaluated to an unsupported type"
                        << driver.typ() << " that is incompatible with a mask defined on edges"
                        << endl;
            }
        }
    }

    driver_->tryWrite();
}


void Foam::manipulateFaField::end()
{
}

void Foam::manipulateFaField::write()
{
}

// ************************************************************************* //
