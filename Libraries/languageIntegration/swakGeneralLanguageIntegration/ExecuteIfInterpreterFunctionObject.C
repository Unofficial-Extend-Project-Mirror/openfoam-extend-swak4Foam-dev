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
    2011-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "ExecuteIfInterpreterFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "objectRegistry.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Wrapper>
ExecuteIfInterpreterFunctionObject<Wrapper>::ExecuteIfInterpreterFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    conditionalFunctionObjectListProxy(
        name,
        t,
        dict
    ),
    Wrapper(
        t.db(),
        dict
    )
{
    if(!this->parallelNoRun()) {
        this->initEnvironment(t);

        this->setRunTime(t);
    }

    readParameters(dict);

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    start();
#endif
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Wrapper>
bool ExecuteIfInterpreterFunctionObject<Wrapper>::condition()
{
    if(!this->parallelNoRun()) {
        this->setRunTime(time());
    }

    this->dictionariesToInterpreterStructs();

    if(writeDebug()) {
        Info << "Evaluating " << conditionCode_ << endl;
    }
    bool result=this->evaluateCodeTrueOrFalse(conditionCode_,true);
    if(writeDebug()) {
        Info << "Evaluated to " << result << endl;
    }
    return result;
}

template<class Wrapper>
bool ExecuteIfInterpreterFunctionObject<Wrapper>::read(const dictionary& dict)
{
    readParameters(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

template<class Wrapper>
void ExecuteIfInterpreterFunctionObject<Wrapper>::readParameters(const dictionary &dict)
{
    this->readCode(dict,"init",initCode_);
    this->readCode(dict,"condition",conditionCode_);

    this->dictionariesToInterpreterStructs();

    Wrapper::executeCode(
        initCode_,
        false
    );

    this->interpreterStructsToDictionaries();
}

} // namespace Foam

// ************************************************************************* //
