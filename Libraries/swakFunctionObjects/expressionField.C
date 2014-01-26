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
    2010-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $ 
\*---------------------------------------------------------------------------*/

#include "expressionField.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {
    defineTypeNameAndDebug(expressionField,0);
}

Foam::expressionField::expressionField
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
    if (!isA<fvMesh>(obr_))
    {
        active_=false;
        WarningIn("expressionField::expressionField")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    write();
}

Foam::expressionField::~expressionField()
{}

template<class T>
void Foam::expressionField::storeField(
    const T &data
)
{
    if(field_.empty()) {
        field_.reset(
            new T(
                IOobject(
                    name_,
                    obr_.time().timeName(),
                    obr_,
                    IOobject::NO_READ,
                    autowrite_ ? IOobject::AUTO_WRITE : IOobject::NO_WRITE
                ),
                data
            )
        );
    } else {
        //        dynamicCast<T &>(field_())==data; // doesn't work with gcc 4.2
        dynamic_cast<T &>(field_())==data;
    }

    if(
        this->autowrite_
        &&
        this->obr_.time().outputTime()
    ) {
        field_->write();
    }
}

void Foam::expressionField::timeSet()
{
    // Do nothing
}

void Foam::expressionField::read(const dictionary& dict)
{
    if(active_) {
        name_=word(dict.lookup("fieldName"));
        expression_=string(dict.lookup("expression"));
        autowrite_=Switch(dict.lookup("autowrite"));

        const fvMesh& mesh = refCast<const fvMesh>(obr_);

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

        driver_->readVariablesAndTables(dict_);

        // this might not work when rereading ... but what is consistent in that case?
        driver_->createWriterAndRead(name_+"_"+type());
    }
}

void Foam::expressionField::write()
{
    if(active_) {
        Info << "Creating expression field " << name_ << " ..." << flush;

        FieldValueExpressionDriver &driver=driver_();

        bool oldDimsetDebug=dimensionSet::debug;
        dimensionSet::debug=false;

        driver.clearVariables();

        driver.parse(expression_);

        dimensionSet::debug=oldDimsetDebug;

        Info << " type:" << driver.getResultType() << endl;

        if(driver.resultIsTyp<volVectorField>()) {
            storeField(
                driver.getResult<volVectorField>()
            );
        } else if(driver.resultIsTyp<volScalarField>()) {
            storeField(
                driver.getResult<volScalarField>()
            );
        } else if(driver.resultIsTyp<volTensorField>()) {
            storeField(
                driver.getResult<volTensorField>()
            );
        } else if(driver.resultIsTyp<volSymmTensorField>()) {
            storeField(
                driver.getResult<volSymmTensorField>()
            );
        } else if(driver.resultIsTyp<volSphericalTensorField>()) {
            storeField(
                driver.getResult<volSphericalTensorField>()
            );
        } else if(driver.resultIsTyp<surfaceVectorField>()) {
            storeField(
                driver.getResult<surfaceVectorField>()
            );
        } else if(driver.resultIsTyp<surfaceScalarField>()) {
            storeField(
                driver.getResult<surfaceScalarField>()
            );
        } else if(driver.resultIsTyp<surfaceTensorField>()) {
            storeField(
                driver.getResult<surfaceTensorField>()
            );
        } else if(driver.resultIsTyp<surfaceSymmTensorField>()) {
            storeField(
                driver.getResult<surfaceSymmTensorField>()
            );
        } else if(driver.resultIsTyp<surfaceSphericalTensorField>()) {
            storeField(
                driver.getResult<surfaceSphericalTensorField>()
            );
        } else if(driver.resultIsTyp<pointVectorField>()) {
            storeField(
                driver.getResult<pointVectorField>()
            );
        } else if(driver.resultIsTyp<pointScalarField>()) {
            storeField(
                driver.getResult<pointScalarField>()
            );
        } else if(driver.resultIsTyp<pointTensorField>()) {
            storeField(
                driver.getResult<pointTensorField>()
            );
        } else if(driver.resultIsTyp<pointSymmTensorField>()) {
            storeField(
                driver.getResult<pointSymmTensorField>()
            );
        } else if(driver.resultIsTyp<pointSphericalTensorField>()) {
            storeField(
                driver.getResult<pointSphericalTensorField>()
            );
        } else {
            WarningIn("Foam::expressionField::execute()")
                << "Expression '" << expression_
                    << "' evaluated to an unsupported type "
                    << driver.typ()
                    << endl;
        }
    }

    driver_->tryWrite();
}


void Foam::expressionField::end()
{
    execute();
}

void Foam::expressionField::execute()
{
}

void Foam::expressionField::clearData()
{
    field_.clear();
}

// ************************************************************************* //
