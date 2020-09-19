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
    PotentialFlowRegionSolverFunctionObject

Description
    Solves the potential flow equation. Replicates potentialFoam from v1906

SourceFiles
    PotentialFlowRegionSolverFunctionObject.C

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef PotentialFlowRegionSolverFunctionObject_H
#define PotentialFlowRegionSolverFunctionObject_H

#include "SimpleRegionSolverFunctionObject.H"

#ifdef FOAM_THEREIS_NO_PISO_CONTROL_CLASS
#include "pimpleControl.H"
#else
#include "pisoControl.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class PotentialFlowRegionSolverFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class PotentialFlowRegionSolverFunctionObject
:
    public SimpleRegionSolverFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    PotentialFlowRegionSolverFunctionObject(const PotentialFlowRegionSolverFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const PotentialFlowRegionSolverFunctionObject&);

protected:

    //- velocity field
    volVectorField U_;

    //- controls
#ifdef FOAM_THEREIS_NO_PISO_CONTROL_CLASS
    pimpleControl
#else
    pisoControl
#endif
    potentialFlow_;

    //- flows
    autoPtr<surfaceScalarField> phi_;

    //- pressure
    autoPtr<volScalarField> p_;

    //- Potential
    autoPtr<volScalarField> Phi_;


    virtual bool start();

    //- actual solving
    virtual bool solveRegion();

public:

    // Constructors

    //- Construct from components
    PotentialFlowRegionSolverFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    TypeName("PotentialFlowRegionSolverFunctionObject");

    // Member Functions

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
