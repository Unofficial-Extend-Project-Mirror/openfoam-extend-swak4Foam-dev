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
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "StateMachineRepository.H"

#include "objectRegistry.H"
#include "swakTime.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(StateMachineRepository, 0);

StateMachineRepository *StateMachineRepository::repositoryInstance(NULL);

StateMachineRepository::StateMachineRepository(
    const objectRegistry &obr
)
    :
    regIOobject(
        IOobject(
            "state_machines",
            obr.time().timeName(),
            "swak4Foam",
            obr.time(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        )
    )
{
    Pbug << "StateMachineRepository at "
        << objectPath() << " created" << endl;

    if(headerOk()) {
        Pbug << "Found a file " <<  objectPath() << endl;

        readData(readStream("StateMachineRepository"));
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

StateMachineRepository::~StateMachineRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

StateMachineRepository &StateMachineRepository::getStateMachines(
    const objectRegistry &obr
)
{
    StateMachineRepository*  ptr=repositoryInstance;

    if(debug) {
        Info << "StateMachineRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Pout << "swak4Foam: Allocating new repository for StateMachine\n";

        ptr=new StateMachineRepository(obr);
    }

    repositoryInstance=ptr;

    return *repositoryInstance;
}

StateMachineRepository &StateMachineRepository::getStateMachines()
{
    if(debug) {
        Info << "StateMachineRepository: asking for Singleton" << endl;
    }

    if(repositoryInstance==NULL) {
        FatalErrorIn("&StateMachineRepository::getStateMachines()")
            << "No repository"
                << endl
                << exit(FatalError);
    }

    return *repositoryInstance;
}

void StateMachineRepository::ensureWrite()
{
    if(time().outputTime()) {
        writeObject(
            time().writeFormat(),
            IOstream::currentVersion,
            time().writeCompression()
        );
    }
}

bool StateMachineRepository::writeData(Ostream &os) const
{
    Pbug << "StateMachineRepository at " << objectPath()
            << " writing" << endl;

    // enforce ASCII because of a problem with HashTable written in BINARY
    IOstream::streamFormat old=os.format(IOstream::ASCII);

    dictionary states;

    forAllConstIter(MachineTable,allMachines_,iter)
    {
	const StateMachine &m=**iter;
        const word &name=iter.key();

        dictionary state;
        state.set("state",m.stateName(m.currentState()));
        state.set("changeTime",m.lastStateChange());
        state.set("stepsSinceChange",m.stepsSinceChange());
        for(label stateNr=0;stateNr<m.numberOfStates();stateNr++) {
            if(m.changedTo(stateNr)>0) {
                state.set(
                    word("changedTo_"+m.stateName(stateNr)),
                    m.changedTo(stateNr)
                );
            }
        }
        states.set(name,state);
    }

    os << states;

    os.format(old);

    return os.good();
}

bool StateMachineRepository::readData(Istream &is)
{
    Pbug << "StateMachineRepository at " << objectPath()
        << " reading" << endl;

    // enforce ASCII because of a problem with HashTable written in BINARY
    IOstream::streamFormat old=is.format(IOstream::ASCII);

    is >> readFromRestart_;

    is.format(old);

    Pbug << "StateMachineRepository reading finished" << endl;

    if(readFromRestart_.size()>0) {
        Info << "Read restart information for state machines "
            << readFromRestart_.toc() << endl;
    }
    return !is.bad();
}

bool StateMachineRepository::found(const word &name)
{
    return
        allMachines_.found(name)
        &&
        !readFromRestart_.found(name);
}

void StateMachineRepository::insert(
    autoPtr<StateMachine> machine
) {
    const word name=machine->name();

    if(allMachines_.found(name)) {
        FatalErrorIn("StateMachineRepository::insert")
            << "Already got a StateMachine " << name
                << endl
                << exit(FatalError);
    } else {
        allMachines_.insert(
            name,
            machine.ptr()
        );
    }
    if(readFromRestart_.found(name)) {
        // Was read during restart. Use the read machine
        Info << "Reseting state machine " << name << flush;
        const dictionary &data=readFromRestart_.subDict(name);
        StateMachine &m=(*this)[name];
        labelList changedTo(
            m.numberOfStates(),
            0
        );
        forAll(changedTo,stateI) {
            const word &name="changedTo_"+m.stateName(stateI);
            changedTo[stateI]=data.lookupOrDefault<label>(name,0);
        }
        m.resetState(
            word(data["state"]),
            readScalar(data["changeTime"]),
            readLabel(data["stepsSinceChange"]),
            changedTo
        );
        Info << " to state " << m.stateName(m.currentState())
            << " (changed at " << m.lastStateChange() << ")" << endl;
        readFromRestart_.remove(name);
    }
}

StateMachine &StateMachineRepository::operator[](const word &name)
{
    Pbug << "operator[" << name << "] Found: " << found(name) << "/"
        << allMachines_.found(name) << endl;

    return *allMachines_[name];
}

wordList StateMachineRepository::toc()
{
    return allMachines_.sortedToc();
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
