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

#include "volumeAverageFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(volumeAverageFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        volumeAverageFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

volumeAverageFunctionObject::volumeAverageFunctionObject
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

word volumeAverageFunctionObject::dirName()
{
    return typeName;
}

scalarField volumeAverageFunctionObject::process(const word& fieldName,scalar preset)
{
    return average(fieldName,preset);
}

Field<vector> volumeAverageFunctionObject::process(const word& fieldName,vector preset)
{
    return average(fieldName,preset);
}

Field<sphericalTensor> volumeAverageFunctionObject::process(const word& fieldName,sphericalTensor preset)
{
    return average(fieldName,preset);
}

Field<symmTensor> volumeAverageFunctionObject::process(const word& fieldName,symmTensor preset)
{
    return average(fieldName,preset);
}

Field<tensor> volumeAverageFunctionObject::process(const word& fieldName,tensor preset)
{
    return average(fieldName,preset);
}

stringList volumeAverageFunctionObject::columnNames()
{
    return stringList(1,"average");
}

} // namespace Foam

// ************************************************************************* //
