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
    Foam::PatchResult

Description

SourceFiles
    PatchResultI.H
    PatchResult.C
    PatchResultIO.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef PatchResult_H
#define PatchResult_H

#include "Field.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class PatchResult Declaration
\*---------------------------------------------------------------------------*/

class PatchResult
{
    // Private data

    string valType_;

    void *valPtr_;
    
    bool isPoint_;

    void uglyDelete();

    template<class Type>
    PatchResult getUniformInternal(const label size,bool noWarn);

public:

    // Constructors

    //- Construct null
    PatchResult();

    //- Construct as copy
    PatchResult(const PatchResult&);

    // Destructor

    ~PatchResult();

    void operator=(const PatchResult&);

    template<class Type>
    inline void setResult(Field<Type> *,bool isPoint=false);

    template<class Type>
    inline void setResult(const Field<Type> &,bool isPoint=false);

    template<class Type>
    inline void setResult(Type val,label size);

    template<class Type>
    inline tmp<Field<Type> > getResult(bool keep=false);

    // Construct a uniform field from the current results
    PatchResult getUniform(const label size,bool noWarn);

    void clearResult();

    inline const string &type() const;

    inline bool isPoint() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "PatchResultI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
