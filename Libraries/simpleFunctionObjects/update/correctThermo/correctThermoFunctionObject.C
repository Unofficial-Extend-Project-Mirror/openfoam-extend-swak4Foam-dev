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
    2008-2011, 2013, 2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "correctThermoFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "basicThermo.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(correctThermoFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        correctThermoFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

correctThermoFunctionObject::correctThermoFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    updateSimpleFunctionObject(name,t,dict),
    rhoName_("none")
{
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    start();
#endif
}

bool correctThermoFunctionObject::start()
{
    updateRho_=readBool(dict_.lookup("updateRho"));
    if(updateRho_) {
        rhoName_=word(dict_.lookup("rhoName"));
    }

    return updateSimpleFunctionObject::start();
}

void correctThermoFunctionObject::recalc()
{
    basicThermo &thermo=const_cast<basicThermo&>(
        obr_.lookupObject<basicThermo>("thermophysicalProperties")
    );
    Info << "Correcting thermo" << endl;

    thermo.correct();

    if(updateRho_) {
        volScalarField &rho=const_cast<volScalarField&>(
            obr_.lookupObject<volScalarField>(rhoName_)
        );
        rho=thermo.rho();
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
