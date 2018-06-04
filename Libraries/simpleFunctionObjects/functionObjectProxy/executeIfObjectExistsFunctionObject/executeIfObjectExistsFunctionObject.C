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
    2011, 2013, 2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "executeIfObjectExistsFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "objectRegistry.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(executeIfObjectExistsFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfObjectExistsFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfObjectExistsFunctionObject::executeIfObjectExistsFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    conditionalFunctionObjectListProxy(
        name,
        t,
        dict
    )
{
    readParameters(dict);

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    start();
#endif
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool executeIfObjectExistsFunctionObject::condition()
{
    if(writeDebug()) {
        Info << "Looking for object " << objectName_
            << " -> " << obr().foundObject<IOobject>(objectName_) << endl;
    }
    if( ! obr().foundObject<IOobject>(objectName_) ) {
        return ! objectShouldExist_;
    } else if(checkType_) {
        const IOobject &theOb=obr().lookupObject<IOobject>(objectName_);
        if(writeDebug()) {
            Info << "Type of " << objectName_ << " is "
                << theOb.type()
                << ". Looking for " << objectType_ << endl;
        }

        if(theOb.type()==objectType_) {
            return objectShouldExist_;
        } else {
            return ! objectShouldExist_;
        }
    } else {
        return objectShouldExist_;
    }
}

bool executeIfObjectExistsFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

void executeIfObjectExistsFunctionObject::readParameters(const dictionary &dict)
{
    objectName_=word(dict.lookup("objectName"));
    checkType_=readBool(dict.lookup("checkType"));
    if(checkType_) {
        objectType_=word(dict.lookup("objectType"));
    } else {
        objectType_="";
    }
    objectShouldExist_=readBool(dict.lookup("objectShouldExist"));
}

} // namespace Foam

// ************************************************************************* //
