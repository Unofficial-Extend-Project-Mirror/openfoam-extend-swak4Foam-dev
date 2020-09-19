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

Class
    HeatConductionRegionSolverFunctionObject

Description
    Solve the heat conduction in a solid. Replicates the solid parts of the
    chtMultiRegionFoam solver from v1906 but solves for the temperature instead
    the energy to allow regions with varying coefficients

SourceFiles
    HeatConductionRegionSolverFunctionObject.C

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef HeatConductionRegionSolverFunctionObject_H
#define HeatConductionRegionSolverFunctionObject_H

#include "SimpleRegionSolverFunctionObject.H"

#include "swakThermoTypes.H"

#ifdef FOAM_HAS_FLUIDTHERMO

#ifdef FOAM_SOLIDTHERMO_PRESSURE_IN_SUBCLASS
#include "solidPressureThermo.H"
typedef Foam::solidPressureThermo solidThermoType;
#else
#include "solidThermo.H"
typedef Foam::solidThermo solidThermoType;
#endif

#ifdef FOAM_HAS_FVOPTIONS
#ifdef FOAM_FVOPTIONS_IN_FV
#include "fvOptions.H"
#else
#include "fvIOoptionList.H"
#endif
#endif
#include "coordinateSystem.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    // //

    namespace Foam {

/*---------------------------------------------------------------------------*\
                           Class HeatConductionRegionSolverFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class HeatConductionRegionSolverFunctionObject
:
    public SimpleRegionSolverFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    HeatConductionRegionSolverFunctionObject(const HeatConductionRegionSolverFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const HeatConductionRegionSolverFunctionObject&);

    //- keep our own temperature field so that the solidThermo doesn't update it
    volScalarField T_;

    //- thermo
    autoPtr<solidThermoType> thermo_;

#ifdef FOAM_HAS_FVOPTIONS
    //- the sources
#ifdef FOAM_FVOPTIONS_IN_FV
    fv::options
#else
    fv::IOoptionList
#endif
    fvOptions_;
#endif

    autoPtr<coordinateSystem> coordinates_;

    autoPtr<volScalarField> betav_;

    autoPtr<volSymmTensorField> aniAlpha_;

protected:

    virtual bool start();

    //- actual solving
    virtual bool solveRegion();

    solidThermoType &thermo() {
        return thermo_();
    }

public:

    // Constructors

    //- Construct from components
    HeatConductionRegionSolverFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    TypeName("HeatConductionRegionSolverFunctionObject");

    // Member Functions

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#endif

#endif

// ************************************************************************* //
