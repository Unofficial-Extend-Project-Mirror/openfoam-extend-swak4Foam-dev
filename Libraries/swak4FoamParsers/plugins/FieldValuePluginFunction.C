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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "FieldValuePluginFunction.H"
#include "FieldValueExpressionDriver.H"

namespace Foam {

defineTypeNameAndDebug(FieldValuePluginFunction,1);
defineRunTimeSelectionTable(FieldValuePluginFunction, name);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

FieldValuePluginFunction::FieldValuePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnType,
    const stringList &argumentSpecification
):
    CommonPluginFunction(
        parentDriver,
        name,
        returnType,
        argumentSpecification
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<FieldValuePluginFunction> FieldValuePluginFunction::New (
    const FieldValueExpressionDriver& driver,
    const word &name
)
{
    nameConstructorTable::iterator cstrIter =
        nameConstructorTablePtr_->find(name);
    if(cstrIter==nameConstructorTablePtr_->end()) {
        FatalErrorIn("autoPtr<FieldValuePluginFunction> FieldValuePluginFunction::New")
            << "Unknow plugin function " << name << endl
                << " Available functions are " << nameConstructorTablePtr_->sortedToc()
                << endl
                << exit(FatalError);        
    }
    return autoPtr<FieldValuePluginFunction>
        (
            cstrIter()(driver,name)
        );
}

bool FieldValuePluginFunction::exists (
    const word &name
)
{
    nameConstructorTable::iterator cstrIter =
        nameConstructorTablePtr_->find(name);
 
    return cstrIter!=nameConstructorTablePtr_->end(); 
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
