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
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $ 
\*---------------------------------------------------------------------------*/

#include "clearExpressionFaField.H"

#include "FaFieldValueExpressionDriver.H"

#include "expressionFaFieldFunctionObject.H"

namespace Foam {
    defineTypeNameAndDebug(clearExpressionFaField,0);
}

Foam::clearExpressionFaField::clearExpressionFaField
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    obr_(obr)
{
    read(dict);
}

Foam::clearExpressionFaField::~clearExpressionFaField()
{}

void Foam::clearExpressionFaField::read(const dictionary& dict)
{
    name_=word(dict.lookup("fieldName"));
}

void Foam::clearExpressionFaField::execute()
{
    const functionObjectList &fol=obr_.time().functionObjects();
    bool found=false;

    forAll(fol,i) {
        if(isA<expressionFaFieldFunctionObject>(fol[i])) {
            expressionFaField &ef=const_cast<expressionFaField &>(
                //                dynamicCast<const expressionFaFieldFunctionObject&>(fol[i]).outputFilter() // doesn't work with gcc 4.2
                dynamic_cast<const expressionFaFieldFunctionObject&>(fol[i]).outputFilter()
            );

            if(ef.name()==name_) {
                found=true;                
                ef.clearData();
            }
        }
    }

    if(!found) {
        WarningIn("clearExpressionFaField::execute()")
            << "No function object named " << name_ << " found" 
                << endl;
    }
}


void Foam::clearExpressionFaField::end()
{
}

void Foam::clearExpressionFaField::write()
{
}

// ************************************************************************* //
