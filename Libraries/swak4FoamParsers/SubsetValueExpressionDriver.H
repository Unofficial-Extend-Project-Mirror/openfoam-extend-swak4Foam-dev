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

    const fvPatch &patch_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const SubsetValueExpressionDriver&);

protected:

    // Handling the scanner 
    void scan_begin();
    void scan_end();

    //- actual size of the field
    virtual label size() const;

    //- actual size of the supporting point-field
    virtual label pointSize() const;

    //- get the mesh we're living on
    virtual const fvMesh &mesh() const;

public:

    // Constructors

    //- Construct from components
    SubsetValueExpressionDriver(const fvPatch& patch_);
    
    //- Construct from and an old Version
    SubsetValueExpressionDriver(const fvPatch& patch_,const SubsetValueExpressionDriver& old);

    //- Copy construct
    SubsetValueExpressionDriver(const SubsetValueExpressionDriver&);

    // Destructor

    ~SubsetValueExpressionDriver();


    // Member Functions

    vectorField *makePositionField();

    vectorField *makePointField();

    vectorField *makeFaceNormalField();

    vectorField *makeFaceAreaField();

    vectorField *makeCellNeighbourField();

    vectorField *makeDeltaField();

    scalarField *makeWeightsField();

    scalarField *makeFaceIdField();

    template<class Type>
    Field<Type> *getField(const string &name);

    template<class Type>
    Field<Type> *getSurfaceNormalField(const string &name);

    template<class Type>
    Field<Type> *getSubsetInternalField(const string &name);

    template<class Type>
    Field<Type> *getSubsetNeighbourField(const string &name);

    // Handling the parser
    void parse(const std::string& f);

    template<class Type>
    Field<Type> *toFace(const Field<Type> &);

    template<class Type>
    Field<Type> *toPoint(const Field<Type> &);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SubsetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
