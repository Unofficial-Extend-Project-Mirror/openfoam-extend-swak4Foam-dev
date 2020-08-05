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

#include "foamVersion4swak.H"

#include "swakThermoTypes.H"

#ifdef FOAM_PATCHFIELDTYPE_IN_GEOFIELD_IS_NOW_PATCH
#define PatchFieldType Patch
#define GeometricBoundaryField Boundary
#endif

#ifdef FOAM_HAS_ENERGY_HE

#include "recalcThermoHeFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#ifdef FOAM_PRESSURE_MOVED_TO_FLUID_THERMO
#include "fluidThermo.H"
#define BasicThermo fluidThermo
#else
#include "basicThermo.H"
#define BasicThermo basicThermo
#endif

#include "fixedEnergyFvPatchScalarField.H"
#include "gradientEnergyFvPatchScalarField.H"
#include "mixedEnergyFvPatchScalarField.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(recalcThermoHeFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        recalcThermoHeFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

recalcThermoHeFunctionObject::recalcThermoHeFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    updateSimpleFunctionObject(name,t,dict)
{
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    start();
#endif
}

void recalcThermoHeFunctionObject::recalc()
{
    BasicThermo &thermo=const_cast<BasicThermo&>(
        obr_.lookupObject<BasicThermo>("thermophysicalProperties")
    );
    Info << "Recalculating enthalpy h" << endl;

    const volScalarField &T=thermo.T();
#ifndef FOAM_BASICTHERMO_METHOD_HE_NO_PRESSURE
    const volScalarField &p=thermo.p();
#endif

    volScalarField &h=thermo.he();

    labelList allCells(T.size());
    forAll(allCells,cellI) {
        allCells[cellI]=cellI;
    }
#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
    const_cast<scalarField&>(h.internalField().field())
#else
    h.internalField()
#endif
    = thermo.he(
#ifndef FOAM_BASICTHERMO_METHOD_HE_NO_PRESSURE
        p.internalField(),
#endif
        T.internalField(),
        allCells
    );
    forAll(h.boundaryField(), patchi)
    {
        const_cast<volScalarField::PatchFieldType&>(h.boundaryField()[patchi]) ==
            thermo.he(
#ifndef FOAM_BASICTHERMO_METHOD_HE_NO_PRESSURE
                p.boundaryField()[patchi],
#endif
                T.boundaryField()[patchi],
                patchi
            );
    }

    // hBoundaryCorrection
    volScalarField::GeometricBoundaryField& hbf =
        const_cast<volScalarField::GeometricBoundaryField&>(h.boundaryField());

    forAll(hbf, patchi)
    {
        if (isA<gradientEnergyFvPatchScalarField>(hbf[patchi]))
        {
            refCast<gradientEnergyFvPatchScalarField>(hbf[patchi]).gradient()
                = hbf[patchi].fvPatchField::snGrad();
        }
        else if (isA<mixedEnergyFvPatchScalarField>(hbf[patchi]))
        {
            refCast<mixedEnergyFvPatchScalarField>(hbf[patchi]).refGrad()
                = hbf[patchi].fvPatchField::snGrad();
        }
    }

}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

#endif

// ************************************************************************* //
