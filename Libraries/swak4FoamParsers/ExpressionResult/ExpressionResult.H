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
    Foam::ExpressionResult

Description

SourceFiles
    ExpressionResultI.H
    ExpressionResult.C
    ExpressionResultIO.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef ExpressionResult_H
#define ExpressionResult_H

#include "foamVersion4swak.H"
#if (FOAM_VERSION4SWAK_MAJOR < 2)
#error "This swak4Foam-version is only used for the 2.x-versions of OpenFOAM. For lower versions there is a special branch"
#endif

#include "Field.H"
#include "dimensionedType.H"
#include "IOobject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class ExpressionResult Declaration
\*---------------------------------------------------------------------------*/

class ExpressionResult
{
    // Private data

    string valType_;

    void *valPtr_;

    bool isPoint_;

    bool isSingleValue_;

    //- alternative if anything different than a plain field should be stored
    autoPtr<IOobject> generalContent_;

    void uglyDelete();

    template<class Type>
    ExpressionResult getUniformInternal(const label size,bool noWarn) const;

    template<class Type>
    inline void setResultInternal(Field<Type> *,bool isPoint=false);

    template<class Type>
    inline void setResultInternal(const Field<Type> &,bool isPoint=false);

    template<class Type>
    inline void setResultInternal(Type val,label size);

    template<class Type>
    inline void setSingleValueInternal(Type val);

    template <typename T>
    inline void setObjectResultInternal(autoPtr<T> o);

protected:

    //- ugly workaround for templated methods not being virtual
    inline virtual ExpressionResult &toBeSet();

public:
    //- Runtime type information
    TypeName("ExpressionResult");

    // Constructors

    //- Construct null
    ExpressionResult();

    //- Construct as copy
    ExpressionResult(const ExpressionResult&);

    //- Construct from a dictionary
    ExpressionResult(const dictionary&,bool isSingleValue=false);

    //- Construct from a field
    template<class Type>
    ExpressionResult(const Field<Type>  &f);

    //- construct for an IOobject
    template<class Type>
    ExpressionResult(autoPtr<Type> o);

    //- Construct from a dimensioned value
    template<class Type>
    ExpressionResult(const dimensioned<Type>  &f);

    template <int, class T = void> struct enable_if_rank0 {};
    template <class T> struct enable_if_rank0<0, T> { typedef T type; };

    //- Construct from a value of rank 0

    //- the enable_if_rank0-business avoids that the template is used for (for instance)
    //- GeometricField
    template<class Type>
    ExpressionResult(
        const Type  &f,
        typename enable_if_rank0<pTraits<Type>::rank>::type* dummy=0
    );

    // Destructor

    virtual ~ExpressionResult();

    void operator=(const ExpressionResult&);

    template<class Type>
    inline void setResult(Field<Type> *,bool isPoint=false);

    template<class Type>
    inline void setResult(const Field<Type> &,bool isPoint=false);

    template<class Type>
    inline void setResult(Type val,label size);

    template<class Type>
    inline void setSingleValue(Type val);

    template <typename T>
    inline void setObjectResult(autoPtr<T> o);

    template<class Type>
    inline tmp<Field<Type> > getResult(bool keep=false);

    // couldn't get specialisation with Field<Type> to work. Maybe later. Maybe forbidden
    template<class Type>
    inline tmp<Type> getObjectResult(bool keep=false);

    // Construct a uniform field from the current results
    ExpressionResult getUniform(const label size,bool noWarn) const;

    template<template<class> class BinOp,class Type>
    inline Type getReduced(BinOp<Type> op,Type initial=pTraits<Type>::zero);

    void clearResult();

    inline const string &valueType() const;

    inline bool isPoint() const;

    inline bool isSingleValue() const;

    bool hasValue() const;

    inline bool isObject() const;

    label size() const;

    friend Ostream & operator<<(Ostream &,const ExpressionResult &);

    friend Istream & operator>>(Istream &,ExpressionResult &);

    friend ExpressionResult operator*(const scalar &,const ExpressionResult &);

    friend ExpressionResult operator+(
        const ExpressionResult &,const ExpressionResult &
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResultI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
