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

Class
    conditionDrivenWritingFunctionObject

Description
    General class for functionObjects that write timesteps if certain conditions
    are met

SourceFiles
    conditionDrivenWritingFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id: conditionDrivenWritingFunctionObject.H,v 78b0d113b99b 2013-02-25 16:12:41Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef conditionDrivenWritingFunctionObject_H
#define conditionDrivenWritingFunctionObject_H

#include "simpleFunctionObject.H"

#include "NamedEnum.H"

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
        scmWriteUntilSwitch
    };

    enum cooldownModeType {
        cdmNoCooldown,
        cdmNTimesteps,
        cdmIntervall,
        cdmRetrigger
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
        stateCooldown
    };

    static const NamedEnum<writeControlModeType,4> writeControlModeTypeNames_;
    writeControlModeType writeControlMode_;

    static const NamedEnum<cooldownModeType,4> cooldownModeTypeNames_;
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

    void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
