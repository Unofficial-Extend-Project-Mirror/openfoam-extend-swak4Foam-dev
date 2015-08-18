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
    2012-2013, 2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "writeAndEndFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(writeAndEndFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeAndEndFunctionObject::writeAndEndFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    isStopped_(false),
    storeAndWritePreviousState_(
        dict.lookupOrDefault<bool>("storeAndWritePreviousState",false)
    )
{
    if(storeAndWritePreviousState_) {
        lastTimes_.set(
            new TimeCloneList(
                dict
            )
        );
    } else if(!dict.found("storeAndWritePreviousState")){
        WarningIn("writeAndEndFunctionObject::writeAndEndFunctionObject")
            << "'storeAndWritePreviousState' unset in " << dict.name()
                << endl;
    }
}

bool writeAndEndFunctionObject::start()
{
    if(debug) {
        Info << name() << "::start() - Entering" << endl;
    }

    simpleFunctionObject::start();

    if(debug) {
        Info << name() << "::start() - Leaving" << endl;
    }

    return true;
}

void writeAndEndFunctionObject::write()
{
    if(debug) {
        Info << name() << "::write() - Entering" << endl;
    }
    if(isStopped()) {
        if(debug) {
            Info << name() << "::write() - isStopped" << endl;
        }
        return;
    }
    if(
        this->endRunNow()
    ) {
        if(debug) {
            Info << name() << "::write() - stopping" << endl;
        }
        isStopped_=true;

        Info << "Ending run because of functionObject " << this->name() << endl;
        Info << "Writing current state" << endl;
        const_cast<Time &>(time()).writeAndEnd();
        if(lastTimes_.valid()) {
            Pout << "Writing old times" << endl;
            lastTimes_->write();
        }
    }
    if(lastTimes_.valid()) {
        Dbug << "Storing current time" << endl;
        lastTimes_->copy(time());
    }
    if(debug) {
        Info << name() << "::write() - Leaving" << endl;
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
