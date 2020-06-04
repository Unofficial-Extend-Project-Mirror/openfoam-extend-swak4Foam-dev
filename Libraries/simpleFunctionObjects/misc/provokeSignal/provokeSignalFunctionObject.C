/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2014-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id: provokeSignalFunctionObject.C,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
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

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    const Enum<Foam::provokeSignalFunctionObject::possibleSignals>
    provokeSignalFunctionObject::possibleSignalsNames_
    ({
        {possibleSignals::sigFPE,"FPE"},
        {possibleSignals::sigSEGV,"SEGV"},
        {possibleSignals::sigINT,"INT"},
        {possibleSignals::sigTERM,"TERM"},
        {possibleSignals::sigQUIT,"QUIT"},
        {possibleSignals::sigUSR1,"USR1"},
        {possibleSignals::sigUSR2,"USR2"}
    });
#else
    template<>
    const char* NamedEnum<Foam::provokeSignalFunctionObject::possibleSignals,8>::names[]=
    {
        "FPE",
        "SEGV",
        "INT",
        "TERM",
        "QUIT",
        "USR1",
        "USR2",
        "FoamFatal"
    };
    const NamedEnum<provokeSignalFunctionObject::possibleSignals,8> provokeSignalFunctionObject::possibleSignalsNames_;
#endif

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
        labelHashSet allProcs(
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

void provokeSignalFunctionObject::writeSimple()
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
            case FoamFatal:
                FatalErrorInFunction
                    << "Completely unnecessary error"
                        << endl
                        << exit(FatalError);
            default:
                FatalErrorIn("provokeSignalFunctionObject::writeSimple()")
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
