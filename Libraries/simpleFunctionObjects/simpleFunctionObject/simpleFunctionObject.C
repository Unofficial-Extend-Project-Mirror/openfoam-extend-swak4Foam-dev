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
    2008-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
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

template<>
const char* NamedEnum<Foam::simpleFunctionObject::outputControlMode,3>::names[]=
{
    "timestep",
    "deltaT",
    "outputTime"
};
const NamedEnum<simpleFunctionObject::outputControlMode,3> simpleFunctionObject::outputControlModeNames_;


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
    outputControlMode_(
        outputControlModeNames_[
            dict.lookupOrDefault<word>("outputControlMode","timestep")
        ]
    ),
    outputInterval_(
        dict.found("outputInterval")
        ? readLabel(dict.lookup("outputInterval"))
        : 1
    ),
    outputDeltaT_(1.),
    time_(t),
    lastWrite_(time_.value()),
    dict_(dict),
    regionName_(
        dict_.found("region")
        ? dict_.lookup("region")
        : polyMesh::defaultRegion
    ),
    obr_(time_.lookupObject<objectRegistry>(regionName_))
{
    if(!dict.found("outputControlMode")) {
        WarningIn("simpleFunctionObject::simpleFunctionObject")
            << "'outputControlMode' not found in " << this->name() << endl
                << "Assuming: " << outputControlModeNames_[outputControlMode_]
                << endl;
    }
    switch(outputControlMode_) {
        case ocmTimestep:
            if(!dict.found("outputInterval")) {
                WarningIn("simpleFunctionObject::simpleFunctionObject")
                    << "'outputInterval' not found in " << this->name() << endl
                        << "Assuming: " << outputInterval_
                        << endl;
            }
            break;
        case ocmDeltaT:
            outputDeltaT_=readScalar(dict.lookup("outputDeltaT"));
            break;
        default:
            break;
    }
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
    if(time_.time().value()<after_) {
        return false;
    }
    bool doOutput=false;

    switch(outputControlMode_) {
        case ocmTimestep:
            if((outputInterval_>0) && (timeSteps_>=outputInterval_)) {
                doOutput=true;
            }
            break;
        case ocmDeltaT:
            {
                // factor (1-SMALL) is necessary to 'hit' exact timesteps
                scalar now=time_.value()*(1-SMALL);
                scalar dt=time_.deltaT().value();
                label stepNow=label(now/outputDeltaT_);
                label stepNext=label((now+dt)/outputDeltaT_);
                if(
                    stepNow!=stepNext
                    ||
                    (lastWrite_+outputDeltaT_) < now
                ) {
                    doOutput=true;
                    lastWrite_=outputDeltaT_*int((now+dt)/outputDeltaT_);
                }
            }
            break;
        case ocmOutputTime:
            doOutput=time_.outputTime();
            break;
        default:
            FatalErrorIn("simpleFunctionObject::outputTime()")
                << "'outputControlMode' not implemented in " << name() << endl
                    << "Mode: " << outputControlModeNames_[outputControlMode_]
                    << endl
                    << exit(FatalError);
    }
    return doOutput || forceWrite;
}

bool simpleFunctionObject::execute(const bool forceWrite)
{
    if(debug) {
        Info << name() << "::execute() - Entering" << endl;
    }
    if(time_.time().value()<after_) {
        if(debug) {
            Info << name() << "::execute() - Leaving - after" << endl;
        }
        return true;
    }

    timeSteps_++;

    if(this->outputTime(forceWrite)) {
        if(debug) {
            Info << name() << "::execute() - outputTime" << endl;
        }
        timeSteps_=0;
        write();
        flush();
    }

    if(debug) {
        Info << name() << "::execute() - Leaving" << endl;
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
