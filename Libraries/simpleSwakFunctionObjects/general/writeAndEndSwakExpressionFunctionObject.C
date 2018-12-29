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
    2012-2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "writeAndEndSwakExpressionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

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

void writeAndEndSwakExpressionFunctionObject::readParameters(
    const dictionary &dict
)
{
    driver_=CommonValueExpressionDriver::New(
        dict,
        refCast<const fvMesh>(obr_)
    );

    logicalExpression_=exprString(
        dict.lookup("logicalExpression"),
        dict
    );

    logicalAccumulation_=LogicalAccumulationNamedEnum::names[
        word(dict.lookup("logicalAccumulation"))
    ];
}

bool writeAndEndSwakExpressionFunctionObject::endRunNow()
{
    driver_->clearVariables();
    driver_->parse(logicalExpression_);

    if(
        driver_->CommonValueExpressionDriver::getResultType()
        !=
        pTraits<bool>::typeName
    ) {
        FatalErrorIn("writeAndEndSwakExpressionFunctionObject::endRunNow()")
            << "Logical Expression " << logicalExpression_
                << " evaluates to type "
                << driver_->CommonValueExpressionDriver::getResultType()
                << " when it should be " << pTraits<bool>::typeName
                << endl
                << exit(FatalError);
    }

    bool result=false;

    switch(logicalAccumulation_) {
        case LogicalAccumulationNamedEnum::logAnd:
        case LogicalAccumulationNamedEnum::logAll:
            result=driver_->getReduced(andOp<bool>(),true);
            break;
        case LogicalAccumulationNamedEnum::logOr:
        case LogicalAccumulationNamedEnum::logAny:
            result=driver_->getReduced(orOp<bool>(),false);
            break;
        case LogicalAccumulationNamedEnum::logNone:
            result=!driver_->getReduced(orOp<bool>(),false);
            break;
        default:
            FatalErrorIn("executeIfSwakExpressionFunctionObject::condition()")
                << "Unimplemented logical accumulation "
                    << LogicalAccumulationNamedEnum::names[logicalAccumulation_]
                    << endl
                    << exit(FatalError);
    }
    if(writeDebug()) {
        Info << "Expression " << logicalExpression_
            << " evaluates to " << driver_->getResult<bool>() << endl;
        Info << " -> "
            << LogicalAccumulationNamedEnum::names[logicalAccumulation_]
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
