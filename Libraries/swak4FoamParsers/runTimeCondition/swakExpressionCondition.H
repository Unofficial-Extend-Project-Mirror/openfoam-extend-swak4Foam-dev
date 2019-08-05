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

Class
    Foam::functionObjects::runTimeControls::swakExpressionCondition

Description
    Activated if a boolean swak-expression is true

SourceFiles
    swakExpressionCondition.H
    swakExpressionCondition.C

Contributors/Copyright:

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef functionObjects_runTimeConditions_swakExpressionCondition_H
#define functionObjects_runTimeConditions_swakExpressionCondition_H

#include "runTimeCondition.H"

#include "swak.H"

#include "CommonValueExpressionDriver.H"

#include "LogicalAccumulationNamedEnum.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
namespace runTimeControls
{

/*---------------------------------------------------------------------------*\
                    Class swakExpressionCondition Declaration
\*---------------------------------------------------------------------------*/

class swakExpressionCondition
:
    public runTimeCondition
{
protected:

    // Protected data

    //- the expression driver
    autoPtr<CommonValueExpressionDriver> driver_;

    //- the logical expression
    exprString triggerExpression_;

    //- possible logical accumulations
    LogicalAccumulationNamedEnum::value triggerAccumulation_;

public:

    //- Runtime type information
    TypeName("swakExpression");

    //- Constructor
    swakExpressionCondition
    (
        const word& name,
        const objectRegistry& obr,
        const dictionary& dict,
        stateFunctionObject& state
    );

    //- Destructor
    virtual ~swakExpressionCondition() = default;


    // Public Member Functions

        //- Apply the condition
        virtual bool apply();

        //- Write
        virtual void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace runTimeControls
} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
