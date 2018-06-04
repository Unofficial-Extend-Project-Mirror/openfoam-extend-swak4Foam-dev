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
    2008-2011, 2013-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SetEndTimeWithInterpreterFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "DebugOStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Wrapper>
SetEndTimeWithInterpreterFunctionObject<Wrapper>::SetEndTimeWithInterpreterFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    TimeManipulationWithInterpreterFunctionObject<Wrapper>(name,t,dict)
{
    this->readParameters(dict);
}

template<class Wrapper>
bool SetEndTimeWithInterpreterFunctionObject<Wrapper>::read(const dictionary& dict)
{
    this->readParameters(dict);
    return TimeManipulationWithInterpreterFunctionObject<Wrapper>::read(dict);
}

template<class Wrapper>
void SetEndTimeWithInterpreterFunctionObject<Wrapper>::readParameters(const dictionary &dict)
{
    this->readCode(dict,"init",initCode_);
    this->readCode(dict,"endTime",endTimeCode_);

    this->dictionariesToInterpreterStructs();

    Wrapper::executeCode(
        initCode_,
        false
    );

    this->interpreterStructsToDictionaries();
}

template<class Wrapper>
scalar SetEndTimeWithInterpreterFunctionObject<Wrapper>::endTime()
{
    if(!this->parallelNoRun()) {
        this->setRunTime(this->time());
    }

    this->dictionariesToInterpreterStructs();

    if(this->writeDebug()) {
        Pbug << "Evaluating " << endTimeCode_ << endl;
    }
    scalar result=this->evaluateCodeScalar(endTimeCode_,true);
    if(this->writeDebug()) {
        Pbug << "Evaluated to " << result << endl;
    }

    if(result!=this->time().endTime().value()) {
        Info << "Changing end time because " << endTimeCode_
            << " evaluated to " << result << "(current endTime: "
            << this->time().endTime().value() << ") in "
            << this->name() << endl;
    }

    return result;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
