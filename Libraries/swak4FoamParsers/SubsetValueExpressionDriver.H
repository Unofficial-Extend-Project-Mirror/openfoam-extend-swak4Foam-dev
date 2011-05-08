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
    Foam::SubsetValueExpressionDriver

Description

SourceFiles
    SubsetValueExpressionDriverI.H
    SubsetValueExpressionDriver.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef SubsetValueExpressionDriver_H
#define SubsetValueExpressionDriver_H

#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"

#include "CommonValueExpressionDriver.H"

namespace Foam {
    class SubsetValueExpressionDriver;
}

using Foam::SubsetValueExpressionDriver;

#include "SubsetValueExpressionParser.tab.hh"

namespace parserSubset {
    class location;
    class SubsetValueExpressionParser;
}

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SubsetValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class SubsetValueExpressionDriver
:
    public CommonValueExpressionDriver
{
    // Private data

    //- allow automatic interpolation
    bool autoInterpolate_;
    //- warn on automatic interpolation
    bool warnAutoInterpolate_;

    // Private Member Functions

protected:

    // Handling the scanner 
    void scan_begin();
    void scan_end();

    //- get the index from the container iterator
    template<class Iter>
    inline label getIndexFromIterator(const Iter &it);

    //- General Method to get the values
    template<class FieldType,class Subset,class T>
    Field<T> *getFieldInternal(const string &name,const Subset &sub);

    //- General Method to get the values and interpolate
    template<class FieldType,class IFieldType,class Subset,class T>
    Field<T> *getFieldInternalAndInterpolate(const string &name,const Subset &sub);

    //- General Method to get values from a field
    template<class Subset,class T>
    Field<T> *getFromFieldInternal(const Field<T> &original,const Subset &sub);
public:

     //- Runtime type information
    TypeName("SubsetValueExpressionDriver");

    // Constructors

    //- Construct from components
    SubsetValueExpressionDriver(
        bool autoInterpolate=false,
        bool warnAutoInterpolate=true
    );
    
    //- Copy construct
    SubsetValueExpressionDriver(const SubsetValueExpressionDriver&);

    //- Copy from a dictionary
    SubsetValueExpressionDriver(const dictionary&);

    // Destructor

    ~SubsetValueExpressionDriver();


    // Member Functions

    virtual vectorField *makePositionField()=0;

    virtual scalarField *makeFaceAreaMagField()=0;

    virtual scalarField *makeCellVolumeField()=0;

    //     virtual vectorField *makePointField()=0;

    virtual vectorField *makeFaceNormalField()=0;

    virtual vectorField *makeFaceAreaField()=0;

    virtual scalarField *makeFaceFlipField()=0;

    virtual bool isCell()=0;

    scalarField *makeIdField();

//     template<class Type>
//     Field<Type> *getField(const string &name);

    virtual Field<scalar> *getScalarField(const string &name)=0;

    virtual Field<vector> *getVectorField(const string &name)=0;

    virtual Field<tensor> *getTensorField(const string &name)=0;

    virtual Field<symmTensor> *getSymmTensorField(const string &name)=0;

    virtual Field<sphericalTensor> *getSphericalTensorField(const string &name)=0;

    // Handling the parser
    void parse(const std::string& f);

//     template<class Type>
//     virtual Field<Type> *toFace(const Field<Type> &)=0;

//     template<class Type>
//     virtual Field<Type> *toPoint(const Field<Type> &)=0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SubsetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
