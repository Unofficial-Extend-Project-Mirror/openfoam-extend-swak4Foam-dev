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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "LagrangianCloudSourcePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "cloud.H"
#include "IOmanip.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

LagrangianCloudSourcePluginFunction::LagrangianCloudSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnValueType,
    const string &additionalArgs
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        returnValueType,
        string("cloudName primitive word")+ (
            additionalArgs==""
            ?
            additionalArgs
            :
            string(string(",")+additionalArgs)
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Ostream& LagrangianCloudSourcePluginFunction::listAvailableClouds(Ostream &o)
{
    o << nl << nl << "Available clouds in " << mesh().name() << endl;
    typedef HashTable<const cloud *> cloudTable;
    cloudTable clouds=mesh().lookupClass<cloud>();
    if(clouds.size()==0) {
        o << " No clouds available\n" << endl;
    }
    const label nameWidth=20;
    o << setw(nameWidth) << "Name" << " | " << "Type" << endl;
    o << "-------------------------------------------------------------" << endl;
    forAllConstIter(cloudTable,clouds,it) {
        o << setw(nameWidth) << it.key() << " | "
            << (*(*it)).type() << endl;
    }
    o << "-------------------------------------------------------------" << endl;
    if(mesh().foundObject<cloud>(cloudName())) {
        o << "Cloud " << cloudName() << " has type "
            << mesh().lookupObject<cloud>(cloudName()).type() << " typeid:"
            << typeid(mesh().lookupObject<cloud>(cloudName())).name()
            << endl;
    }
    return o;
}


void LagrangianCloudSourcePluginFunction::setArgument(
    label index,
    const word &name
)
{
    assert(index==0);
    cloudName_=name;
}


// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
