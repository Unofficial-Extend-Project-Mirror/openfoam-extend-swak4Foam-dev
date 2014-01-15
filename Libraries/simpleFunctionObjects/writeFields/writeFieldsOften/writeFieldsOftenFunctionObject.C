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
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "writeFieldsOftenFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

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
template<>
const char* NamedEnum<Foam::Time::writeControls, 5>::names[] =
{
    "timeStep",
    "runTime",
    "adjustableRunTime",
    "clockTime",
    "cpuTime"
};

const NamedEnum<Foam::Time::writeControls, 5> writeControlNames;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeFieldsOftenFunctionObject::writeFieldsOftenFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    writeFieldsGeneralFunctionObject(name,t,dict),
    writeControl_(Time::wcTimeStep),
    writeInterval_(GREAT),
    outputTimeIndex_(0)
{
}

bool writeFieldsOftenFunctionObject::start()
{
    writeFieldsGeneralFunctionObject::start();

    word wcName=dict_.lookup("writeControl");

    writeControl_ = writeControlNames.read
        (
            dict_.lookup("writeControl")
        );

    writeInterval_ = readScalar(dict_.lookup("writeIntervall"));
    if(writeControl_ == Time::wcTimeStep && label(writeInterval_) <1) {
        WarningIn("bool writeFieldsOftenFunctionObject::start()")
            << "writeInterval " << writeInterval_
                << " < 1 for writeControl timeStep. Reseting to 1 "<< endl;
        writeInterval_=1;
    }

    Info << "Additional fields " << fieldNames() << " will be written "
        << "with writeControl " << wcName << " and intervall " << writeInterval_ << endl;

    if(writeControl_ == Time::wcAdjustableRunTime) {
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
        case Time::wcTimeStep:
            writeNow = !(time().timeIndex()%label(writeInterval_));
            break;

        case Time::wcRunTime:
        case Time::wcAdjustableRunTime:
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

        case Time::wcCpuTime:
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

        case Time::wcClockTime:
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
