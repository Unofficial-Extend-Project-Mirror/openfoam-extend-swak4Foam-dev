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

#include "expressionFaField.H"

#include "FaFieldValueExpressionDriver.H"

namespace Foam {
    defineTypeNameAndDebug(expressionFaField,0);
}

Foam::expressionFaField::expressionFaField
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
        WarningIn("expressionFaField::expressionFaField")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    execute();
}

Foam::expressionFaField::~expressionFaField()
{}

template<class T>
void Foam::expressionFaField::storeField(
    const T &data,
    autoPtr<T> &store
)
{
    if(store.empty()) {
        store.reset(
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
        store()==data;
    }
}

void Foam::expressionFaField::read(const dictionary& dict)
{
    if(active_) {
        name_=word(dict.lookup("fieldName"));
        expression_=string(dict.lookup("expression"));
        autowrite_=Switch(dict.lookup("autowrite"));

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
    }
}

void Foam::expressionFaField::execute()
{
    if(active_) {
        FaFieldValueExpressionDriver &driver=driver_();

        driver.clearVariables();

        driver.parse(expression_);

        if(driver.resultIsVector()) {
            storeField(
                driver.getVector(),
                vectorField_
            );
            
        } else if(driver.resultIsScalar()) {
            storeField(
                driver.getScalar(),
                scalarField_
            );
        } else {
            WarningIn("Foam::expressionFaField::execute()")
                << "Expression '" << expression_ 
                    << "' evaluated to an unsupported type"
                    << endl;
        }
    }
}


void Foam::expressionFaField::end()
{
}

void Foam::expressionFaField::write()
{
}

void Foam::expressionFaField::clearData()
{
    scalarField_.clear();
    vectorField_.clear();
}

// ************************************************************************* //
