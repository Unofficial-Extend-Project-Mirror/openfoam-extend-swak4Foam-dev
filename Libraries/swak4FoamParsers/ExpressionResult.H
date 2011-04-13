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

#include "Field.H"

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

    void uglyDelete();

    template<class Type>
    ExpressionResult getUniformInternal(const label size,bool noWarn);

public:

    // Constructors

    //- Construct null
    ExpressionResult();

    //- Construct as copy
    ExpressionResult(const ExpressionResult&);

    //- Construct from a dictionary
    ExpressionResult(const dictionary&);

    // Destructor

    ~ExpressionResult();

    void operator=(const ExpressionResult&);

    template<class Type>
    inline void setResult(Field<Type> *,bool isPoint=false);

    template<class Type>
    inline void setResult(const Field<Type> &,bool isPoint=false);

    template<class Type>
    inline void setResult(Type val,label size);

    template<class Type>
    inline tmp<Field<Type> > getResult(bool keep=false);

    // Construct a uniform field from the current results
    ExpressionResult getUniform(const label size,bool noWarn);

    void clearResult();

    inline const string &type() const;

    inline bool isPoint() const;

    bool hasValue() const;

    friend Ostream & operator<<(Ostream &,const ExpressionResult &);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResultI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
