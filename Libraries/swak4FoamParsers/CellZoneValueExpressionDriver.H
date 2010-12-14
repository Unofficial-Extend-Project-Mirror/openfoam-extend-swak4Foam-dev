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
    Foam::CellZoneValueExpressionDriver

Description

SourceFiles
    CellZoneValueExpressionDriverI.H
    CellZoneValueExpressionDriver.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef CellZoneValueExpressionDriver_H
#define CellZoneValueExpressionDriver_H

#include "volFields.H"

#include "cellZone.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SubsetValueExpressionDriver.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class CellZoneValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class CellZoneValueExpressionDriver
:
    public SubsetValueExpressionDriver
{
    // Private data
    const cellZone &cellZone_;

    // Private Member Functions

public:

     //- Runtime type information
    TypeName("CellZoneValueExpressionDriver");

   // Constructors

    //- Construct from components
    CellZoneValueExpressionDriver(const cellZone &);
    
    //- Construct from a dictionary
    CellZoneValueExpressionDriver(const dictionary& dict,const fvMesh&mesh);
    
    //- Construct from an id
    CellZoneValueExpressionDriver(const word& id,const fvMesh&mesh);
    
    //- Copy construct
    CellZoneValueExpressionDriver(const cellZone &,const CellZoneValueExpressionDriver&);

    // Destructor

    ~CellZoneValueExpressionDriver();


    // Member Functions

    virtual vectorField *makePositionField();

    virtual scalarField *makeFaceAreaMagField();

    virtual scalarField *makeCellVolumeField();

    //     virtual vectorField *makePointField()=0;

    virtual scalarField *makeFaceFlipField();

    virtual vectorField *makeFaceNormalField();

    virtual vectorField *makeFaceAreaField();

//     template<class Type>
//     Field<Type> *getField(const string &name);

    virtual bool isCell() { return true; }

    virtual Field<scalar> *getScalarField(const string &name);

    virtual Field<vector> *getVectorField(const string &name);

    virtual Field<tensor> *getTensorField(const string &name);

    virtual Field<symmTensor> *getSymmTensorField(const string &name);

    virtual Field<sphericalTensor> *getSphericalTensorField(const string &name);

    // Handling the parser
    //    void parse(const std::string& f);

    inline virtual label size() const;

    inline virtual label pointSize() const;

    inline virtual const fvMesh &mesh() const;

//     template<class Type>
//     virtual Field<Type> *toFace(const Field<Type> &)=0;

//     template<class Type>
//     virtual Field<Type> *toPoint(const Field<Type> &)=0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "CellZoneValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
