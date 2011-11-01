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

#include "volumeMinMaxFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(volumeMinMaxFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        volumeMinMaxFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

volumeMinMaxFunctionObject::volumeMinMaxFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    volumeFieldFunctionObject(name,t,dict)
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word volumeMinMaxFunctionObject::dirName()
{
    return typeName;
}

scalarField volumeMinMaxFunctionObject::process(const word& fieldName,scalar preset)
{
    return minMax(fieldName,preset);
}

Field<vector> volumeMinMaxFunctionObject::process(const word& fieldName,vector preset)
{
    return minMax(fieldName,preset);
}

Field<sphericalTensor> volumeMinMaxFunctionObject::process(const word& fieldName,sphericalTensor preset)
{
    return minMax(fieldName,preset);
}

Field<symmTensor> volumeMinMaxFunctionObject::process(const word& fieldName,symmTensor preset)
{
    return minMax(fieldName,preset);
}

Field<tensor> volumeMinMaxFunctionObject::process(const word& fieldName,tensor preset)
{
    return minMax(fieldName,preset);
}

stringList volumeMinMaxFunctionObject::columnNames()
{
    stringList result(2);
    result[0]="minimum";
    result[1]="maximum";
    return result;
}

} // namespace Foam

// ************************************************************************* //
