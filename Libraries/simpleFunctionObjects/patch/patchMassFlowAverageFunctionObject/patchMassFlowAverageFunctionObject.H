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

Class
    patchMassFlowAverageFunctionObject

Description
    Mass-Flow-Weighted Averaging the fields on the given patches

SourceFiles
    patchMassFlowAverageFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef patchMassFlowAverageFunctionObject_H
#define patchMassFlowAverageFunctionObject_H

#include "patchFieldFunctionObject.H"
#include "SolverInfo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class patchMassFlowAverageFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class patchMassFlowAverageFunctionObject
:
    public patchFieldFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    patchMassFlowAverageFunctionObject(const patchMassFlowAverageFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const patchMassFlowAverageFunctionObject&);

    //- integrate single field on all patches
    template <class T>
    Field<T> average(const word& fieldName,T preset) const;

    SolverInfo solver_;

protected:

    word dirName();

    virtual scalarField process(const word& fieldName,scalar preset);
    virtual Field<vector> process(const word& fieldName,vector preset);
    virtual Field<sphericalTensor> process(const word& fieldName,sphericalTensor preset);
    virtual Field<symmTensor> process(const word& fieldName,symmTensor preset);
    virtual Field<tensor> process(const word& fieldName,tensor preset);

public:

    //- Runtime type information
    TypeName("patchMassFlowAverage");


    // Constructors

    //- Construct from components
    patchMassFlowAverageFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "patchMassFlowAverageFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
