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

Contributors/Copyright:
    2010-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
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

    static word driverName_;

protected:

    // Handling the scanner
    void scan_begin();
    void scan_end();

    //- set the state of the lexer to scanning vector components
    void startVectorComponent();

    //- set the state of the lexer to scanning tensor components
    void startTensorComponent();

    //- set the state to "eat characters"
    void startEatCharacters();

    // Handling the parser
    void parseInternal(int startToken);

    //- execute a plugin-function
    template<class T>
    autoPtr<Field<T> > evaluatePluginFunction(
        const word &name,
        const parserFaPatch::location &loc,
        int &scanned,
        bool isPoint=false
    );

    static const SymbolTable<FaPatchValueExpressionDriver> &symbolTable();

    int startupSymbol(const word &name);

    //- weights of the entries if it is not a point field

    //- parametert size is the expected size of the weight field and
    //- will be used to determine whether this is correct.
    virtual tmp<scalarField> weightsNonPoint(
        label size
    ) const;

public:

    //- Runtime type information
    TypeName("FaPatchValueExpressionDriver");

    //- entity this driver operates on
    typedef faPatch EntityType;

    static const word &driverName() { return driverName_; }

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

    autoPtr<CommonValueExpressionDriver> clone() const
        {
            return autoPtr<CommonValueExpressionDriver>(
                new FaPatchValueExpressionDriver(*this)
            );
        }

    // Destructor

    ~FaPatchValueExpressionDriver();


    // Member Functions

    const faPatch & patch() const { return patch_; }

    //- actual size of the field
    virtual label size() const;

    //- actual size of the supporting point-field
    virtual label pointSize() const;

    //- get the mesh we're living on
    virtual const fvMesh &mesh() const;

    virtual const faMesh &aMesh() const;

    tmp<vectorField> makePositionField() const;

    tmp<vectorField> makePointField() const;

    tmp<vectorField> makeEdgeNormalField() const;

    tmp<vectorField> makeEdgeLengthField() const;

    tmp<vectorField> makeFaceNeighbourField() const;

    tmp<vectorField> makeDeltaField() const;

    tmp<scalarField> makeWeightsField() const;

    tmp<scalarField> makeEdgeIdField() const;

    template<class Type>
    tmp<Field<Type> > getField(const word &name);

    template<class Type>
    tmp<Field<Type> > getOldTimeField(const word &name);

    template<class Type>
    tmp<Field<Type> > getSurfaceNormalField(const word &name);

    template<class Type>
    tmp<Field<Type> > getPatchInternalField(const word &name);

    template<class Type>
    tmp<Field<Type> > getPatchNeighbourField(const word &name);

    template<class Type>
    tmp<Field<Type> > toFace(const Field<Type> &);

    template<class Type>
    tmp<Field<Type> > toPoint(const Field<Type> &);

    //- return a new plugin-function
    virtual autoPtr<CommonPluginFunction> newPluginFunction(
        const word &name
    );

    //- tests for a plugin-function
    virtual bool existsPluginFunction(
        const word &name
    );

    // allow access of protected methods
    friend class parserFaPatch::FaPatchValueExpressionParser;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "FaPatchValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
