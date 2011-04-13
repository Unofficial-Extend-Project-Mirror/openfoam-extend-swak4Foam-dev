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

#include "manipulateField.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {
    defineTypeNameAndDebug(manipulateField,0);
}

Foam::manipulateField::manipulateField
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
        WarningIn("manipulateField::manipulateField")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    execute();
}

Foam::manipulateField::~manipulateField()
{}

template<class T>
void Foam::manipulateField::manipulate(
    const T &data,
    const volScalarField &mask
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

void Foam::manipulateField::read(const dictionary& dict)
{
    if(active_) {
        name_=word(dict.lookup("fieldName"));
        expression_=string(dict.lookup("expression"));
        maskExpression_=string(dict.lookup("mask"));

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
    }
}

void Foam::manipulateField::execute()
{
    if(active_) {
        FieldValueExpressionDriver &driver=driver_();

        driver.clearVariables();

        driver.parse(maskExpression_);

        if(!driver.resultIsLogical()) {
            FatalErrorIn("manipulateField::execute()")
                << maskExpression_ << " does not evaluate to a logical expression"
                    << endl
                    << abort(FatalError);
        }

        volScalarField conditionField(driver.getScalar());

        driver.parse(expression_);

        if(driver.resultIsVector()) {
            manipulate(
                driver.getVector(),
                conditionField
            );
            
        } else if(driver.resultIsScalar()) {
            manipulate(
                driver.getScalar(),
                conditionField
            );
        } else {
            WarningIn("Foam::manipulateField::execute()")
                << "Expression '" << expression_ 
                    << "' evaluated to an unsupported type"
                    << endl;
        }
    }
}


void Foam::manipulateField::end()
{
}

void Foam::manipulateField::write()
{
}

// ************************************************************************* //
