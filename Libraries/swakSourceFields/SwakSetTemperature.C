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
    2010-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SwakSetTemperature.H"
#include "polyMesh.H"
#include "cellSet.H"
#include "fvMatrix.H"

#include "FieldValueExpressionDriver.H"

#include "swakThermoTypes.H"

#include "basicThermo.H"

namespace Foam {

#ifdef FOAM_HAS_FVOPTIONS
    namespace fv {
#endif

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

    defineTypeNameAndDebug(SwakSetTemperature, 0);

    addNamedToRunTimeSelectionTable
    (
        option,
        SwakSetTemperature,
        dictionary,
        swakSetTemperature
    );

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
SwakSetTemperature::SwakSetTemperature
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    SwakSetValue<scalar>(name, modelType, dict, mesh)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SwakSetTemperature::~SwakSetTemperature()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void SwakSetTemperature::setValueInternal(
    const DynamicList<label> &cellIDs,
    const typename SwakSetValue<scalar>::resultField &result,
    fvMatrix<scalar>& eqn
) {
    const basicThermo& thermo =
        mesh_.lookupObject<basicThermo>(basicThermo::dictName);

    scalarField values(cellIDs.size());

    //    UIndirectList<Type>(values, cells_) = injectionRate_[fieldI];
    forAll(cellIDs,i)
    {
        label cellI=cellIDs[i];

        values[i]=result[cellI];
    }

    eqn.setValues(cellIDs, thermo.he(
#ifndef FOAM_BASICTHERMO_METHOD_HE_NO_PRESSURE
        thermo.p(),
#endif
        values,
        cells_
    ));
}


#ifdef FOAM_HAS_FVOPTIONS
    }
#endif

} // end namespace

// ************************************************************************* //
