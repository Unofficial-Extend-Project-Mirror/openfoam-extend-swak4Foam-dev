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

#include "volumeIntegrateFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(volumeIntegrateFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        volumeIntegrateFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

volumeIntegrateFunctionObject::volumeIntegrateFunctionObject
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

word volumeIntegrateFunctionObject::dirName()
{
    return typeName;
}

scalarField volumeIntegrateFunctionObject::process(const word& fieldName,scalar preset)
{
    return integrate(fieldName,preset);
}

Field<vector> volumeIntegrateFunctionObject::process(const word& fieldName,vector preset)
{
    return integrate(fieldName,preset);
}

Field<sphericalTensor> volumeIntegrateFunctionObject::process(const word& fieldName,sphericalTensor preset)
{
    return integrate(fieldName,preset);
}

Field<symmTensor> volumeIntegrateFunctionObject::process(const word& fieldName,symmTensor preset)
{
    return integrate(fieldName,preset);
}

Field<tensor> volumeIntegrateFunctionObject::process(const word& fieldName,tensor preset)
{
    return integrate(fieldName,preset);
}

stringList volumeIntegrateFunctionObject::columnNames()
{
    return stringList(1,"integrale");
}

} // namespace Foam

// ************************************************************************* //
