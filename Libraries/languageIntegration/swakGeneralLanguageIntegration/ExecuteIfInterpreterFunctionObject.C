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
    2011-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
    // defineTypeNameAndDebug(ExecuteIfInterpreterFunctionObject, 0);

    // addToRunTimeSelectionTable
    // (
    //     functionObject,
    //     ExecuteIfInterpreterFunctionObject,
    //     dictionary
    // );

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

    Wrapper::executeCode(
        initCode_,
        false
    );
}

} // namespace Foam

// ************************************************************************* //
