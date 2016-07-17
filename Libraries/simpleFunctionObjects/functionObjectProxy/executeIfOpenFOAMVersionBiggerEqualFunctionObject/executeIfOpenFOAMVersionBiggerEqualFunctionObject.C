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

#include "executeIfOpenFOAMVersionBiggerEqualFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"
#include "argList.H"

#include "foamVersion4swak.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(executeIfOpenFOAMVersionBiggerEqualFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfOpenFOAMVersionBiggerEqualFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfOpenFOAMVersionBiggerEqualFunctionObject::executeIfOpenFOAMVersionBiggerEqualFunctionObject
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
    // do it here to avoid the superclass-read being read twice
    readData(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

label executeIfOpenFOAMVersionBiggerEqualFunctionObject::toLabel(const string &v) {
    if(v=="") {
        return -1;
    } else if (v=="x") {
        return labelMax;
    } else {
        IStringStream i(v);

        return readLabel(i);
    }
}

bool executeIfOpenFOAMVersionBiggerEqualFunctionObject::condition()
{
#define TOSTRING(x) string(#x)

    label foamVersionPatch=-1;
    if(TOSTRING(FOAM_VERSION4SWAK_PATCH)!="x") {
        foamVersionPatch=toLabel(TOSTRING(FOAM_VERSION4SWAK_PATCH));
    }
    if(majorVersion_>FOAM_VERSION4SWAK_MAJOR) {
        return false;
    } else if(majorVersion_<FOAM_VERSION4SWAK_MAJOR) {
        return true;
    } else if(minorVersion_>FOAM_VERSION4SWAK_MINOR) {
        return false;
    } else if(minorVersion_<FOAM_VERSION4SWAK_MINOR) {
        return true;
    } else if(foamVersionPatch<0) {
        return true;
    } else {
        return patchVersion_<=foamVersionPatch;
    }
#undef TOSTRING
}

void executeIfOpenFOAMVersionBiggerEqualFunctionObject::readData(const dictionary& dict)
{
    majorVersion_=toLabel(string(dict.lookup("majorVersion")));
    minorVersion_=toLabel(string(dict.lookup("minorVersion")));
    patchVersion_=toLabel(dict.lookupOrDefault<string>("patchVersion",""));
}

bool executeIfOpenFOAMVersionBiggerEqualFunctionObject::read(const dictionary& dict)
{
    readData(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

} // namespace Foam

// ************************************************************************* //
