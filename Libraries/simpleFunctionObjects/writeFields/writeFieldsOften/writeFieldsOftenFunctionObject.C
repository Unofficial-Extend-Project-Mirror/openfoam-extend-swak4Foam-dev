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
    2008-2011, 2013, 2015-2016, 2018-2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id: writeFieldsOftenFunctionObject.C,v 45163b34f052 2019-07-09 10:42:58Z bgschaid $
\*---------------------------------------------------------------------------*/

#include "writeFieldsOftenFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(writeFieldsOftenFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        writeFieldsOftenFunctionObject,
        dictionary
    );


    // copied from Time.C because the original is protected
    // to work the order of values in writeControls must not change
#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
const Enum<TimeWriteControl> writeControlNames
({
    {TimeWriteControl::wcTimeStep, "timeStep"},
    {TimeWriteControl::wcRunTime, "runTime"},
    {TimeWriteControl::wcAdjustableRunTime, "adjustableRunTime"},
    {TimeWriteControl::wcClockTime, "clockTime"},
    {TimeWriteControl::wcCpuTime, "cpuTime"},
});
#else
template<>
const char* NamedEnum<TimeWriteControl, 5>::names[] =
{
    "timeStep",
    "runTime",
    "adjustableRunTime",
    "clockTime",
    "cpuTime"
};

const NamedEnum<TimeWriteControl, 5> writeControlNames;
#endif

#ifdef FOAM_WRITECONTROL_IN_TIME_CHANGED
        #define wcTimeStep Time::writeControl::timeStep
        #define wcRunTime Time::writeControl::runTime
        #define wcAdjustableRunTime Time::writeControl::adjustableRunTime
        #define wcClockTime Time::writeControl::clockTime
        #define wcCpuTime Time::writeControl::cpuTime
#else
        #define wcTimeStep Time::writeControls::wcTimeStep
        #define wcRunTime Time::writeControls::wcRunTime
        #define wcAdjustableRunTime Time::writeControls::wcAdjustableRunTime
        #define wcClockTime Time::writeControls::wcClockTime
        #define wcCpuTime Time::writeControls::wcCpuTime
#endif

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeFieldsOftenFunctionObject::writeFieldsOftenFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    writeFieldsGeneralFunctionObject(name,t,dict),
    writeControl_(wcTimeStep),
    writeInterval_(GREAT),
    outputTimeIndex_(0)
{
}

bool writeFieldsOftenFunctionObject::start()
{
    writeFieldsGeneralFunctionObject::start();

    const word wcName(dict_.lookup("writeControl"));

    writeControl_ = writeControlNames[wcName];

    writeInterval_ = readScalar(dict_.lookup("writeIntervall"));
    if(writeControl_ == wcTimeStep && label(writeInterval_) <1) {
        WarningIn("bool writeFieldsOftenFunctionObject::start()")
            << "writeInterval " << writeInterval_
                << " < 1 for writeControl timeStep. Reseting to 1 "<< endl;
        writeInterval_=1;
    }

    Info << "Additional fields " << fieldNames() << " will be written "
        << "with writeControl " << wcName << " and interval " << writeInterval_ << endl;

    if(writeControl_ == wcAdjustableRunTime) {
        WarningIn("bool writeFieldsOftenFunctionObject::start()")
            << "Cant adjust the run-time. Defaulting to runTime" << endl;

    }

    outputTimeIndex_=0;

    return true;
}

bool writeFieldsOftenFunctionObject::outputTime(const bool forceWrite)
{
    if(forceWrite) {
        return true;
    }

    if(time().time().value()<after()) {
        return false;
    }

    bool writeNow=false;

    // lifted from Time::operator++
    switch(writeControl_){
        case wcTimeStep:
            writeNow = !(time().timeIndex()%label(writeInterval_));
            break;

        case wcRunTime:
        case wcAdjustableRunTime:
            {
                label outputTimeIndex =
                    label(((time().time().value() - time().startTime().value()) + 0.5*time().deltaT().value())/writeInterval_);

                if (outputTimeIndex > outputTimeIndex_)
                {
                    writeNow = true;
                    outputTimeIndex_ = outputTimeIndex;
                }
                else
                {
                    writeNow = false;
                }
            }
        break;

        case wcCpuTime:
            {
                label outputTimeIndex =
                    label(time().elapsedCpuTime()/writeInterval_);

                if (outputTimeIndex > outputTimeIndex_)
                {
                    writeNow = true;
                    outputTimeIndex_ = outputTimeIndex;
                }
                else
                {
                    writeNow = false;
                }
            }
        break;

        case wcClockTime:
            {
                label outputTimeIndex = label(time().elapsedClockTime()/writeInterval_);
                if (outputTimeIndex > outputTimeIndex_)
                {
                    writeNow = true;
                    outputTimeIndex_ = outputTimeIndex;
                }
                else
                {
                    writeNow = false;
                }
            }
        break;
        };

    return writeNow;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
