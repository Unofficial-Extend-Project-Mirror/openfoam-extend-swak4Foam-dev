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
#include "interpolationTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"

namespace Foam {
    class PatchValueExpressionDriver;
}

using Foam::PatchValueExpressionDriver;

#include "PatchValueExpressionParser.tab.hh"

namespace parsers {
    class location;
    class PatchValueExpressionParser;
}

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class PatchValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class PatchValueExpressionDriver
{
    // Private data

    const fvPatch &patch_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const PatchValueExpressionDriver&);

    ExpressionResult result_;

    HashTable<ExpressionResult,word> variables_;

    HashTable<interpolationTable<scalar>,word> lines_;

public:

    // Constructors

    //- Construct from components
    PatchValueExpressionDriver(const fvPatch& patch_);
    
    //- Construct from and an old Version
    PatchValueExpressionDriver(const fvPatch& patch_,const PatchValueExpressionDriver& old);

    //- Copy construct
    PatchValueExpressionDriver(const PatchValueExpressionDriver&);

    // Destructor

    ~PatchValueExpressionDriver();


    // Member Functions

    template<class Type>
    tmp<Field<Type> > evaluate(const string &expr,bool qPoint=false);

    void evaluateVariable(const word &name,const string &expr);

    void evaluateVariableRemote(const string &remoteExpr,const word &name,const string &expr);

    void addVariables(const string &exprList,bool clear=true);

    template<class Type>
    void setResult(Field<Type> *,bool isPoint=false);

    template<class Type>
    tmp<Field<Type> > getResult(bool qPoint=false);

    word getResultType();

    ExpressionResult getUniform(label size,bool noWarning);

    void clearResult();

    template <class Op,class Type>
    Field<bool> *doCompare(Field<Type> *a,Op op,Field<Type> *b);

    template <class Op>
    Field<bool> *doLogicalOp(Field<bool> *a,Op op,Field<bool> *b);

    inline Field<bool> *doLogicalNot(Field<bool> *a);
    
    template<class Type>
    Field<Type> *doConditional(Field<bool> *d,Field<Type> *yes,Field<Type> *no);

    template<class Type>
    Field<Type> *makeField(Type,bool isPoint=false);

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
    Field<Type> *getPatchInternalField(const string &name);

    template<class Type>
    Field<Type> *getPatchNeighbourField(const string &name);

    template<class Type>
    bool is(const string &name,bool isPoint=false) const;

    template<class Type>
    bool isField(const string &name,bool isPoint=false) const;

    inline bool isLine(const string &name) const;
    scalarField *getLine(const string &name,scalar t);

    void readLines(Istream &is,bool clear=true);
    void writeLines(Ostream &os) const;

    scalarField *makeModuloField(const scalarField &a,const scalarField &b);
    scalarField *makeRandomField(label seed=0);
    scalarField *makeGaussRandomField(label seed=0);

    const Time& runTime() const;

    vectorField *composeVectorField(scalarField *,scalarField *,scalarField *);

    tensorField *composeTensorField(scalarField *,scalarField *,scalarField *,
                                    scalarField *,scalarField *,scalarField *,
                                    scalarField *,scalarField *,scalarField *);

    symmTensorField *composeSymmTensorField(scalarField *,scalarField *,scalarField *,
                                            scalarField *,scalarField *,scalarField *);

    sphericalTensorField *composeSphericalTensorField(scalarField *);

    // Error handling.
    void error (const parsers::location& l, const std::string& m);
    void error (const std::string& m);

    // Handling the scanner 
    void scan_begin();
    void scan_end();
    bool trace_scanning;

    // Handling the parser
    void parse(const std::string& f);
    Foam::string content;
    bool trace_parsing;

    // Handling temporary variables
    void clearVariables();

    template<class Type>
    Field<Type> *toFace(const Field<Type> &);

    template<class Type>
    Field<Type> *toPoint(const Field<Type> &);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "PatchValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
