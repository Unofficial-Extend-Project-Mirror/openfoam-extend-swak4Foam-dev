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

Contributors/Copyright:
    2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "StateMachine.H"

#include "StateMachineRepository.H"

namespace Foam {

    defineTypeNameAndDebug(StateMachine, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

StateMachine &StateMachine::NewMachine(
    const dictionary &dict,
    const fvMesh &mesh
) {
    autoPtr<StateMachine> m(
        new StateMachine(
            dict,
            mesh
        )
    );
    const word name=m->name();

    StateMachineRepository &repo=
        StateMachineRepository::getStateMachines(mesh);

    if(
        repo.found(
            m->name()
        )
    ) {
        FatalErrorIn("StateMachine::NewMachine")
            << "In " << dict.name() << nl
            << "A machine " << name << " already exists"
                << endl
                << exit(FatalError);
    } else {
        repo.insert(
            m
        );
    }
    return machine(name);
}

void StateMachine::ensureWrite()
{
    StateMachineRepository::getStateMachines().ensureWrite();
}

StateMachine &StateMachine::machine(
    const word &name
) {
    StateMachineRepository &repo=
        StateMachineRepository::getStateMachines();

    if(
        !repo.found(name)
    ) {
        FatalErrorIn("StateMachine::machine")
            << "No machine " << name << " found. " << nl
                << "Available: " << repo.toc()
                << endl
                << exit(FatalError);
    }
    return repo[name];
}

StateMachine::StateMachine(
    const dictionary &dict,
    const fvMesh &mesh
):
    driver_(
        CommonValueExpressionDriver::New(
            dict,
            mesh
        )
    ),
    mesh_(mesh),
    names_(
        dict.lookup("states")
    ),
    machineName_(
        dict.lookup("machineName")
    ),
    initialState_(
        stateCode(
            dict.lookup("initialState")
        )
    ),
    state_(initialState_),
    lastStateChange_(
        mesh.time().value()
    ),
    stepsSinceChange_(0),
    changedTo_(
        names_.size(),
        0
    )
{
    driver_->createWriterAndRead(
        "StateMachine_"+machineName_+"_"+dict.name().name()
    );

    List<dictionary> data(
        dict.lookup("transitions")
    );
    transitions_.resize(data.size());
    forAll(data,i) {
        transitions_.set(
            i,
            new StateTransition(
                *this,
                dictionary(
                    dict,
                    data[i]
                )
            )
        );
    }
}

StateMachine::StateTransition::StateTransition(
     StateMachine &mach,
    const dictionary &data
):
    machine_(mach),
    description_(
        data.lookup("description")
    ),
    condition_(
        data.lookup("condition"),
        data
    ),
    logicalAccumulation_(
        LogicalAccumulationNamedEnum::names[
            data.lookup("logicalAccumulation")
        ]
    )
{
    word fromName(data.lookup("from"));
    word toName(data.lookup("to"));
    if(!machine_.hasState((fromName))) {
        FatalErrorIn("StateMachine::StateTransition::StateTransition")
            << "Problem in " << data.name() << ": State "
                << fromName << " not a valid state"
                << endl
                << exit(FatalError);
    } else {
        from_=machine_.stateCode(fromName);
    }
    if(!machine_.hasState((toName))) {
        FatalErrorIn("StateMachine::StateTransition::StateTransition")
            << "Problem in " << data.name() << ": State "
                << toName << " not a valid state"
                << endl
                << exit(FatalError);
    } else {
        to_=machine_.stateCode(toName);
    }
}

bool StateMachine::StateTransition::operator()()
{
    CommonValueExpressionDriver &driver=machine_.driver();

    driver.parse(condition_);

    if(
        driver.CommonValueExpressionDriver::getResultType()
        !=
        pTraits<bool>::typeName
    ) {
        FatalErrorIn("StateMachine::StateTransition::operator()()")
            << "Logical Expression " << condition_
                << " evaluates to type "
                << driver.CommonValueExpressionDriver::getResultType()
                << " when it should be " << pTraits<bool>::typeName
                << endl
                << exit(FatalError);
    }

    bool result=false;

    switch(logicalAccumulation_) {
        case LogicalAccumulationNamedEnum::logAnd:
            result=driver.getReduced(andOp<bool>(),true);
            break;
        case LogicalAccumulationNamedEnum::logOr:
            result=driver.getReduced(orOp<bool>(),false);
            break;
        default:
            FatalErrorIn("StateMachine::StateTransition::operator()()")
                << "Unimplemented logical accumulation "
                    << LogicalAccumulationNamedEnum::names[logicalAccumulation_]
                    << endl
                    << exit(FatalError);
    }

    return result;
}

std::string StateMachine::step() {
    OStringStream out;
    out << machineName_ << ": ";
    label oldState=state_;
    string reason="";

    driver().clearVariables();

    forAll(transitions_,i) {
        StateTransition &trans=transitions_[i];
        if(
            trans.from()==state_
        ) {
            if(trans()) {
                reason=trans.description();
                state_=trans.to();
                break;
            }
        }
    }

    if(
        state_
        ==
        oldState
    ) {
        stepsSinceChange_++;
        out << "Stayed in state " << stateName(state_)
            << " (" << stepsSinceChange_ << " steps)";
    } else {
        out << "Changed state from " << stateName(oldState)
            << " to " << stateName(state_)
            << " (rule: " << reason.c_str() << ")"
            << " after "
            << timeSinceChange() << "s (" << stepsSinceChange_ << " steps)";
        lastStateChange_=mesh_.time().value();
        stepsSinceChange_=0;
        changedTo_[state_]++;
    }

    return out.str();
}

std::string StateMachine::force(label newState) {
    OStringStream out;
    out << machineName_ << ": ";
    if(
        newState<0
        ||
        newState>=names_.size()
    ) {
        FatalErrorIn("StateMachine::force")
            << "Can not set machine " << name() << "to state " << newState << nl
                << "Valid states are 0 to " << names_.size()-1
                << endl
                << exit(FatalError);
    }
    if(
        state_
        ==
        newState
    ) {
        out << "Already in state " << stateName(state_);
    } else {
            out << "Forced state from " << stateName(state_)
                << " to " << stateName(newState)
                << " after "
                << timeSinceChange() << "s (" << stepsSinceChange_ << " steps)";
            state_=newState;
            lastStateChange_=mesh_.time().value();
            stepsSinceChange_=0;
            changedTo_[state_]++;
    }
    return out.str();
}

scalar StateMachine::timeSinceChange() const
{
    return mesh_.time().value()-lastStateChange_;
}

void StateMachine::resetState(
    const word &state,
    scalar timeOfChange,
    label stepsSinceChange,
    labelList changedTo
) {
    assert(changedTo.size()==names_.size());
    state_=stateCode(state);
    lastStateChange_=timeOfChange;
    stepsSinceChange_=stepsSinceChange;
    changedTo_=changedTo;
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
