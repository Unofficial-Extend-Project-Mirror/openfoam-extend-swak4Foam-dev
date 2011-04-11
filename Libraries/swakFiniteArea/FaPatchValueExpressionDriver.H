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
    Foam::FaPatchValueExpressionDriver

Description

SourceFiles
    FaPatchValueExpressionDriverI.H
    FaPatchValueExpressionDriver.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef FaPatchValueExpressionDriver_H
#define FaPatchValueExpressionDriver_H

#include "areaFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"

#include "FaCommonValueExpressionDriver.H"

namespace Foam {
    class FaPatchValueExpressionDriver;
}

using Foam::FaPatchValueExpressionDriver;

#include "FaPatchValueExpressionParser.tab.hh"

namespace parserFaPatch {
    class location;
    class FaPatchValueExpressionParser;
}

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class FaPatchValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class FaPatchValueExpressionDriver
:
    public FaCommonValueExpressionDriver
{
    // Private data

    const faPatch &patch_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const FaPatchValueExpressionDriver&);

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

    virtual const faMesh &aMesh() const;

public:

    //- Runtime type information
    TypeName("FaPatchValueExpressionDriver");

    // Constructors

    //- Construct from components
    FaPatchValueExpressionDriver(const faPatch& patch_);
    
    //- Construct from a dictionary
    FaPatchValueExpressionDriver(const dictionary& dict,const fvMesh&mesh);
    
    //- Construct from and an old Version
    FaPatchValueExpressionDriver(const faPatch& patch_,const FaPatchValueExpressionDriver& old);

    //- Construct from an id
    FaPatchValueExpressionDriver(const word& id,const fvMesh&mesh);
    
    //- Copy construct
    FaPatchValueExpressionDriver(const FaPatchValueExpressionDriver&);

    // Destructor

    ~FaPatchValueExpressionDriver();


    // Member Functions

    vectorField *makePositionField();

    vectorField *makePointField();

    vectorField *makeEdgeNormalField();

    vectorField *makeEdgeLengthField();

    vectorField *makeFaceNeighbourField();

    vectorField *makeDeltaField();

    scalarField *makeWeightsField();

    scalarField *makeEdgeIdField();

    template<class Type>
    Field<Type> *getField(const string &name);

    template<class Type>
    Field<Type> *getSurfaceNormalField(const string &name);

    template<class Type>
    Field<Type> *getPatchInternalField(const string &name);

    template<class Type>
    Field<Type> *getPatchNeighbourField(const string &name);

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

#include "FaPatchValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
