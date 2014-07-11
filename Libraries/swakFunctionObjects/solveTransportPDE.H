/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
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
    solveTransportPDE

Description
    Function object that solves the transport equation

SourceFiles
    solveTransportPDE.C

Contributors/Copyright:
    2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef solveTransportPDE_H
#define solveTransportPDE_H

#include "solvePDECommonFiniteVolume.H"

namespace Foam
{

class solveTransportPDE
:
    public solvePDECommonFiniteVolume
{
    //- Disallow default bitwise copy construct
    solveTransportPDE(const solveTransportPDE&);

    //- Disallow default bitwise assignment
    void operator=(const solveTransportPDE&);

protected:

    //- the coefficent for the time-derivative
    exprString rhoExpression_;
    dimensionSet rhoDimension_;

    //- the coefficient for the laplacian term
    exprString diffusionExpression_;
    dimensionSet diffusionDimension_;

    //- the name of the phi-field.
    exprString phiExpression_;
    dimensionSet phiDimension_;

    //- the right hand side
    exprString sourceExpression_;
    dimensionSet sourceDimension_;

    //- the right hand side
    exprString sourceImplicitExpression_;
    dimensionSet sourceImplicitDimension_;

    //- do the actual solving
    void solve();

public:

    //- Runtime type information
    TypeName("solveTransportPDE");


    // Constructors

    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    solveTransportPDE
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~solveTransportPDE();


    // Member Functions

    //- Read the forces data
    virtual void read(const dictionary&);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
