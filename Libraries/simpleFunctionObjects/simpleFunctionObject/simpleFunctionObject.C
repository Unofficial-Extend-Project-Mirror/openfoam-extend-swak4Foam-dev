//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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

\*---------------------------------------------------------------------------*/

#include "simpleFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(simpleFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

simpleFunctionObject::simpleFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(name),
    verbose_(
        dict.found("verbose")
        ? readBool(dict.lookup("verbose"))
        : false
    ),
    writeDebug_(
        dict.found("writeDebug")
        ? readBool(dict.lookup("writeDebug"))
        : false
    ),
    after_(
        dict.found("after")
        ? readScalar(dict.lookup("after"))
        : t.startTime().value()-1
    ),
    timeSteps_(0),
    outputInterval_(
        dict.found("outputInterval")
        ? readLabel(dict.lookup("outputInterval"))
        : 1
    ),
    time_(t),
    dict_(dict),
    regionName_(
        dict_.found("region") 
        ? dict_.lookup("region") 
        : polyMesh::defaultRegion
    ),
    obr_(time_.lookupObject<objectRegistry>(regionName_))
{
    if(regionName_==polyMesh::defaultRegion) {
        regionString_ = "";
    } else {
        regionString_ = " Region: "+regionName_+" :";
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool simpleFunctionObject::start()
{
    timeSteps_=outputInterval_;
    
    return true;
}

bool simpleFunctionObject::outputTime(const bool forceWrite)
{
    return 
        (
            ((outputInterval_>0) && (timeSteps_>=outputInterval_))
            &&
            time_.time().value()>=after_
        ) 
        ||
        forceWrite
        ;
}

bool simpleFunctionObject::execute(const bool forceWrite)
{
    if(time_.time().value()<after_) {
        return true;
    }

    timeSteps_++;

    if(this->outputTime(forceWrite)) {
        timeSteps_=0;
        write();
        flush();
    }

    return true;
}

void simpleFunctionObject::flush()
{
}

bool simpleFunctionObject::read(const dictionary& dict)
{
    if (dict != dict_)
    {
        dict_ = dict;

        if(dict_.found("outputInterval")) {
            outputInterval_=readLabel(dict.lookup("outputInterval"));
        }
        if(dict_.found("after")) {
            after_=readScalar(dict.lookup("after"));
        }

        return start();
    }
    else
    {
        return false;
    }
}

} // namespace Foam

// ************************************************************************* //
