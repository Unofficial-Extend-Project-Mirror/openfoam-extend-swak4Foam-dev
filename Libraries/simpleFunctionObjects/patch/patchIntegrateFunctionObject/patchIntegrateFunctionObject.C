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
    2008-2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "patchIntegrateFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(patchIntegrateFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        patchIntegrateFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

patchIntegrateFunctionObject::patchIntegrateFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    patchFieldFunctionObject(name,t,dict)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word patchIntegrateFunctionObject::dirName()
{
    return typeName;
}

scalarField patchIntegrateFunctionObject::process(const word& fieldName,scalar preset)
{
    return integrate(fieldName,preset);
}

Field<vector> patchIntegrateFunctionObject::process(const word& fieldName,vector preset)
{
    return integrate(fieldName,preset);
}

Field<sphericalTensor> patchIntegrateFunctionObject::process(const word& fieldName,sphericalTensor preset)
{
    return integrate(fieldName,preset);
}

Field<symmTensor> patchIntegrateFunctionObject::process(const word& fieldName,symmTensor preset)
{
    return integrate(fieldName,preset);
}

Field<tensor> patchIntegrateFunctionObject::process(const word& fieldName,tensor preset)
{
    return integrate(fieldName,preset);
}



} // namespace Foam

// ************************************************************************* //
