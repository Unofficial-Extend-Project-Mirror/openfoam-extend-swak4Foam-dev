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
    2008-2011, 2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "timeManipulationFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(timeManipulationFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

timeManipulationFunctionObject::timeManipulationFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    tolerateAdaptiveTimestep_(false),
    myEndTime_(-1)
{
}

bool timeManipulationFunctionObject::start()
{
    simpleFunctionObject::start();

    tolerateAdaptiveTimestep_=dict_.lookupOrDefault<bool>(
        "tolerateAdaptiveTimestep",false
    );

    if(
        !tolerateAdaptiveTimestep_
        &&
        time().controlDict().lookupOrDefault<bool>("adjustTimeStep",false)
    ) {
        FatalErrorIn("timeManipulationFunctionObject::start()")
            << "'adjustTimeStep' is set. Function object " << name()
                << " manipulates time. This may lead to strange behaviour. "
                << " If you think that is OK set 'tolerateAdaptiveTimestep'"
                << " in " << name()
                << endl
                << exit(FatalError);
    }

    return true;
}

void timeManipulationFunctionObject::write()
{
    scalar newDeltaT=this->deltaT();
    scalar newEndTime=this->endTime();

    scalar minDeltaT=newDeltaT;
    scalar maxDeltaT=newDeltaT;
    reduce(minDeltaT,minOp<scalar>());
    reduce(maxDeltaT,maxOp<scalar>());
    if(minDeltaT!=maxDeltaT) {
        FatalErrorIn("timeManipulationFunctionObject::write()")
            << "Across the processors the minimum " << minDeltaT
                << " and the maximum " << maxDeltaT << " of the new deltaT"
                << " differ by " << maxDeltaT-minDeltaT
                << endl
                << exit(FatalError);

    }
    if(newDeltaT!=time().deltaT().value()) {
        Info << name() << " sets deltaT from " <<
            time().deltaT().value() << " to " << newDeltaT << endl;

        const_cast<Time&>(time()).setDeltaT(newDeltaT);
    }

    scalar minEndTime=newEndTime;
    scalar maxEndTime=newEndTime;
    reduce(minEndTime,minOp<scalar>());
    reduce(maxEndTime,maxOp<scalar>());
    if(minEndTime!=maxEndTime) {
        FatalErrorIn("timeManipulationFunctionObject::write()")
            << "Across the processors the minimum " << minEndTime
                << " and the maximum " << maxEndTime << " of the new endTime"
                << " differ by " << maxEndTime-minEndTime
                << endl
                << exit(FatalError);

    }
    if(newEndTime!=time().endTime().value()) {
        Info << name() << " sets endTime from " <<
            time().endTime().value() << " to " << newEndTime << endl;

        const_cast<Time&>(time()).setEndTime(newEndTime);

        myEndTime_=newEndTime;
    }

    if(
        myEndTime_>-1
        &&
        time().value()>=time().endTime().value()
        &&
        !time().outputTime()
    ) {
        WarningIn("timeManipulationFunctionObject::write()")
            << "Forcing write because we (" << name()
                << ") changed the endTime to "
                << myEndTime_ << " and this is not a write-time"
                << endl;

        const_cast<Time&>(time()).writeNow();
    }
}

scalar timeManipulationFunctionObject::deltaT()
{
    return time().deltaT().value();
}

scalar timeManipulationFunctionObject::endTime()
{
    return time().endTime().value();
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
