//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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

\*---------------------------------------------------------------------------*/

#include "executeIfSwakExpressionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"
#include "argList.H"

#include "objectRegistry.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(executeIfSwakExpressionFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfSwakExpressionFunctionObject,
        dictionary
    );

template<>
const char* NamedEnum<Foam::executeIfSwakExpressionFunctionObject::logicalAccumulations,2>::names[]=
{
    "and",
    "or"
};
const NamedEnum<executeIfSwakExpressionFunctionObject::logicalAccumulations,2> executeIfSwakExpressionFunctionObject::logicalAccumulationsNames_;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfSwakExpressionFunctionObject::executeIfSwakExpressionFunctionObject
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
    )
{
    readParameters(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool executeIfSwakExpressionFunctionObject::condition()
{
    driver_->clearVariables();
    driver_->parse(logicalExpression_);

    if(driver_->getResultType()!=pTraits<bool>::typeName) {
        FatalErrorIn("executeIfSwakExpressionFunctionObject::condition()")
            << "Logical Expression " << logicalExpression_ 
                << " evaluates to type " << driver_->getResultType() 
                << " when it should be " << pTraits<bool>::typeName
                << endl
                << exit(FatalError);
    }

    bool result=false;

    switch(logicalAccumulation_) {
        case logAnd:
            result=driver_->getReduced(andOp<bool>(),true);
            break;
        case logOr:
            result=driver_->getReduced(orOp<bool>(),false);
            break;
        default:
            FatalErrorIn("executeIfSwakExpressionFunctionObject::condition()")
                << "Unimplemented logical accumulation "
                    << logicalAccumulationsNames_[logicalAccumulation_]
                    << endl
                    << exit(FatalError);
    }
    if(writeDebug()) {
        Info << "Expression " << logicalExpression_ 
            << " evaluates to " << driver_->getResult<bool>() << endl;
        Info << " -> " << logicalAccumulationsNames_[logicalAccumulation_]
            << " gives " << result << endl;
    }

    return result;
}

bool executeIfSwakExpressionFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

void executeIfSwakExpressionFunctionObject::readParameters(const dictionary &dict) 
{
    driver_=CommonValueExpressionDriver::New(
        dict,
        refCast<const fvMesh>(obr_)
    ); 
    
    logicalExpression_=dict.lookup("logicalExpression");

    logicalAccumulation_=logicalAccumulationsNames_[dict.lookup("logicalAccumulation")];
}

} // namespace Foam

// ************************************************************************* //
