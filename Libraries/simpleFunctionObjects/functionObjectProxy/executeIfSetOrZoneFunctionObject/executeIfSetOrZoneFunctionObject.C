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
    2011-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

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
    ),
    loadAndCacheMissingSets_(
        readBool(dict.lookup("loadAndCacheMissingSets"))
    )
{
    // do it here to avoid the superclass-read being read twice
    readSetsAndZones(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
bool executeIfSetOrZoneFunctionObject::hasSet(const word &name)
{
    if(mesh_.foundObject<T>(name)) {
        Dbug << name << " of type " << T::typeName << " already in memory" << endl;
        return true;
    } else {
        Dbug << name << " of type " << T::typeName << " not in memory" << endl;
        if(loadAndCacheMissingSets_) {
            Dbug << "Loading " << name << endl;
            autoPtr<T> s(
                new T(
                    mesh_,
                    name,
                    IOobject::READ_IF_PRESENT
                )
            );

            if(s->headerOk()) {
                Dbug << "Storing " << name << " in mesh" << endl;
                s->store(s);
                return true;
            } else {
                Dbug << "No valid " << name << endl;
            }
        }
        return false;
    }
}

bool executeIfSetOrZoneFunctionObject::condition()
{
    forAllConstIter(dictionary,setsAndZones_,it) {
        const entry &e=*it;
	const word &name=e.keyword();
	word typ(e.stream());

        Dbug << "Typ: " << typ << " Name: " << name << endl;

	if(typ=="cellZone") {
#ifdef FOAM_ZONEMESH_HAS_NO_FINDINDEX
	    if(mesh_.cellZones().findZoneID(name)<0) {
#else
	    if(mesh_.cellZones().findIndex(name)<0) {
#endif
                Dbug << "No " << name << " of type " << typ << endl;
	        return false;
	    }
	} else if(typ=="faceZone") {
#ifdef FOAM_ZONEMESH_HAS_NO_FINDINDEX
            if(mesh_.faceZones().findZoneID(name)<0) {
#else
            if(mesh_.faceZones().findIndex(name)<0) {
#endif
                Dbug << "No " << name << " of type " << typ << endl;
	        return false;
	    }
	} else if(typ=="pointZone") {
#ifdef FOAM_ZONEMESH_HAS_NO_FINDINDEX
            if(mesh_.pointZones().findZoneID(name)<0) {
#else
	    if(mesh_.pointZones().findIndex(name)<0) {
#endif
                Dbug << "No " << name << " of type " << typ << endl;
	        return false;
	    }
	} else if(typ=="cellSet") {
	    if(!this->hasSet<cellSet>(name)) {
                Dbug << "No " << name << " of type " << typ << endl;
	        return false;
	    }
	} else if(typ=="faceSet") {
	    if(!this->hasSet<faceSet>(name)) {
                Dbug << "No " << name << " of type " << typ << endl;
	        return false;
	    }
	} else if(typ=="pointSet") {
	    if(!this->hasSet<pointSet>(name)) {
                Dbug << "No " << name << " of type " << typ << endl;
	        return false;
	    }
	} else {
	    FatalErrorIn("executeIfSetOrZoneFunctionObject::condition()")
	        << "Unimplemented type " << typ << " for " << name << endl
                    << "Implemented are faceZone, faceSet, cellZone, cellSet, pointZone, pointSet" << endl
	        << abort(FatalError);
	}
    }
    return true;
}

void executeIfSetOrZoneFunctionObject::readSetsAndZones(const dictionary& dict)
{
    setsAndZones_=dict.subDict("setsAndZones");
    loadAndCacheMissingSets_=
        readBool(dict.lookup("loadAndCacheMissingSets"));
}

bool executeIfSetOrZoneFunctionObject::read(const dictionary& dict)
{
    readSetsAndZones(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

} // namespace Foam

// ************************************************************************* //
