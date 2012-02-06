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
    Foam::FaceSetValueExpressionDriver

Description

SourceFiles
    FaceSetValueExpressionDriverI.H
    FaceSetValueExpressionDriver.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef FaceSetValueExpressionDriver_H
#define FaceSetValueExpressionDriver_H

#include "volFields.H"

#include "faceSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SetSubsetValueExpressionDriver.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class FaceSetValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class FaceSetValueExpressionDriver
:
    public SetSubsetValueExpressionDriver
{
    // Private data
    autoPtr<faceSet> faceSet_;

    // Private Member Functions

protected:

    //- update stuff that needs updating
    virtual bool update();

public:

     //- Runtime type information
    TypeName("FaceSetValueExpressionDriver");

   // Constructors

    //- Construct from components
    FaceSetValueExpressionDriver(
        const faceSet &,
        bool autoInterpolate=false,
        bool warnAutoInterpolate=true
    );
    
    //- Construct from a dictionary
    FaceSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh);
    
    //- Construct from an id
    FaceSetValueExpressionDriver(const word& id,const fvMesh&mesh);
    
    //- Copy construct
    FaceSetValueExpressionDriver(const faceSet &,const FaceSetValueExpressionDriver&);

    autoPtr<CommonValueExpressionDriver> clone() const
        {
            return autoPtr<CommonValueExpressionDriver>(
                new FaceSetValueExpressionDriver(faceSet_(),*this)
            );
        }


    // Destructor

    ~FaceSetValueExpressionDriver();


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

    virtual bool isCell() { return false; }

    virtual Field<scalar> *getScalarField(const string &name,bool oldTime=false);

    virtual Field<vector> *getVectorField(const string &name,bool oldTime=false);

    virtual Field<tensor> *getTensorField(const string &name,bool oldTime=false);

    virtual Field<symmTensor> *getSymmTensorField(const string &name,bool oldTime=false);

    virtual Field<sphericalTensor> *getSphericalTensorField(const string &name,bool oldTime=false);

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

#include "FaceSetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
