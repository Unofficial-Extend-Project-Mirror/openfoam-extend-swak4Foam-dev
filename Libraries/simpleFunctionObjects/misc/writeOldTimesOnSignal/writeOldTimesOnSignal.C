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
    2008-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "writeOldTimesOnSignal.H"
#include "addToRunTimeSelectionTable.H"

#include "Pstream.H"

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
    sleepSecondsBeforeReraising_(dict.lookupOrDefault<scalar>("sleepSecondsBeforeReraising",60)),
    sigFPE_(dict.lookupOrDefault<bool>("sigFPE",true)),
    sigSEGV_(dict.lookupOrDefault<bool>("sigSEGV",true)),
    sigINT_(dict.lookupOrDefault<bool>("sigINT",false)),
    sigTERM_(dict.lookupOrDefault<bool>("sigTERM",false)),
    sigQUIT_(dict.lookupOrDefault<bool>("sigQUIT",false)),
    sigUSR1_(dict.lookupOrDefault<bool>("sigUSR1",false)),
    sigUSR2_(dict.lookupOrDefault<bool>("sigUSR2",false)),
    alreadyDumped_(false),
    itWasMeWhoReraised_(false)
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
    if(
        Pstream::parRun()
        &&
        !dict.found("sigTERM")
        &&
        !sigTERM_
    ) {
        sigTERM_=true;
        WarningIn("writeOldTimesOnSignalFunctionObject::writeOldTimesOnSignalFunctionObject")
            << "sigTERM unset. Setting it to true so that signal is propagated to other processors"
                << nl << "If this is undesired explicitly set 'sigTERM false;' in "
                << dict.name()
                << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void writeOldTimesOnSignalFunctionObject::sigHandler(int sig) {
    Pout << "Signal " << sig << " encountered" << endl;

    bool toReraise=(
        Pstream::parRun()
        &&
        (
            sig==SIGFPE
            ||
            sig==SIGSEGV
        )
    );

    if(toReraise) {
        Pout << "Going to reraise SIGTERM after writting" << endl;
    }
    if(singleton_!=NULL) {
        writeOldTimesOnSignalFunctionObject &sh=*singleton_;
        Pout << "Resetting old handlers (just in case)" << endl;
        forAll(sh.handlers_,i){
             if(sh.handlers_[i].set()) {
                if(
                    !toReraise
                    ||
                    sh.handlers_[i].sig()!=SIGTERM
                ) {
                   sh.handlers_[i].resetHandler();
                }
            }
        }

        if(sh.alreadyDumped_) {
              Pout << "Other handler dumped already. Exiting" << endl;
        } else {
            if(
                sh.writeCurrent_
                &&
                sh.times_.has(sh.theTime_)
            ) {
                Pout << "Current time in old times. Not writing separately" << endl;
                sh.writeCurrent_=false;
            }
            Pout << "Writing old times:" << endl;
            sh.times_.write();
            if(sh.writeCurrent_) {
                Pout << "Writing current time " << sh.theTime_.value() << endl;
                WarningIn("writeOldTimesOnSignalFunctionObject::sigHandler(int sig)")
                  << "This action may end in a segmentation fault" << endl
                  << "Set 'writeCurrent false;' to avoid this"
                  << endl;

                const_cast<Time&>(sh.theTime_).writeNow();
            } else {
                Pout << "Current time not written."
                     << "Set 'writeCurrent true' if you want that (but it may cause segfaults)" << endl;
            }
            sh.alreadyDumped_=true;
        }
    } else {
        Pout << endl << "Problem: No instance of "
            << "'writeOldTimesOnSignalFunctionObject'." << endl
            << "This can't be" << endl;
    }

    if(toReraise) {
        Pout << "Printstack:" << endl << endl;
        error::printStack(Perr);
        Pout << endl << endl;
        singleton_->itWasMeWhoReraised_=true;
        Pout << "Raising SIGTERM so that other processes will dump too" << endl;
        raise(SIGTERM);
    }
    if(
        sig==SIGTERM
        &&
        singleton_->sleepSecondsBeforeReraising_>0
        &&
        singleton_->itWasMeWhoReraised_
    ) {
        Pout << "Sleeping " << singleton_->sleepSecondsBeforeReraising_ << " before reraising SIGTERM "
            << "to allow other processes to write" << endl;
        sleep(singleton_->sleepSecondsBeforeReraising_);
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
    if(sigTERM_ || Pstream::parRun()){
        handlers_.append(
            SignalHandlerInfo(
                "SIGTERM",
                SIGTERM
            )
        );
        if(!sigTERM_) {
            Info << "Automatically setting sigTERM because this is propagated "
                << "to other processors" << endl;
        }
    } else {
        Info << "To catch the TERM-signal set 'sigTERM true;'" << endl;
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
    if(sigUSR1_) {
        handlers_.append(
            SignalHandlerInfo(
                "SIGUSR1",
                SIGUSR1
            )
        );
    } else {
        Info << "To catch the USR1-signal set 'sigUSR1 true;'" << endl;
    }
    if(sigUSR2_) {
        handlers_.append(
            SignalHandlerInfo(
                "SIGUSR2",
                SIGUSR2
            )
        );
    } else {
        Info << "To catch the USR2-signal set 'sigUSR2 true;'" << endl;
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
    set_=false;
}

} // namespace Foam

// ************************************************************************* //
