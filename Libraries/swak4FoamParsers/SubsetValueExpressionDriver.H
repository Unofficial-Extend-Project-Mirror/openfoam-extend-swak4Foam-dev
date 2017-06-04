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

Contributors/Copyright:
    2010-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
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

    //- get the value of a boundary face. Return false if the value should not be used
    template<class FieldType,class T>
    bool getBoundaryFaceValue(
        const FieldType &field,
        const label index,
        T &value
    ) const;

protected:

    inline bool useFaceValue(
        const label index
    ) const;

    // Handling the scanner
    void scan_begin();
    void scan_end();

    //- get the index from the container iterator
    template<class Iter>
    label getIndexFromIterator(const Iter &it) const;

    template<class Subset>
    label nrValidFaces(const Subset &sub) const;

    //- General Method to get the values
    template<class FieldType,class Subset,class T>
    tmp<Field<T> > getFieldInternal(
        const word &name,
        const Subset &sub,
        bool oldTime=false
    );

    //- General Method to get the values and interpolate
    template<class FieldType,class IFieldType,class Subset,class T>
    tmp<Field<T> > getFieldInternalAndInterpolate(
        const word &name,
        const Subset &sub,
        bool oldTime=false
    );

    //- General Method to get values from a field
    template<class Subset,class T>
    tmp<Field<T> > getFromFieldInternal(
        const Field<T> &original,const Subset &sub
    ) const;

    //- General Method to get values from a field
    template<class Subset,class T>
    tmp<Field<T> > getFromSurfaceFieldInternal(
        const GeometricField<T,fvsPatchField,surfaceMesh> &original,
        const Subset &sub
    ) const;

    //- set the state of the lexer to scanning vector components
    void startVectorComponent();

    //- set the state of the lexer to scanning tensor components
    void startTensorComponent();

    //- set the state to "eat characters"
    void startEatCharacters();

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

    void setAutoInterpolate(
        bool autoInterpolate,
        bool warnAutoInterpolate
    );

    // Member Functions

    virtual tmp<vectorField> makePositionField() const = 0;

    virtual tmp<scalarField> makeFaceAreaMagField() const = 0;

    virtual tmp<scalarField> makeCellVolumeField() const = 0;

    //     virtual tmp<vectorField> makePointField() const = 0;

    virtual tmp<vectorField> makeFaceNormalField() const = 0;

    virtual tmp<vectorField> makeFaceAreaField() const = 0;

    virtual tmp<scalarField> makeFaceFlipField() const = 0;

    virtual bool isCell()=0;

    tmp<scalarField> makeIdField();

//     template<class Type>
//     Field<Type> *getField(const string &name);

    virtual tmp<Field<scalar> > getScalarField(
        const word &name,
        bool oldTime=false
    )=0;

    virtual tmp<Field<vector> > getVectorField(
        const word &name,
        bool oldTime=false
    )=0;

    virtual tmp<Field<tensor> > getTensorField(
        const word &name,
        bool oldTime=false
    )=0;

    virtual tmp<Field<symmTensor> > getSymmTensorField(
        const word &name,
        bool oldTime=false
    )=0;

    virtual tmp<Field<sphericalTensor> > getSphericalTensorField(
        const word &name,
        bool oldTime=false
    )=0;

    // Handling the parser
    void parseInternal(int startToken);

    //- execute a plugin-function
    template<class T>
    autoPtr<Field<T> > evaluatePluginFunction(
        const word &name,
        const parserSubset::location &loc,
        int &scanned,
        bool isPoint=false
    );

    static const SymbolTable<SubsetValueExpressionDriver> &symbolTable();

    int startupSymbol(const word &name);

//     template<class Type>
//     virtual Field<Type> *toFace(const Field<Type> &)=0;

//     template<class Type>
//     virtual Field<Type> *toPoint(const Field<Type> &)=0;

    // allow access to protected methods
    friend class parserSubset::SubsetValueExpressionParser;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SubsetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
