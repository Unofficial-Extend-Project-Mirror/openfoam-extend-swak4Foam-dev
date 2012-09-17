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

#include "loadSLGThermoModelFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "fvMesh.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    template <>
    void modelLoadingFunctionObject<SLGThermo>::write()
    {
        if(correctModel_) {
            if(model_.valid()) {
                FatalErrorIn("modelLoadingFunctionObject::start()")
                    << "SLGThermo has no correct method"
                        << endl
                        << exit(FatalError);
            } else {
                FatalErrorIn("modelLoadingFunctionObject::start()")
                    << "Model has never been intialized"
                        << endl
                        << exit(FatalError);
            }
        }
    }

    defineTypeNameAndDebug(loadSLGThermoModelFunctionObject, 0);

    addNamedToRunTimeSelectionTable
    (
        functionObject,
        loadSLGThermoModelFunctionObject,
        dictionary,
        loadSLGThermoModel
    );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

loadSLGThermoModelFunctionObject::loadSLGThermoModelFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    modelLoadingFunctionObject<SLGThermo>(name,t,dict)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

    autoPtr<SLGThermo> loadSLGThermoModelFunctionObject::initModel()
{
    return autoPtr<SLGThermo>(
        new SLGThermo(
            dynamicCast<const fvMesh &>(
                obr()
            ),
            const_cast<basicThermo &>(
                obr().lookupObject<basicThermo>(
                    dict_.lookup("thermoName")
                )
            )
        )
    );
}


} // namespace Foam

// ************************************************************************* //
