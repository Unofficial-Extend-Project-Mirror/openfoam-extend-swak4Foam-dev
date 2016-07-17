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
    2011, 2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "executeIfObjectExistsFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"
#include "argList.H"

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
