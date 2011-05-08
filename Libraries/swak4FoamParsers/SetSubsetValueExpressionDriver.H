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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::SetSubsetValueExpressionDriver

Description

SourceFiles
    SetSubsetValueExpressionDriverI.H
    SetSubsetValueExpressionDriver.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef SetSubsetValueExpressionDriver_H
#define SetSubsetValueExpressionDriver_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SubsetValueExpressionDriver.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SetSubsetValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class SetSubsetValueExpressionDriver
:
    public SubsetValueExpressionDriver
{
    // Private Member Functions

protected:

    const word id_;

    SetOrigin origin_;

public:

     //- Runtime type information
    TypeName("SetSubsetValueExpressionDriver");

    // Constructors

    //- Construct from components
    SetSubsetValueExpressionDriver(
        const word &id,
        const SetOrigin origin,
        bool autoInterpolate=false,
        bool warnAutoInterpolate=true
    );
    
    //- Copy construct
    SetSubsetValueExpressionDriver(const SetSubsetValueExpressionDriver&);

    //- Copy from a dictionary
    SetSubsetValueExpressionDriver(
        const dictionary&,
        const word &id,
        const SetOrigin origin
    );

    // Destructor

    ~SetSubsetValueExpressionDriver();


    // Member Functions


};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SetSubsetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
