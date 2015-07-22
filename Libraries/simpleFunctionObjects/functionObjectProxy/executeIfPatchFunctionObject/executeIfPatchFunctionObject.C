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

#include "executeIfPatchFunctionObject.H"
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
    defineTypeNameAndDebug(executeIfPatchFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfPatchFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfPatchFunctionObject::executeIfPatchFunctionObject
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
    readPatches(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool executeIfPatchFunctionObject::condition()
{
    forAll(patchNames_,i) {
        if(mesh_.boundaryMesh().findPatchID(patchNames_[i])<0) {
            return false;
        }
    }
    return true;
}

void executeIfPatchFunctionObject::readPatches(const dictionary& dict)
{
    patchNames_=wordList(dict.lookup("patchNames"));
}

bool executeIfPatchFunctionObject::read(const dictionary& dict)
{
    readPatches(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

} // namespace Foam

// ************************************************************************* //
