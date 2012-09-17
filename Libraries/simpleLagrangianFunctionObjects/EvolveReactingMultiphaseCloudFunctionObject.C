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

#include "EvolveReactingMultiphaseCloudFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(EvolveReactingMultiphaseCloudFunctionObject, 0);

    addNamedToRunTimeSelectionTable
    (
        functionObject,
        EvolveReactingMultiphaseCloudFunctionObject,
        dictionary,
        evolveReactingMultiphaseCloud
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

EvolveReactingMultiphaseCloudFunctionObject::EvolveReactingMultiphaseCloudFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    EvolveCloudFunctionObject<basicReactingMultiphaseCloud>(
        name,
        t,
        dict
    )
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool EvolveReactingMultiphaseCloudFunctionObject::start()
{
    this->cloud().set(
        new basicReactingMultiphaseCloud(
            this->cloudName(),
            this->getField<volScalarField>("rhoName"),
            this->getField<volVectorField>("UName"),
            this->g(),
            const_cast<SLGThermo &>(
                this->getField<SLGThermo>("SLGThermoPhysicsName")
            )
        )
    );

    return true;
}


} // namespace Foam

// ************************************************************************* //
