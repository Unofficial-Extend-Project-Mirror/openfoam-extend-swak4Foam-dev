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
    Foam::PatchValueExpressionDriver

Description

SourceFiles
    PatchValueExpressionDriverI.H
    PatchValueExpressionDriver.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef PatchValueExpressionDriver_H
#define PatchValueExpressionDriver_H

#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"

#include "CommonValueExpressionDriver.H"

namespace Foam {
    class PatchValueExpressionDriver;
}

using Foam::PatchValueExpressionDriver;

#include "PatchValueExpressionParser.tab.hh"

namespace parserPatch {
    class location;
    class PatchValueExpressionParser;
}

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class PatchValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class PatchValueExpressionDriver
:
    public CommonValueExpressionDriver
{
    // Private data

    const fvPatch &patch_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const PatchValueExpressionDriver&);

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
        const string &name,
        const parserPatch::location &loc,
        int &scanned,
        bool isPoint=false
    );

    static const SymbolTable<PatchValueExpressionDriver> &symbolTable();

    int startupSymbol(const word &name);

public:

    //- Runtime type information
    TypeName("PatchValueExpressionDriver");

    static const word &driverName() { return driverName_; }

    // Constructors

    //- Construct from components
    PatchValueExpressionDriver(const fvPatch& patch_);

    //- Construct from components
    PatchValueExpressionDriver(const dictionary& dict,const fvPatch& patch_);

    //- Construct from a dictionary
    PatchValueExpressionDriver(const dictionary& dict,const fvMesh&mesh);

    //- Construct from and an old Version
    PatchValueExpressionDriver(const fvPatch& patch_,const PatchValueExpressionDriver& old);

    //- Construct from an id
    PatchValueExpressionDriver(const word& id,const fvMesh&mesh);

    //- Copy construct
    PatchValueExpressionDriver(const PatchValueExpressionDriver&);

    autoPtr<CommonValueExpressionDriver> clone() const
        {
            return autoPtr<CommonValueExpressionDriver>(
                new PatchValueExpressionDriver(*this)
            );
        }

    // Destructor

    ~PatchValueExpressionDriver();

    const fvPatch &patch() const { return patch_; }

    //- actual size of the field
    virtual label size() const;

    //- actual size of the supporting point-field
    virtual label pointSize() const;

    //- get the mesh we're living on
    virtual const fvMesh &mesh() const;

    // Member Functions

    vectorField *makePositionField();

    vectorField *makePointField();

    vectorField *makeFaceNormalField();

    vectorField *makeFaceAreaField();

    vectorField *makeCellNeighbourField();

    vectorField *makeDeltaField();

    scalarField *makeWeightsField();

    scalarField *makeFaceIdField();

    scalarField *makeNearDistField();

    template<class Type>
    Field<Type> *getField(const string &name);

    template<class Type>
    Field<Type> *getOldTimeField(const string &name);

    template<class Type>
    Field<Type> *getSurfaceNormalField(const string &name);

    template<class Type>
    Field<Type> *getPatchInternalField(const string &name);

    template<class Type>
    Field<Type> *getPatchNeighbourField(const string &name);

    template<class Type>
    Field<Type> *toFace(const Field<Type> &);

    template<class Type>
    Field<Type> *toPoint(const Field<Type> &);

    //- return a new plugin-function
    virtual autoPtr<CommonPluginFunction> newPluginFunction(
        const word &name
    );

    //- tests for a plugin-function
    virtual bool existsPluginFunction(
        const word &name
    );

    // allow access of protected methods
    friend class parserPatch::PatchValueExpressionParser;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "PatchValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
