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
    SolidDisplacementRegionSolverFunctionObject

Description
    Solve stresses and displacement in a solid

    Replicates the functionality of the solidDisplacementFoam soliver
    in v1906

SourceFiles
    SolidDisplacementRegionSolverFunctionObject.C

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef SolidDisplacementRegionSolverFunctionObject_H
#define SolidDisplacementRegionSolverFunctionObject_H

#include "SimpleRegionSolverFunctionObject.H"

#include "swak.H"

#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// //

namespace Foam {

/*---------------------------------------------------------------------------*\
                           Class SolidDisplacementRegionSolverFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class SolidDisplacementRegionSolverFunctionObject
:
    public SimpleRegionSolverFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    SolidDisplacementRegionSolverFunctionObject(const SolidDisplacementRegionSolverFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const SolidDisplacementRegionSolverFunctionObject&);

    //- dictionary with settings
    dictionary stressControl_;

    //- mechanical properties
    IOdictionary mechanicalProperties_;

    //- density field
    autoPtr<volScalarField> rhoPtr_;

    autoPtr<volScalarField> EPtr_;

    autoPtr<volScalarField> ERealPtr_;

    autoPtr<volScalarField> nuPtr_;

    autoPtr<volScalarField> muPtr_;

    autoPtr<volScalarField> lambdaPtr_;

    autoPtr<volScalarField> threeKPtr_;

    //- thermal properties
    IOdictionary thermalProperties_;

    //- should we calculate the thermal stress?
    bool thermalStress_;

    volScalarField threeKalpha_;

    volScalarField DT_;

    //- displacement
    volVectorField D_;

    //- pointer to the temperature field
    autoPtr<volScalarField> Tptr_;

    //- stress field
    autoPtr<volSymmTensorField> sigmaD_;

    autoPtr<volVectorField> divSigmaExp_;

protected:

    virtual bool start();

    //- actual solving
    virtual bool solveRegion();

public:

    // Constructors

    //- Construct from components
    SolidDisplacementRegionSolverFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    TypeName("SolidDisplacementRegionSolverFunctionObject");

    // Member Functions

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
