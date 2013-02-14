/*----------------------- -*- C++ -*- ---------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description


Contributors/Copyright:
    2006-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef FieldValueExpressionDriver_H
#define FieldValueExpressionDriver_H

#include <string.H>
#include <scalar.H>

#include <volFields.H>
#include <surfaceFields.H>
#include <pointFields.H>

namespace Foam {
    class FieldValueExpressionDriver;
}

using Foam::FieldValueExpressionDriver;

#include "FieldValueExpressionParser.tab.hh"

namespace parserField {
    class location;
    class FieldValueExpressionParser;
}

#include "CommonValueExpressionDriver.H"

namespace Foam {

class FieldValueExpressionDriver
:
    public CommonValueExpressionDriver
{

    // forbid copy
    //    FieldValueExpressionDriver(const FieldValueExpressionDriver &);
    void operator=(const FieldValueExpressionDriver&);

    const string time_;
    const fvMesh &mesh_;
    const Time& runTime_;

//     autoPtr<volScalarField> sresult_;
//     autoPtr<volVectorField> vresult_;

    autoPtr<regIOobject> resultField_;

    word typ_;

    bool isLogical_;

    bool isSurfaceField_;

    bool isPointField_;

    dimensionSet resultDimension_;

    static const SymbolTable<FieldValueExpressionDriver> &symbolTable();

    int startupSymbol(const word &name);

    static word driverName_;

public:

    TypeName("FieldValueExpressionDriver");

    static const word &driverName() { return driverName_; }

    //- old style constructor
    FieldValueExpressionDriver(
        const string& time,
        const Time& runTime,
        const fvMesh &mesh,
        bool cacheReadFields=false,
        bool searchInMemory=false,
        bool searchOnDisc=true
    );

    //- simpler Constructor
    FieldValueExpressionDriver(
        const fvMesh &mesh,
        bool cacheReadFields=false,
        bool searchInMemory=false,
        bool searchOnDisc=true
    );

    //- Construct from dictionary
    FieldValueExpressionDriver(const dictionary &,const fvMesh &);

    //- Construct from an id
    FieldValueExpressionDriver(const word& id,const fvMesh&mesh);

    //- Copy construct
    FieldValueExpressionDriver(const FieldValueExpressionDriver &);

    autoPtr<CommonValueExpressionDriver> clone() const
        {
            return autoPtr<CommonValueExpressionDriver>(
                new FieldValueExpressionDriver(*this)
            );
        }

    virtual ~FieldValueExpressionDriver();

    //- read variables and tables (for objects that were not constructed by a dictionary)
    void readVariablesAndTables(const dictionary &);

    // necessary to override Common
    inline label size() const;
    inline label pointSize() const;
    inline const fvMesh &mesh() const;
    const word time() const;

    //- check if field is of requested type
    template<class T>
    bool resultIsTyp(bool isLogical=false);

protected:

    //- set the result field

    //. Not everyone should be allowed to use this
    template<class T>
    void setResult(T *,bool isSurfaceField,bool isPointField);

    template<class T>
    void setLogicalResult(T *,bool isSurfaceField,bool isPointField);

    //- set the state of the lexer to scanning vector components
    void startVectorComponent();

    //- set the state of the lexer to scanning tensor components
    void startTensorComponent();

    //- set the state to "eat characters"
    void startEatCharacters();

    //- Handling the parser.
    void parseInternal(int startToken);

    //- execute a plugin-function
    template<class T>
    autoPtr<T> evaluatePluginFunction(
        const string &name,
        const parserField::location &loc,
        int &scanned
    );

    template<class T>
    struct correctBC;

    //- weights of the entries if it is not a point field

    //- parametert size is the expected size of the weight field and
    //- will be used to determine whether this is correct.
    virtual tmp<scalarField> weightsNonPoint(
        label size
    ) const;

public:

    //- get the result field (if it is of the right type)
    template<class T>
    const T &getResult(bool correctBC=true) const;

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();

    bool isCellSet(const string &name);
    bool isCellZone(const string &name);

    bool isFaceSet(const string &name);
    bool isFaceZone(const string &name);

    bool isPointSet(const string &name);
    bool isPointZone(const string &name);

    // returning fields
    template<class T>
    tmp<T> getField(const string &name,bool getOldTime=false);

    template<class T>
    tmp<T> getPointField(const string &name,bool getOldTime=false);

    // make constant fields
    tmp<volVectorField> makeVectorField(
        const volScalarField &x,
        const volScalarField &y,
        const volScalarField &z
    );

    tmp<surfaceVectorField> makeSurfaceVectorField(
        const surfaceScalarField &x,
        const surfaceScalarField &y,
        const surfaceScalarField &z
    );

    tmp<pointVectorField> makePointVectorField(
        const pointScalarField &x,
        const pointScalarField &y,
        const pointScalarField &z
    );

    tmp<volTensorField> makeTensorField(
        const volScalarField &,const volScalarField &,const volScalarField &,
        const volScalarField &,const volScalarField &,const volScalarField &,
        const volScalarField &,const volScalarField &,const volScalarField &
    );
    tmp<surfaceTensorField> makeSurfaceTensorField(
        const surfaceScalarField &,const surfaceScalarField &,const surfaceScalarField &,
        const surfaceScalarField &,const surfaceScalarField &,const surfaceScalarField &,
        const surfaceScalarField &,const surfaceScalarField &,const surfaceScalarField &
    );

    tmp<pointTensorField> makePointTensorField(
        const pointScalarField &,const pointScalarField &,const pointScalarField &,
        const pointScalarField &,const pointScalarField &,const pointScalarField &,
        const pointScalarField &,const pointScalarField &,const pointScalarField &
    );

    tmp<volSymmTensorField> makeSymmTensorField(
        const volScalarField &,const volScalarField &,const volScalarField &,
        const volScalarField &,const volScalarField &,const volScalarField &
    );

    tmp<surfaceSymmTensorField> makeSurfaceSymmTensorField(
        const surfaceScalarField &,const surfaceScalarField &,const surfaceScalarField &,
        const surfaceScalarField &,const surfaceScalarField &,const surfaceScalarField &
    );

    tmp<pointSymmTensorField> makePointSymmTensorField(
        const pointScalarField &,const pointScalarField &,const pointScalarField &,
        const pointScalarField &,const pointScalarField &,const pointScalarField &
    );

    tmp<volSphericalTensorField> makeSphericalTensorField(const volScalarField &);

    tmp<surfaceSphericalTensorField> makeSurfaceSphericalTensorField(
        const surfaceScalarField &
    );

    tmp<pointSphericalTensorField> makePointSphericalTensorField(
        const pointScalarField &
    );

    template<class FType,class Mesh>
    inline tmp<FType> makeConstantFieldInternal(
        const typename FType::value_type &val,
        const Mesh &mesh,
        bool makeValuePatches=false
    );

    template<class FType>
    inline tmp<FType> makeConstantField(
        const typename FType::value_type &val,
        bool makeValuePatches=false
    );

    template<class FType>
    inline tmp<FType> makePointConstantField(
        const typename FType::value_type &val,
        bool makeValuePatches=false
    );

    template<class FType,class Mesh>
    inline tmp<FType> makeFieldInternal(
        const Field<typename FType::value_type> &val,
        const Mesh &actualMesh
    );

    template<class FType>
    inline tmp<FType> makeField(const Field<typename FType::value_type> &val);

    template<class FType>
    inline tmp<FType> makePointField(
        const Field<typename FType::value_type> &val
    );

    template<class Type>
    inline autoPtr<GeometricField<Type,pointPatchField,pointMesh> >
    cellToPointInterpolate(
        const GeometricField<Type,fvPatchField,volMesh> &field
    );

    template<class Type>
    inline autoPtr<GeometricField<Type,fvPatchField,volMesh> >
    pointToCellInterpolate(
        const GeometricField<Type,pointPatchField,pointMesh> &field
    );

    tmp<volScalarField> makeCellSetField(const string &name);
    tmp<volScalarField> makeCellZoneField(const string &name);
    tmp<surfaceScalarField> makeFaceSetField(const string &name);
    tmp<surfaceScalarField> makeFaceZoneField(const string &name);
    tmp<pointScalarField> makePointSetField(const string &name);
    tmp<pointScalarField> makePointZoneField(const string &name);

    tmp<surfaceScalarField> makeOnPatchField(const string &name);
    tmp<surfaceScalarField> makeInternalFaceField();

    // modulo operator
    tmp<volScalarField> makeModuloField(
        const volScalarField &a,const volScalarField &b
    );

    // make special fields
    tmp<surfaceVectorField> makeFacePositionField();
    tmp<surfaceVectorField> makeFaceProjectionField();
    tmp<surfaceVectorField> makeFaceField();
    tmp<surfaceScalarField> makeAreaField();
    tmp<volVectorField> makePositionField();
    tmp<pointVectorField> makePointPositionField();

    tmp<volScalarField> makeDistanceField();
    tmp<volScalarField> makeNearDistanceField();
    tmp<volScalarField> makeRDistanceField(const volVectorField &r);
    tmp<volScalarField> makeVolumeField();
    tmp<volScalarField> makeRandomField(label seed=0);
    tmp<volScalarField> makeGaussRandomField(label seed=0);
    tmp<volScalarField> makeCellIdField();

    //- return Time object
    const Time& runTime() const { return runTime_; }

    //- what type
    const word &typ() const { return typ_; }

    //- override type reporting from common class
    word getResultType() { return typ(); }

    //- is this a logical field
    bool isLogical() const { return isLogical_; }

    //- is this a surface field
    bool isSurfaceField() const { return isSurfaceField_; }

    //- is this a point field
    bool isPointField() const { return isPointField_; }

    // Logical expressions
    template <class Field>
    tmp<Field> makeLogicalField(scalar v);

    template <class Op,class Field>
    tmp<Field> doCompare(const Field &a,Op op,const Field &b);

    template <class Op,class Field>
    tmp<Field> doPointCompare(const Field &a,Op op,const Field &b);

    template <class Op,class Field>
    tmp<Field> doLogicalOp(const Field &a,Op op,const Field &b);

    template <class Op,class Field>
    tmp<Field> doPointLogicalOp(const Field &a,Op op,const Field &b);

    template <class Field>
    tmp<Field> doLogicalNot(const Field &a);

    template <class Field>
    tmp<Field> doPointLogicalNot(const Field &a);

    template <class T,class Field>
    tmp<T>  doConditional(
        const Field &l,
        const T & yes,
        const T & no
    );

    template <class T>
    tmp<T>  doConditional(
        const pointScalarField &l,
        const T & yes,
        const T & no
    );

private:
    inline static bool canBeValuePatch(const fvPatch &);

    inline static bool canBeValuePatch(const pointPatch &);

public:
    template<class T>
    static void makePatches(
        GeometricField<T,fvPatchField,volMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void setValuePatches(
        GeometricField<T,fvPatchField,volMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void makePatches(
        GeometricField<T,fvsPatchField,surfaceMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void setValuePatches(
        GeometricField<T,fvsPatchField,surfaceMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void makePatches(
        GeometricField<T,pointPatchField,pointMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void setValuePatches(
        GeometricField<T,pointPatchField,pointMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void setCalculatedPatches(
        GeometricField<T,fvPatchField,volMesh> &field,
        T unusedValue=pTraits<T>::zero
    );

    template<class T>
    static void setCalculatedPatches(
        GeometricField<T,fvsPatchField,surfaceMesh> &field,
        T value=pTraits<T>::zero
    );

    template<class T>
    static void setCalculatedPatches(
        GeometricField<T,pointPatchField,pointMesh> &field,
        T value=pTraits<T>::zero
    );

    template<class T>
    static void copyCalculatedPatches(
        GeometricField<T,fvPatchField,volMesh> &field,
        const GeometricField<T,fvPatchField,volMesh> &orig
    );

    template<class T>
    static void copyCalculatedPatches(
        GeometricField<T,fvsPatchField,surfaceMesh> &field,
        const GeometricField<T,fvsPatchField,surfaceMesh> &orig
    );

    template<class T>
    static void copyCalculatedPatches(
        GeometricField<T,pointPatchField,pointMesh> &field,
        const GeometricField<T,pointPatchField,pointMesh> &orig
    );

    //- return a new plugin-function
    virtual autoPtr<CommonPluginFunction> newPluginFunction(
        const word &name
    );

    //- tests for a plugin-function
    virtual bool existsPluginFunction(
        const word &name
    );

    // allow access to the setResult-method
    friend class parserField::FieldValueExpressionParser;
};

} // end namespace

#include "FieldValueExpressionDriverI.H"

#endif
