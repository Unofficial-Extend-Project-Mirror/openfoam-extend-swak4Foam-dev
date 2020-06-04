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

Class
    conditionDrivenWritingFunctionObject

Description
    General class for functionObjects that write timesteps if certain conditions
    are met

SourceFiles
    conditionDrivenWritingFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: conditionDrivenWritingFunctionObject.H,v 78b0d113b99b 2013-02-25 16:12:41Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef conditionDrivenWritingFunctionObject_H
#define conditionDrivenWritingFunctionObject_H

#include "simpleFunctionObject.H"

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
#include "Enum.H"
#else
#include "NamedEnum.H"
#endif

#include "TimeCloneList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class conditionDrivenWritingFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class conditionDrivenWritingFunctionObject
:
    public simpleFunctionObject
{
public:
    enum writeControlModeType {
        scmWriteAlways,
        scmWriteNTimesteps,
        scmWriteIntervall,
        scmWriteUntilSwitch,
    };

    enum cooldownModeType {
        cdmNoCooldown,
        cdmNTimesteps,
        cdmIntervall,
        cdmRetrigger,
    };

private:
    // Private Member Functions

    //- Disallow default bitwise copy construct
    conditionDrivenWritingFunctionObject(const conditionDrivenWritingFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const conditionDrivenWritingFunctionObject&);

    enum theStateType {
        stateWaiting,
        stateWriting,
        stateStartCooldown,
        stateCooldown,
    };

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    static const Enum<writeControlModeType> writeControlModeTypeNames_;
#else
    static const NamedEnum<writeControlModeType,4> writeControlModeTypeNames_;
#endif
    writeControlModeType writeControlMode_;

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    static const Enum<cooldownModeType> cooldownModeTypeNames_;
#else
    static const NamedEnum<cooldownModeType,4> cooldownModeTypeNames_;
#endif
    cooldownModeType cooldownMode_;

    bool storeAndWritePreviousState_;

    theStateType theState_;

    label writeTimesteps_;
    scalar writeIntervall_;
    label cooldownTimesteps_;
    scalar cooldownIntervall_;

    label timestepForStateChange_;
    scalar timeForStateChange_;

    void storePreviousState();

    void writePreviousState();

    void writeNow();

    bool alreadyWritten(word timename);

    bool checkWrite();

    bool checkCooldown();

    autoPtr<TimeCloneList> lastTimes_;

protected:
    //- should writing start now?
    virtual bool checkStartWriting() =0;

    //- switch off writing? Does not have to be implemented
    virtual bool checkStopWriting();

    //- switch off cooldown? Does not have to be implemented
    virtual bool checkStopCooldown();

    cooldownModeType cooldownMode() { return cooldownMode_; }

    writeControlModeType writeControlMode() { return writeControlMode_; }

public:

    //- Runtime type information
    TypeName("conditionDrivenWriting");


    // Constructors

    //- Construct from components
    conditionDrivenWritingFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
