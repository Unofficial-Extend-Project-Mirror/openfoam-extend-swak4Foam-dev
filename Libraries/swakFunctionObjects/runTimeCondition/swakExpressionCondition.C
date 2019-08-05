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

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"

#ifdef FOAM_RUNTIME_CONDITION

#include "swakExpressionCondition.H"
#include "addToRunTimeSelectionTable.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
namespace runTimeControls
{
    defineTypeNameAndDebug(swakExpressionCondition, 0);
    addToRunTimeSelectionTable
    (
        runTimeCondition,
        swakExpressionCondition,
        dictionary
    );
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::runTimeControls::swakExpressionCondition::
swakExpressionCondition
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    stateFunctionObject& state
)
:
    runTimeCondition(name, obr, dict, state),
    driver_(
        CommonValueExpressionDriver::New(
            dict,
            refCast<const fvMesh>(obr)
        )
    ),
    triggerExpression_(
        dict.lookup("triggerCondition"),
        dict
    ),
    triggerAccumulation_(
        LogicalAccumulationNamedEnum::names[
            word(dict.lookup("triggerConditionAccumulation"))
        ]
    )
{
}


// * * * * * * * * * * * * * * Public Member Functions * * * * * * * * * * * //

bool Foam::functionObjects::runTimeControls::swakExpressionCondition::apply()
{
    if (!active_)
    {
        return true;
    }

    driver_->clearVariables();
    driver_->parse(triggerExpression_);

    if(
        driver_->CommonValueExpressionDriver::getResultType()
        !=
        pTraits<bool>::typeName
    ) {
        FatalErrorInFunction
            << "Logical Expression " << triggerExpression_
                << " evaluates to type "
                << driver_->CommonValueExpressionDriver::getResultType()
                << " when it should be " << pTraits<bool>::typeName
                << endl
                << exit(FatalError);
    }

    bool result=false;

    switch(triggerAccumulation_) {
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
            FatalErrorInFunction
                << "Unimplemented logical accumulation "
                    << LogicalAccumulationNamedEnum::names[triggerAccumulation_]
                    << endl
                    << exit(FatalError);
    }

    return result;
}


void Foam::functionObjects::runTimeControls::swakExpressionCondition::write()
{
}

#endif

// ************************************************************************* //
