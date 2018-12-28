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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "loadCompressibleTurbulenceModelFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "fvMesh.H"
#include "volFields.H"
#include "surfaceFields.H"

#include "swakThermoTypes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(loadCompressibleTurbulenceModelFunctionObject, 0);

    addNamedToRunTimeSelectionTable
    (
        functionObject,
        loadCompressibleTurbulenceModelFunctionObject,
        dictionary,
        loadCompressibleTurbulenceModel
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

loadCompressibleTurbulenceModelFunctionObject::loadCompressibleTurbulenceModelFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    modelLoadingFunctionObject<compressible::turbulenceModel>(name,t,dict)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

    autoPtr<compressible::turbulenceModel> loadCompressibleTurbulenceModelFunctionObject::initModel()
{
    return compressible::turbulenceModel::New(
        obr().lookupObject<volScalarField>(
            word(dict_.lookup("rhoName"))
        ),
        obr().lookupObject<volVectorField>(
            word(dict_.lookup("UName"))
        ),
        obr().lookupObject<surfaceScalarField>(
            word(dict_.lookup("phiName"))
        ),
        obr().lookupObject<swakFluidThermoType>(
            word(dict_.lookup("thermoName"))
        )
    );
}


} // namespace Foam

// ************************************************************************* //
