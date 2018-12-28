/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2008-2011, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
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
