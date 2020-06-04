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
    2008-2011, 2013-2016, 2018-2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: conditionDrivenWritingFunctionObject.C,v 78b0d113b99b 2013-02-25 16:12:41Z bgschaid $
\*---------------------------------------------------------------------------*/

#include "conditionDrivenWritingFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(conditionDrivenWritingFunctionObject, 0);

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    const Enum<Foam::conditionDrivenWritingFunctionObject::writeControlModeType>
    conditionDrivenWritingFunctionObject::writeControlModeTypeNames_
    ({
        {writeControlModeType::scmWriteAlways,"always"},
        {writeControlModeType::scmWriteNTimesteps,"timesteps"},
        {writeControlModeType::scmWriteIntervall,"intervall"},
        {writeControlModeType::scmWriteUntilSwitch,"untilSwitch"}
    });

    const Enum<Foam::conditionDrivenWritingFunctionObject::cooldownModeType>
    conditionDrivenWritingFunctionObject::cooldownModeTypeNames_
    ({
        {cooldownModeType::cdmNoCooldown,"no"},
        {cooldownModeType::cdmNTimesteps,"timesteps"},
        {cooldownModeType::cdmIntervall,"intervall"},
        {cooldownModeType::cdmRetrigger,"retrigger"}
    });
#else
    template<>
    const char* NamedEnum<Foam::conditionDrivenWritingFunctionObject::writeControlModeType,4>::names[]=
    {
        "always",
        "timesteps",
        "intervall",
        "untilSwitch"
    };
    const NamedEnum<conditionDrivenWritingFunctionObject::writeControlModeType,4> conditionDrivenWritingFunctionObject::writeControlModeTypeNames_;

    template<>
    const char* NamedEnum<Foam::conditionDrivenWritingFunctionObject::cooldownModeType,4>::names[]=
    {
        "no",
        "timesteps",
        "intervall",
        "retrigger"
    };
    const NamedEnum<conditionDrivenWritingFunctionObject::cooldownModeType,4> conditionDrivenWritingFunctionObject::cooldownModeTypeNames_;
#endif



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

conditionDrivenWritingFunctionObject::conditionDrivenWritingFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    writeControlMode_(
        writeControlModeTypeNames_[
            word(dict.lookup("writeControlMode"))
        ]
    ),
    cooldownMode_(
        cooldownModeTypeNames_[
            word(dict.lookup("cooldownMode"))
        ]
    ),
    storeAndWritePreviousState_(
        readBool(
            dict.lookup("storeAndWritePreviousState")
        )
    ),
    theState_(stateWaiting),
    writeTimesteps_(-1),
    writeIntervall_(-1),
    cooldownTimesteps_(-1),
    cooldownIntervall_(-1),
    timestepForStateChange_(t.timeIndex()),
    timeForStateChange_(t.value())
{
    switch(writeControlMode_) {
        case scmWriteNTimesteps:
            writeTimesteps_=readLabel(dict.lookup("writeTimesteps"));
            break;
        case scmWriteIntervall:
            writeIntervall_=readScalar(dict.lookup("writeIntervall"));
            break;
        case scmWriteUntilSwitch:
            // to be implemented by subclass
            break;
        default:
            // do nothing
            break;
    }
    switch(cooldownMode_) {
        case cdmNTimesteps:
            cooldownTimesteps_=readLabel(dict.lookup("cooldownTimesteps"));
            break;
        case cdmIntervall:
            cooldownIntervall_=readScalar(dict.lookup("cooldownIntervall"));
            break;
        case cdmRetrigger:
            // to be implemented by subclass
            break;
        default:
            // do nothing
            break;
    }
    if(storeAndWritePreviousState_) {
        lastTimes_.set(
            new TimeCloneList(
                dict
            )
        );
    }
}

bool conditionDrivenWritingFunctionObject::checkStopWriting()
{
    FatalErrorIn("conditionDrivenWritingFunctionObject::checkStopWriting")
        << name() << " does not implement 'untilSwitch' for 'writeControlMode'"
            << endl
            << exit(FatalError);

    return false;
}

bool conditionDrivenWritingFunctionObject::checkStopCooldown()
{
    FatalErrorIn("conditionDrivenWritingFunctionObject::checkStopCooldown")
        << name() << " does not implement 'retrigger' for 'cooldownMode'"
            << endl
            << exit(FatalError);

    return false;
}

bool conditionDrivenWritingFunctionObject::alreadyWritten(word timename)
{
    // TODO: implement properly
    return false;
}

void conditionDrivenWritingFunctionObject::storePreviousState()
{
    if(lastTimes_.valid()) {
        lastTimes_->copy(time());
    } else {
        WarningIn("conditionDrivenWritingFunctionObject::storePreviousState")
            << "Logic error"
                << endl;
    }
}

void conditionDrivenWritingFunctionObject::writePreviousState()
{
    if(lastTimes_.valid()) {
        lastTimes_->write();
    } else {
        WarningIn("conditionDrivenWritingFunctionObject::writePreviousState")
            << "Logic error"
                << endl;
    }
}

void conditionDrivenWritingFunctionObject::writeNow()
{
    Info << name() << " triggered writing of t=" << time().timeName() << endl;
    if(!alreadyWritten(time().timeName())) {
        Info << name() << ": Writing ...." << endl;
        bool result=const_cast<Time&>(time()).writeNow();
        Dbug << "Write now " << result << endl;
    } else {
        Info << name() << ": Already written. Skipping" << endl;
    }

    if(storeAndWritePreviousState_) {
        Info << name() << ": Attempt to write previous timestep" << endl;
        writePreviousState();
    }
}

bool conditionDrivenWritingFunctionObject::start()
{
    simpleFunctionObject::start();

    if(cooldownMode_==cdmRetrigger) {
        // provoke 'not implemented'
        bool result=checkStopCooldown();
        Dbug << "Stop cooldown: " << result << endl;
    }

    if(writeControlMode_==scmWriteUntilSwitch) {
        // provoke 'not implemented'
        bool result=checkStopWriting();
        Dbug << "Stop writing: " << result << endl;
    }

    if(storeAndWritePreviousState_) {
        storePreviousState();
    }

    return true;
}

bool conditionDrivenWritingFunctionObject::checkCooldown()
{
    switch(cooldownMode_) {
        case cdmNoCooldown:
            return true;
        case cdmNTimesteps:
            return time().timeIndex()>=timestepForStateChange_;
            break;
        case cdmIntervall:
            return time().value()>timeForStateChange_;
            break;
        case cdmRetrigger:
            return checkStopCooldown();
            break;
        default:
            FatalErrorIn("conditionDrivenWritingFunctionObject::write")
                << "Unsupported cooldown mode"
                    << endl
                    << exit(FatalError);
    }
    return false;
}

bool conditionDrivenWritingFunctionObject::checkWrite()
{
    if(checkStartWriting()) {
        Info << name() << " starts writing" << endl;
        theState_=stateWriting;
        switch(writeControlMode_) {
            case scmWriteAlways:
                break;
            case scmWriteNTimesteps:
                Info << name() << ": Writing for " << writeTimesteps_
                    << endl;
                timestepForStateChange_=time().timeIndex()+writeTimesteps_-1;
                break;
            case scmWriteIntervall:
                timeForStateChange_=time().value()+writeIntervall_;
                Info << name() << ": Writing till " << timeForStateChange_
                    << endl;
                break;
            case scmWriteUntilSwitch:
                Info << name() << ": writing until switched off"
                    << endl;
                break;
            default:
                FatalErrorIn("conditionDrivenWritingFunctionObject::write")
                    << "Unimplemented 'writeControlMode'"
                        << endl
                        << exit(FatalError);
        }
        return true;
    } else {
        return false;
    }
}

void conditionDrivenWritingFunctionObject::writeSimple()
{
    bool doWrite=false;

    switch(theState_) {
        case stateWaiting:
            doWrite=checkWrite();
            break;
        case stateWriting:
            switch(writeControlMode_) {
                case scmWriteAlways:
                    doWrite=checkStartWriting();
                    break;
                case scmWriteNTimesteps:
                    doWrite=(time().timeIndex()<=timestepForStateChange_);
                    break;
                case scmWriteIntervall:
                    doWrite=(time().value()<=timeForStateChange_);
                    break;
                case scmWriteUntilSwitch:
                    doWrite=!checkStopWriting();
                    break;
                default:
                    FatalErrorIn("conditionDrivenWritingFunctionObject::write")
                        << "Unimplemented 'writeControlMode'"
                            << endl
                            << exit(FatalError);
            }
            if(!doWrite) {
                Info << name() << ": Stopped writting. Starting cooldown" << endl;
                theState_=stateStartCooldown;
            }
            break;
        case stateCooldown:
            {
                bool stopCooldown=checkCooldown();
                if(stopCooldown) {
                    Info << name() << " cooldown ended. Writing possible" << endl;
                    theState_=stateWaiting;
                    doWrite=checkWrite();
                }
            }
            break;
        case stateStartCooldown:
            FatalErrorIn("conditionDrivenWritingFunctionObject::write")
                << "State 'startCooldown' should not be reached here"
                    << endl
                    << exit(FatalError);
            break;
        default:
            FatalErrorIn("conditionDrivenWritingFunctionObject::write")
                << "Unsupported state"
                    << endl
                    << exit(FatalError);
    }

    if(theState_==stateStartCooldown) {
        theState_=stateCooldown;
        switch(cooldownMode_) {
            case cdmNoCooldown:
                theState_=stateWaiting;
                break;
            case cdmNTimesteps:
                Info << name() << ": Waiting for " << cooldownTimesteps_
                    << " till next output" << endl;
                timestepForStateChange_=time().timeIndex()+cooldownTimesteps_;
                break;
            case cdmIntervall:
                timeForStateChange_=time().value()+cooldownIntervall_;
                Info << name() << ": Waiting till " << timeForStateChange_
                    << " till next output" << endl;
                break;
            case cdmRetrigger:
                Info << name() << ": waiting until triggered for next output"
                    << endl;
                break;
            default:
                FatalErrorIn("conditionDrivenWritingFunctionObject::write")
                    << "Unsupported cooldown mode"
                        << endl
                        << exit(FatalError);
        }
        bool stopCooldown=checkCooldown();
        if(stopCooldown) {
            Info << name() << " cooldown ended. Writing possible" << endl;
            theState_=stateWaiting;
            doWrite=checkWrite();
        }
    }

    if(doWrite) {
        writeNow();
    } else {
        // don't need to store the state if it was already written
        if(storeAndWritePreviousState_) {
            storePreviousState();
        }
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
