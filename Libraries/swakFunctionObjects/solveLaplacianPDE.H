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

Class
    solveLaplacianPDE

Description
    Function object that solves the laplacian (Poisson) equation

SourceFiles
    solveLaplacianPDE.C

\*---------------------------------------------------------------------------*/

#ifndef solveLaplacianPDE_H
#define solveLaplacianPDE_H

#include "solvePDECommon.H"

namespace Foam
{

class solveLaplacianPDE
:
    public solvePDECommon
{
    //- Disallow default bitwise copy construct
    solveLaplacianPDE(const solveLaplacianPDE&);
    
    //- Disallow default bitwise assignment
    void operator=(const solveLaplacianPDE&);

protected:

    //- the coefficent for the time-derivative
    string rhoExpression_;
    dimensionSet rhoDimension_;

    //- the coefficient for the laplacian term
    string lambdaExpression_;
    dimensionSet lambdaDimension_;

    //- the right hand side
    string sourceExpression_;
    dimensionSet sourceDimension_;

    //- the right hand side
    string sourceImplicitExpression_;
    dimensionSet sourceImplicitDimension_;

    //- do the actual solving
    void solve();

public:

    //- Runtime type information
    TypeName("solveLaplacianPDE");


    // Constructors
    
    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    solveLaplacianPDE
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~solveLaplacianPDE();


    // Member Functions

    //- Read the forces data
    virtual void read(const dictionary&);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
