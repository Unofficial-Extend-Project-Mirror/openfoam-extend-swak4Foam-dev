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
    2008-2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "simpleFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(simpleFunctionObject, 0);

template<>
const char* NamedEnum<Foam::simpleFunctionObject::outputControlModeType,7>::names[]=
{
    "timeStep",
    "deltaT",
    "outputTime",
    "startup",
    "outputTimeAndStartup",
    "timeStepAndStartup",
    "deltaTAndStartup"
};
const NamedEnum<simpleFunctionObject::outputControlModeType,7> simpleFunctionObject::outputControlModeTypeNames_;


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
        outputControlModeTypeNames_[
            dict.lookupOrDefault<word>("outputControlMode","timeStep")
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
    started_(false),
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
                << "Assuming: " << outputControlModeTypeNames_[outputControlMode_]
                << endl;
    }
    switch(outputControlMode_) {
        case ocmTimestep:
        case ocmTimestepAndStartup:
            if(!dict.found("outputInterval")) {
                WarningIn("simpleFunctionObject::simpleFunctionObject")
                    << "'outputInterval' not found in " << this->name() << endl
                        << "Assuming: " << outputInterval_
                        << endl;
            }
            break;
        case ocmDeltaT:
        case ocmDeltaTAndStartup:
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
    Dbug << name() << "::start() - Entering" << endl;

    if(started_) {
        // Break infinite recursion
        return true;
    }
    started_=true;

    timeSteps_=outputInterval_;

    if(
        outputControlMode()==ocmStartup
        ||
        outputControlMode()==ocmOutputTimeAndStartup
        ||
        outputControlMode()==ocmTimestepAndStartup
        ||
        outputControlMode()==ocmDeltaTAndStartup
    ) {
        write();
        flush();
    }

    return true;
}

bool simpleFunctionObject::outputTime(const bool forceWrite)
{
    Dbug << name() << "::output() - Entering" << endl;

    if(time_.time().value()<after_) {
        return false;
    }
    bool doOutput=false;

    switch(outputControlMode_) {
        case ocmTimestep:
        case ocmTimestepAndStartup:
            if((outputInterval_>0) && (timeSteps_>=outputInterval_)) {
                doOutput=true;
            }
            break;
        case ocmDeltaT:
        case ocmDeltaTAndStartup:
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
        case ocmOutputTimeAndStartup:
            doOutput=time_.outputTime();
            break;
        case ocmStartup:
            doOutput=false;
            break;
        default:
            FatalErrorIn("simpleFunctionObject::outputTime()")
                << "'outputControlMode' not implemented in " << name() << endl
                    << "Mode: " << outputControlModeTypeNames_[outputControlMode_]
                    << endl
                    << exit(FatalError);
    }
    return doOutput || forceWrite;
}

bool simpleFunctionObject::execute(const bool forceWrite)
{
    Dbug << name() << "::execute() - Entering" << endl;
    if(time_.time().value()<after_) {
        Dbug << name() << "::execute() - Leaving - after" << endl;
        return true;
    }

    timeSteps_++;

    if(this->outputTime(forceWrite)) {
        Dbug << name() << "::execute() - outputTime" << endl;
        timeSteps_=0;
        write();
        flush();
    }

    Dbug << name() << "::execute() - Leaving" << endl;
    return true;
}

void simpleFunctionObject::flush()
{
}

bool simpleFunctionObject::read(const dictionary& dict)
{
    Dbug << name() << "::read() - Entering" << endl;

    if (dict != dict_)
    {
        dict_ = dict;

        if(dict_.found("outputInterval")) {
            outputInterval_=readLabel(dict.lookup("outputInterval"));
        }
        if(dict_.found("after")) {
            after_=readScalar(dict.lookup("after"));
        }

        bool isStart=start();

        return isStart;
    }
    else
    {
        return false;
    }
}

} // namespace Foam

// ************************************************************************* //
