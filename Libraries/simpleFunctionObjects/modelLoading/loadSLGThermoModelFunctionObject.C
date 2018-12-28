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

#include "swak.H"

#ifndef FOAM_NO_SLG_THERMOPHYSICS

#include "loadSLGThermoModelFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "fvMesh.H"
#include "volFields.H"
#include "surfaceFields.H"

#include "swakThermoTypes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    template <>
    void modelLoadingFunctionObject<SLGThermo>::writeSimple()
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
    this->read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

autoPtr<SLGThermo> loadSLGThermoModelFunctionObject::initModel()
{
    autoPtr<SLGThermo> result(
        new SLGThermo(
            dynamicCast<const fvMesh &>(
                obr()
            ),
            const_cast<swakFluidThermoType &>(
                obr().lookupObject<swakFluidThermoType>(
                    word(dict_.lookup("thermoName"))
                )
            )
        )
    );

    return result;
}


} // namespace Foam

#endif

// ************************************************************************* //
