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
    2011-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "WriteIfInterpreterFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    // defineTypeNameAndDebug(WriteIfInterpreterFunctionObject, 0);

    // addToRunTimeSelectionTable
    // (
    //     functionObject,
    //     WriteIfInterpreterFunctionObject,
    //     dictionary
    // );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Wrapper>
WriteIfInterpreterFunctionObject<Wrapper>::WriteIfInterpreterFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    conditionDrivenWritingFunctionObject(name,t,dict),
    Wrapper(
        t.db(),
        dict
    )
{
    if(!this->parallelNoRun()) {
        this->initEnvironment(t);

        this->setRunTime(t);
    }

    this->readParameters(dict);
}

template<class Wrapper>
bool WriteIfInterpreterFunctionObject<Wrapper>::read(const dictionary& dict)
{
    this->readParameters(dict);
    return conditionDrivenWritingFunctionObject::read(dict);
}

template<class Wrapper>
void WriteIfInterpreterFunctionObject<Wrapper>::readParameters(const dictionary &dict)
{
    this->readCode(dict,"initVariables",initCode_);
    this->readCode(dict,"startWrite",writeCode_);
    if(this->writeControlMode()==scmWriteUntilSwitch) {
        this->readCode(dict,"stopWrite",stopWriteCode_);
    }
    if(this->cooldownMode()==cdmRetrigger) {
        this->readCode(dict,"stopCooldown",stopCooldownCode_);
    }

    Wrapper::executeCode(
        initCode_,
        false
    );
}

template<class Wrapper>
bool WriteIfInterpreterFunctionObject<Wrapper>::executeCode(const string code)
{
    if(!this->parallelNoRun()) {
        this->setRunTime(this->time());
    }

    if(this->writeDebug()) {
        Info << "Evaluating " << code << endl;
    }
    bool result=this->evaluateCodeTrueOrFalse(code,true);
    if(writeDebug()) {
        Info << "Evaluated to " << result << endl;
    }

    return result;
}

template<class Wrapper>
bool WriteIfInterpreterFunctionObject<Wrapper>::checkStartWriting()
{
    return this->executeCode(writeCode_);
}

template<class Wrapper>
bool WriteIfInterpreterFunctionObject<Wrapper>::checkStopWriting()
{
    return this->executeCode(stopWriteCode_);
}

template<class Wrapper>
bool WriteIfInterpreterFunctionObject<Wrapper>::checkStopCooldown()
{
    return this->executeCode(stopCooldownCode_);
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //