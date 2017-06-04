/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           |
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
    Foam::StateMachine

Description
    Basic statemachine. Stores the state and is in charge of calculating
    progress

    Also holds the ValueExpressionDriver that does the state calculations

SourceFiles
    StateMachine.C

Contributors/Copyright:
    2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef StateMachine_H
#define StateMachine_H

#include "CommonValueExpressionDriver.H"
#include "speciesTable.H"
#include "PtrList.H"
#include "LogicalAccumulationNamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class StateMachineRepository;

/*---------------------------------------------------------------------------*\
                           Class StateMachine Declaration
\*---------------------------------------------------------------------------*/

class StateMachine
{
    // Private data

    // Constructors

    //- Disallow default bitwise assignment
    void operator=(const StateMachine&);

    //- Construct null
    StateMachine(const IOobject &);

    //- the driver
    autoPtr<CommonValueExpressionDriver> driver_;

    //- the mesh
    const fvMesh &mesh_;

    //- the state names
    const speciesTable names_;

    //- the name of the machine
    word machineName_;

    //- initial state
    label initialState_;

    //- current state
    label state_;

    //- last time the state changed
    scalar lastStateChange_;

    //- Steps since last change
    label stepsSinceChange_;

    //- How often changed the machine to this state
    labelList changedTo_;

    //- Constructor
    StateMachine(
        const dictionary &dict,
        const fvMesh &mesh
    );

    class StateTransition {

        //- state this goes from
        label from_;

        //- state this goes to
        label to_;

        //- The "parent"
        StateMachine &machine_;

        //- description of the transition
        string description_;

        //- The condition expression
        const exprString condition_;

        //- How should the condition be aqccumulated
        LogicalAccumulationNamedEnum::value logicalAccumulation_;

    public:
        StateTransition(
            StateMachine &machine,
            const dictionary &data
        );

        inline label from() {
            return from_;
        }

        inline label to() {
            return to_;
        }

        inline const string &description() {
            return description_;
        }
        bool operator()();
    };

    PtrList<StateTransition> transitions_;

protected:

    void resetState(
        const word &state,
        scalar timeOfChange,
        label stepsSinceChange,
        labelList changedTo
    );

    inline scalar lastStateChange() const {
        return lastStateChange_;
    }

public:
     //- Runtime type information
    TypeName("StateMachine");


    //- Destructor

    virtual ~StateMachine() {}

    //- construct a new machine and register it
    static StateMachine &NewMachine(
        const dictionary &dict,
        const fvMesh &mesh
    );

    static StateMachine &machine(
        const word &name
    );

    //- make sure that the state is consistently written
    static void ensureWrite();

    inline CommonValueExpressionDriver &driver() {
        return driver_();
    }

    inline label numberOfStates() const {
        return names_.size();
    }

    inline const word &name() {
        return machineName_;
    }

    inline label stateCode(word name) const {
        return names_[name];
    }

    inline bool hasState(word name) const {
        return names_.contains(name);
    }

    inline const word &stateName(label stateCode) const {
        return names_[stateCode];
    }

    inline label currentState() const {
        return state_;
    }

    inline label stepsSinceChange() const {
        return stepsSinceChange_;
    }

    inline label changedTo(label state) const {
        return changedTo_[state];
    }

    //- time since last state change
    scalar timeSinceChange() const;

    //- moves the state machine to the next state

    //- return value is a string with the information about what happened
    std::string step();

    //- force the state machine to a given state

    //- return value is a string with the information about what happened
    std::string force(label state);

    friend class StateMachineRepository;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
