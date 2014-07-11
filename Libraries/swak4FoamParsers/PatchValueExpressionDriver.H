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

Contributors/Copyright:
    2009-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef PatchValueExpressionDriver_H
#define PatchValueExpressionDriver_H

#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"

#include "CommonValueExpressionDriver.H"

#include "interpolation.H"

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

    //- the patch we're living on
    const fvPatch &patch_;

    //- dictionary with the interpolation-schemes for mapping
    dictionary mappingInterpolationSchemes_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const PatchValueExpressionDriver&);

    static word driverName_;

    //- give back an interpolation-scheme specified by name and type
    template<class Type>
    const interpolation<Type> &getInterpolation(
        const GeometricField< Type, fvPatchField, volMesh > &psi
    );

    //- get the specification of the interpolations by name
    const word getInterpolationScheme(const word &name);

    //- HashPtrTable of interpolations
    HashPtrTable<interpolation<scalar> > interpolationScalar_;
    HashPtrTable<interpolation<vector> > interpolationVector_;
    HashPtrTable<interpolation<tensor> > interpolationTensor_;
    HashPtrTable<interpolation<symmTensor> > interpolationSymmTensor_;
    HashPtrTable<interpolation<sphericalTensor> > interpolationSphericalTensor_;

    //- table with the interpolation scheme (depending on the type)
    template <class Type>
    HashPtrTable<interpolation<Type> > &interpolations();

    //- map a field that is as long as the patch
    template<class T>
    tmp<Field<T> > mapField(tmp<Field<T> > orig);

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
        const parserPatch::location &loc,
        int &scanned,
        bool isPoint=false
    );

    static const SymbolTable<PatchValueExpressionDriver> &symbolTable();

    int startupSymbol(const word &name);

    //- check whether the patch maps from another one
    autoPtr<ExpressionResult> getRemoteResult(
        CommonValueExpressionDriver &otherDriver
    );

    //- weights of the entries if it is not a point field

    //- parametert size is the expected size of the weight field and
    //- will be used to determine whether this is correct.
    virtual tmp<scalarField> weightsNonPoint(
        label size
    ) const;

public:

    //- Runtime type information
    TypeName("PatchValueExpressionDriver");

    //- entity this driver operates on
    typedef fvPatch EntityType;

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

    tmp<vectorField> makePositionField();

    tmp<vectorField> makePointField();

    tmp<vectorField> makeFaceNormalField();

    tmp<vectorField> makeFaceAreaField();

    tmp<vectorField> makeCellNeighbourField();

    tmp<vectorField> makeDeltaField();

    tmp<scalarField> makeWeightsField();

    tmp<scalarField> makeFaceIdField();

    tmp<scalarField> makeNearDistField();

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

    //- for a mapped patch returns the mapped field from "the other side""
    template<class T>
    tmp<Field<T> > mapField(const word &name,bool internalField=false);

    //- for a mapped point-patch returns the mapped field from "the other side""
    template<class T>
    tmp<Field<T> > mapPointField(const word &name);

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
