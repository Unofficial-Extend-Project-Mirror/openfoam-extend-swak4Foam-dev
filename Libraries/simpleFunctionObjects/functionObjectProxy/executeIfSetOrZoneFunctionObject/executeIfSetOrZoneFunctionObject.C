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
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "executeIfSetOrZoneFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "cellZone.H"
#include "faceZone.H"
#include "pointZone.H"
#include "cellSet.H"
#include "faceSet.H"
#include "pointSet.H"

#include "polyMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(executeIfSetOrZoneFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfSetOrZoneFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfSetOrZoneFunctionObject::executeIfSetOrZoneFunctionObject
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
    ),
    mesh_(
	dynamicCast<const polyMesh&>(obr())
    )
{
    // do it here to avoid the superclass-read being read twice
    readSetsAndZones(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool executeIfSetOrZoneFunctionObject::condition()
{
    forAllConstIter(dictionary,setsAndZones_,it) {
        const entry &e=*it;
	const word &name=e.keyword();
	word typ(e.stream());
	if(typ=="cellZone") {
	    if(mesh_.cellZones().findIndex(name)<0) {
	        return false;
	    } 
	} else if(typ=="faceZone") {
	    if(mesh_.faceZones().findIndex(name)<0) {
	        return false;
	    } 
	} else if(typ=="pointZone") {
	    if(mesh_.pointZones().findIndex(name)<0) {
	        return false;
	    } 
	} else {
	    FatalErrorIn("executeIfSetOrZoneFunctionObject::condition()")
	        << "Unimplemented type " << typ << " for " << name << endl
	        << abort(FatalError);
	}
    }
    return true;
}

void executeIfSetOrZoneFunctionObject::readSetsAndZones(const dictionary& dict)
{
    setsAndZones_=dict.subDict("setsAndZones");
}

bool executeIfSetOrZoneFunctionObject::read(const dictionary& dict)
{
    readSetsAndZones(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

} // namespace Foam

// ************************************************************************* //
