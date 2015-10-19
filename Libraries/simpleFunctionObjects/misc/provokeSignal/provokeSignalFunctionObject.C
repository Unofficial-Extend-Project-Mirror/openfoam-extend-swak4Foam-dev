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
    2014-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "provokeSignalFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "swakTime.H"

#include <signal.h>

#include "HashSet.H"

#include "objectRegistry.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(provokeSignalFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        provokeSignalFunctionObject,
        dictionary
    );

    template<>
    const char* NamedEnum<Foam::provokeSignalFunctionObject::possibleSignals,7>::names[]=
    {
        "FPE",
        "SEGV",
        "INT",
        "TERM",
        "QUIT",
        "USR1",
        "USR2"
    };
    const NamedEnum<provokeSignalFunctionObject::possibleSignals,7> provokeSignalFunctionObject::possibleSignalsNames_;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

provokeSignalFunctionObject::provokeSignalFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    signalToRaise_(
        possibleSignalsNames_[
            word(dict.lookup("signalToRaise"))
        ]
    ),
    timeToRaise_(
        readScalar(dict.lookup("timeToRaise"))
    ),
    raiseOnThisProc_(false)
{
    if(Pstream::parRun()) {
        HashSet<label> allProcs(
            labelList(
                dict.lookup("processorsToRaiseSignal")
            )
        );
        raiseOnThisProc_=allProcs.found(
            Pstream::myProcNo()
        );
    } else {
        if(!dict.found("processorsToRaiseSignal")) {
            WarningIn("provokeSignalFunctionObject::provokeSignalFunctionObject")
                << "No entry 'processorsToRaiseSignal' in " << dict.name()
                << nl << "Not needed now but needed in parallel runs" << endl;
        }
        raiseOnThisProc_=true;
    }

    if(raiseOnThisProc_) {
        Pout << endl;
        Pout << "Will raise signal " << possibleSignalsNames_[signalToRaise_]
            << " at time " << timeToRaise_
            << " and there is nothing you can do about it. "
            << "In fact the only possible use of this is testing" << endl;
        Pout << endl;
    }
}

void provokeSignalFunctionObject::write()
{
    if(
        raiseOnThisProc_
        &&
        obr_.time().value()>=timeToRaise_
    ) {
        Pout << endl
            << "The time has come. Raising "
            << possibleSignalsNames_[signalToRaise_] << endl
            << "It was nice knowing you"
            << endl;


       switch(signalToRaise_) {
            case sigFPE:
                raise(SIGFPE);
                break;
            case sigSEGV:
                raise(SIGSEGV);
                break;
            case sigINT:
                raise(SIGINT);
                break;
            case sigTERM:
                raise(SIGTERM);
                break;
            case sigQUIT:
                raise(SIGQUIT);
                break;
            case sigUSR1:
                raise(SIGUSR1);
                break;
            case sigUSR2:
                raise(SIGUSR2);
                break;
            default:
                FatalErrorIn("provokeSignalFunctionObject::write()")
                    << "Unimplemented signal "
                        << possibleSignalsNames_[signalToRaise_]
                        << endl;
        }
        Pout << "We signaled (this should not be seen)" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
