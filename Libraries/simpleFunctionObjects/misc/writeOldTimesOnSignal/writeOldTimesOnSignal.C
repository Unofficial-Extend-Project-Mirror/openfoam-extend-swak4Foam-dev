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
    2008-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "writeOldTimesOnSignal.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(writeOldTimesOnSignalFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        writeOldTimesOnSignalFunctionObject,
        dictionary
    );

    writeOldTimesOnSignalFunctionObject *writeOldTimesOnSignalFunctionObject::singleton_=NULL;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeOldTimesOnSignalFunctionObject::writeOldTimesOnSignalFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(name),
    times_(dict),
    theTime_(t),
    writeCurrent_(
        readBool(dict.lookup("writeCurrent"))
    ),
    sigFPE_(dict.lookupOrDefault<bool>("sigFPE",true)),
    sigSEGV_(dict.lookupOrDefault<bool>("sigSEGV",true)),
    sigINT_(dict.lookupOrDefault<bool>("sigINT",false)),
    sigQUIT_(dict.lookupOrDefault<bool>("sigQUIT",false))
{
    if(writeCurrent_) {
        WarningIn("writeOldTimesOnSignalFunctionObject::writeOldTimesOnSignalFunctionObject")
            << "'writeCurrent' was set. This may lead to uncaught segmentation faults"
                << endl;
    }

    if(singleton_!=NULL) {
        FatalErrorIn("writeOldTimesOnSignalFunctionObject::writeOldTimesOnSignalFunctionObject")
            << "Only one instance of 'writeOldTimesOnSignal' may be used in one simulation"
                << endl
                << exit(FatalError);
    } else {
        singleton_=this;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void writeOldTimesOnSignalFunctionObject::sigHandler(int sig) {
    Pout << "Signal " << sig << " encountered" << endl;

    if(singleton_!=NULL) {
        writeOldTimesOnSignalFunctionObject &sh=*singleton_;
        Pout << "Resetting old handlers (just in case)" << endl;
        forAll(sh.handlers_,i){
            sh.handlers_[i].resetHandler();
        }

        Pout << "Writing old times:" << endl;
        sh.times_.write();
        if(sh.writeCurrent_) {
            Pout << "Writing current time" << endl;
            WarningIn("writeOldTimesOnSignalFunctionObject::sigHandler(int sig)")
                << "This action may end in a segmentation fault" << endl
                    << "Set 'writeCurrent false;' to avoid this"
                    << endl;

            const_cast<Time&>(sh.theTime_).writeNow();
        } else {
            Pout << "Current time not written."
                << "Set 'writeCurrent true' if you want that (but it may cause segfaults)" << endl;
        }
    } else {
        Pout << endl << "Problem: No instance of "
            << "'writeOldTimesOnSignalFunctionObject'." << endl
            << "This can't be" << endl;
    }

    Pout << "Reraising original signal" << endl;

    raise(sig);
}

bool writeOldTimesOnSignalFunctionObject::start()
{
    if(sigFPE_) {
        handlers_.append(
            SignalHandlerInfo(
                "SIGFPE",
                SIGFPE
            )
        );
        Info << "To switch this off set 'sigFPE false;'" << endl;
    }
    if(sigSEGV_) {
        handlers_.append(
            SignalHandlerInfo(
                "SIGSEGV",
                SIGSEGV
            )
        );
        Info << "To switch this off set 'sigSEGV false;'" << endl;
    }
    if(sigINT_){
        handlers_.append(
            SignalHandlerInfo(
                "SIGINT",
                SIGINT
            )
        );
    } else {
        Info << "To catch Ctrl-C set 'sigINT true;'" << endl;
    }
    if(sigQUIT_) {
        handlers_.append(
            SignalHandlerInfo(
                "SIGQUIT",
                SIGQUIT
            )
        );
    } else {
        Info << "To catch the QUIT-signal set 'sigQUIT true;'" << endl;
    }

    handlers_.shrink();
    Info << handlers_.size() << " signal handlers installed" << endl;
    return true;
}


#ifdef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
bool writeOldTimesOnSignalFunctionObject::execute()
#else
bool writeOldTimesOnSignalFunctionObject::execute(const bool forceWrite)
#endif
{
    times_.copy(theTime_);

    return true;
}

bool writeOldTimesOnSignalFunctionObject::read(const dictionary& dict)
{
    return true;
}

writeOldTimesOnSignalFunctionObject::SignalHandlerInfo::SignalHandlerInfo(
    word name,
    int sig
)
    :
    name_(name),
    sig_(sig),
    set_(false)
{
    Pout << "Set " << name_ << "(" << sig_ << ") signal handler" << endl;

    struct sigaction newAction;
    newAction.sa_handler = sigHandler;
    newAction.sa_flags = SA_NODEFER;
    sigemptyset(&newAction.sa_mask);
    if (sigaction(sig_, &newAction, &oldAction_) < 0) {
        FatalErrorIn
        (
            "writeOldTimesOnSignalFunctionObject::SignalHandlerInfo::SignalHandlerInfo"
        )   << "Cannot set " << name_ << "(" << sig_ << ") trapping"
            << abort(FatalError);
    }
    set_=true;
}

void writeOldTimesOnSignalFunctionObject::SignalHandlerInfo::resetHandler()
{
    Pout << "Unset " << name_ << "(" << sig_ << ") signal handler" << endl;
    if(!set_) {
        Pout << "Handler has not been set in the first place. Doing nothing"
            << endl;
        return;
    }
    if (sigaction(sig_, &oldAction_, NULL) < 0) {
        FatalErrorIn
            (
                "writeOldTimesOnSignalFunctionObject::SignalHandlerInfo::SignalHandlerInfo"
            )   << "Cannot unset " << name_ << "(" << sig_ << ") trapping"
                << abort(FatalError);
    }
}

} // namespace Foam

// ************************************************************************* //
