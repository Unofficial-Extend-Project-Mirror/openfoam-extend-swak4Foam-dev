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
    simpleFunctionObject

Description
    Basis for the other function objects here. Evaluates the most common options

SourceFiles
    simpleFunctionObject.C

Contributors/Copyright:
    2008-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef simpleFunctionObject_H
#define simpleFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "OFstream.H"
#include "HashPtrTable.H"
#include "boolList.H"
#include "Field.H"
#include "NamedEnum.H"

#include "swak.H"
#include "switchableNotImplemented.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class simpleFunctionObject Declaration
\*---------------------------------------------------------------------------*/

    class objectRegistry;

class simpleFunctionObject
:
    public functionObject
{
public:
    enum outputControlModeType {
        ocmTimestep,
        ocmDeltaT,
        ocmOutputTime,
        ocmStartup,
        ocmOutputTimeAndStartup,
        ocmTimestepAndStartup,
        ocmDeltaTAndStartup
    };
private:
    // Private Member Functions

    //- Disallow default bitwise copy construct
    simpleFunctionObject(const simpleFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const simpleFunctionObject&);

    // Private data

    //- Do output on the terminal
    bool verbose_;

    //- Write debug-information to the terminal
    bool writeDebug_;

    //- Time after which calculation should start
    scalar after_;

    //- how many time-steps since last output
    label timeSteps_;

    //- possible outputControl-modes
    static const NamedEnum<outputControlModeType,7> outputControlModeTypeNames_;
    outputControlModeType outputControlMode_;

    //- how many time-steps between outputs
    label outputInterval_;

    //- deltaT for output
    scalar outputDeltaT_;

    const Time& time_;

    scalar lastWrite_;

    bool started_;

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    //- make sure that execute is not called twice at one time-step
    label lastTimeStepExecute_;

    //- allow executing more than once - for instance in fvOptions
    bool executeMoreThanOnce_;

    bool ensureExecuteOnce();
#endif

protected:

    outputControlModeType outputControlMode() const
        { return outputControlMode_; }

    dictionary dict_;

    word regionName_;

    string regionString_;

    const char *regionString() const { return regionString_.c_str(); }

    const objectRegistry& obr_;

    //- Do output on the terminal
    bool verbose() const { return verbose_; }

    //- Write debug-information to tthe terminal
    bool writeDebug() const { return writeDebug_; }

    //- Do output this time?
    virtual bool outputTime(const bool forceWrite);

public:

    //- Runtime type information
    TypeName("simpleFunctionObject");


    // Constructors

    //- Construct from components
    simpleFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute(const bool forceWrite);

#ifdef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
    bool execute()
        {
            return execute(false);
        }
#endif

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    //- Interface for new functionObject
    virtual bool execute();
    virtual bool write();
#endif

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

    //- Sample and write
    virtual void writeSimple()=0;

#ifdef FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH
    // make functionObject happy but fail because this is not an implementation
    virtual void updateMesh(const mapPolyMesh&) {
        switchableNotImplemented(
            "simpleFunctionObject::updateMesh",
            dict()
        );
    }

    // make functionObject happy but fail because this is not an implementation
    virtual void movePoints(
#ifdef FOAM_MOVEPOINTS_GETS_POLYMESH
        const polyMesh&
#else
        const pointField&
#endif
    ) {
        switchableNotImplemented(
            "simpleFunctionObject::movePoints",
            dict()
        );
    }
#endif

    //- Optional stuff to do after write
    virtual void flush();

    //- the actual time
    const Time &time() const { return time_; }

    //- the actual time
    scalar after() const { return after_; }

    const objectRegistry &obr() const { return obr_; }

    const dictionary &dict() const { return dict_; }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
