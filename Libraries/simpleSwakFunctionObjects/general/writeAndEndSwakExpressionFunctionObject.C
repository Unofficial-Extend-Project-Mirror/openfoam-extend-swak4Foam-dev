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

#include "writeAndEndSwakExpressionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(writeAndEndSwakExpressionFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        writeAndEndSwakExpressionFunctionObject,
        dictionary
    );

template<>
const char* NamedEnum<Foam::writeAndEndSwakExpressionFunctionObject::logicalAccumulations,2>::names[]=
{
    "and",
    "or"
};
const NamedEnum<writeAndEndSwakExpressionFunctionObject::logicalAccumulations,2> writeAndEndSwakExpressionFunctionObject::logicalAccumulationsNames_;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeAndEndSwakExpressionFunctionObject::writeAndEndSwakExpressionFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    writeAndEndFunctionObject(name,t,dict)
{
    readParameters(dict);
}

bool writeAndEndSwakExpressionFunctionObject::read(const dictionary& dict)
{
    readParameters(dict);
    return writeAndEndFunctionObject::read(dict);
}

void writeAndEndSwakExpressionFunctionObject::readParameters(const dictionary &dict)
{
    driver_=CommonValueExpressionDriver::New(
        dict,
        refCast<const fvMesh>(obr_)
    );

    logicalExpression_=dict.lookup("logicalExpression");

    logicalAccumulation_=logicalAccumulationsNames_[dict.lookup("logicalAccumulation")];
}

bool writeAndEndSwakExpressionFunctionObject::endRunNow()
{
    driver_->clearVariables();
    driver_->parse(logicalExpression_);

    if(driver_->CommonValueExpressionDriver::getResultType()!=pTraits<bool>::typeName) {
        FatalErrorIn("writeAndEndSwakExpressionFunctionObject::endRunNow()")
            << "Logical Expression " << logicalExpression_
                << " evaluates to type " << driver_->CommonValueExpressionDriver::getResultType()
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

    if(result) {
        Info << "Stopping because expression " << logicalExpression_
            << " evaluated to 'true' in " << name() << endl;
    }

    return result;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
