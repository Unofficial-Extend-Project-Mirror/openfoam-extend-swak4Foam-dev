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
    Foam::FaCommonValueExpressionDriver

Description

SourceFiles
    FaCommonValueExpressionDriverI.H
    FaCommonValueExpressionDriver.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef FaCommonValueExpressionDriver_H
#define FaCommonValueExpressionDriver_H

#include "areaFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"

#include "CommonValueExpressionDriver.H"


namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class FaCommonValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class FaCommonValueExpressionDriver
:
    public CommonValueExpressionDriver
{
    // Private data

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const FaCommonValueExpressionDriver&);

protected:

    template<class T>
    inline tmp<T> readAndRegister(const word &name);

    static const faMesh &faRegionMesh(
        const fvMesh &mesh
    );

public:

    //- Runtime type information
    TypeName("FaCommonValueExpressionDriver");

    // Constructors

    //- Construct and determine behaviour
    FaCommonValueExpressionDriver(        
        bool cacheReadFields=false,
        bool searchInMemory=true,
        bool searchOnDisc=false
    );

    //- Construct from a dictionary
    FaCommonValueExpressionDriver(const dictionary& dict);
    
    //- Copy construct
    FaCommonValueExpressionDriver(const FaCommonValueExpressionDriver&);

    // Destructor

    ~FaCommonValueExpressionDriver();


    // Member Functions

    template<class Type>
    bool is(const string &name,bool isPoint=false) const;

    template<class Type>
    bool isField(const string &name,bool isPoint=false) const;

    virtual const faMesh &aMesh() const =0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "FaCommonValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
