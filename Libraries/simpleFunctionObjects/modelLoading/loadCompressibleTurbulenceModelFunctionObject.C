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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

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
            dict_.lookup("rhoName")
        ),
        obr().lookupObject<volVectorField>(
            dict_.lookup("UName")
        ),
        obr().lookupObject<surfaceScalarField>(
            dict_.lookup("phiName")
        ),
        obr().lookupObject<swakFluidThermoType>(
            dict_.lookup("thermoName")
        )
    );
}


} // namespace Foam

// ************************************************************************* //
