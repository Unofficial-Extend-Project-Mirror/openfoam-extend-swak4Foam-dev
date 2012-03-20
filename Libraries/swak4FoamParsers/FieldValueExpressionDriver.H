//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_H
#define VALUE_EXPRESSION_DRIVER_H

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

public:

    TypeName("FieldValueExpressionDriver");

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

public:
    //- get the result field (if it is of the right type)
    template<class T>
    const T &getResult();

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();

    // Handling the parser.
    void parse (const std::string& f);

    bool isCellSet(const string &name);
    bool isCellZone(const string &name);

    bool isFaceSet(const string &name);
    bool isFaceZone(const string &name);

    bool isPointSet(const string &name);
    bool isPointZone(const string &name);

    // returning fields
    template<class T>
    T* getField(const string &name,bool getOldTime=false);

    template<class T>
    T* getPointField(const string &name,bool getOldTime=false);

    // make constant fields
    volVectorField *makeVectorField(
        volScalarField *x,
        volScalarField *y,
        volScalarField *z
    );

    surfaceVectorField *makeSurfaceVectorField(
        surfaceScalarField *x,
        surfaceScalarField *y,
        surfaceScalarField *z
    );

    pointVectorField *makePointVectorField(
        pointScalarField *x,
        pointScalarField *y,
        pointScalarField *z
    );

    volTensorField *makeTensorField(
        volScalarField *,volScalarField *,volScalarField *,
        volScalarField *,volScalarField *,volScalarField *,
        volScalarField *,volScalarField *,volScalarField *
    );
    surfaceTensorField *makeSurfaceTensorField(
        surfaceScalarField *,surfaceScalarField *,surfaceScalarField *,
        surfaceScalarField *,surfaceScalarField *,surfaceScalarField *,
        surfaceScalarField *,surfaceScalarField *,surfaceScalarField *
    );

    pointTensorField *makePointTensorField(
        pointScalarField *,pointScalarField *,pointScalarField *,
        pointScalarField *,pointScalarField *,pointScalarField *,
        pointScalarField *,pointScalarField *,pointScalarField *
    );

    volSymmTensorField *makeSymmTensorField(
        volScalarField *,volScalarField *,volScalarField *,
        volScalarField *,volScalarField *,volScalarField *
    );

    surfaceSymmTensorField *makeSurfaceSymmTensorField(
        surfaceScalarField *,surfaceScalarField *,surfaceScalarField *,
        surfaceScalarField *,surfaceScalarField *,surfaceScalarField *
    );

    pointSymmTensorField *makePointSymmTensorField(
        pointScalarField *,pointScalarField *,pointScalarField *,
        pointScalarField *,pointScalarField *,pointScalarField *
    );

    volSphericalTensorField *makeSphericalTensorField(volScalarField *);

    surfaceSphericalTensorField *makeSurfaceSphericalTensorField(surfaceScalarField *);

    pointSphericalTensorField *makePointSphericalTensorField(pointScalarField *);

    template<class FType,class Mesh>
    inline FType *makeConstantFieldInternal(
        const typename FType::value_type &val,
        const Mesh &mesh
    );

    template<class FType>
    inline FType *makeConstantField(const typename FType::value_type &val);

    template<class FType>
    inline FType *makePointConstantField(const typename FType::value_type &val);

    template<class FType,class Mesh>
    inline FType *makeFieldInternal(
        const Field<typename FType::value_type> &val,
        const Mesh &actualMesh
    );

    template<class FType>
    inline FType *makeField(const Field<typename FType::value_type> &val);

    template<class FType>
    inline FType *makePointField(const Field<typename FType::value_type> &val);

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

    volScalarField *makeCellSetField(const string &name);
    volScalarField *makeCellZoneField(const string &name);
    surfaceScalarField *makeFaceSetField(const string &name);
    surfaceScalarField *makeFaceZoneField(const string &name);
    pointScalarField *makePointSetField(const string &name);
    pointScalarField *makePointZoneField(const string &name);

    // modulo operator
    volScalarField *makeModuloField(const volScalarField &a,const volScalarField &b);

    // make special fields
    surfaceVectorField *makeFacePositionField();
    surfaceVectorField *makeFaceProjectionField();
    surfaceVectorField *makeFaceField();
    surfaceScalarField *makeAreaField();
    volVectorField *makePositionField();
    pointVectorField *makePointPositionField();

    volScalarField *makeDistanceField();
    volScalarField *makeNearDistanceField();
    volScalarField *makeRDistanceField(const volVectorField &r);
    volScalarField *makeVolumeField();
    volScalarField *makeRandomField(label seed=0);
    volScalarField *makeGaussRandomField(label seed=0);
    volScalarField *makeCellIdField();
    
    //- return Time object
    const Time& runTime() const { return runTime_; }

    //- what type
    const word &typ() const { return typ_; }

    //- is this a logical field
    bool isLogical() const { return isLogical_; }

    //- is this a surface field
    bool isSurfaceField() const { return isSurfaceField_; }

    //- is this a point field
    bool isPointField() const { return isPointField_; }

    // Logical expressions
    template <class Field>
    Field *makeLogicalField(scalar v);

    template <class Op,class Field>
    Field *doCompare(Field *a,Op op,Field *b);

    template <class Op,class Field>
    Field *doPointCompare(Field *a,Op op,Field *b);

    template <class Op,class Field>
    Field *doLogicalOp(Field *a,Op op,Field *b);

    template <class Op,class Field>
    Field *doPointLogicalOp(Field *a,Op op,Field *b);

    template <class Field>
    Field *doLogicalNot(Field *a);
    
    template <class Field>
    Field *doPointLogicalNot(Field *a);
    
    template <class T,class Field>
    T * doConditional(Field *l,T* yes,T* no,T* result);

    template <class T>
    T * doConditional(pointScalarField *l,T* yes,T* no,T* result);

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

    // allow access to the setResult-method
    friend class parserField::FieldValueExpressionParser;
};

} // end namespace

#include "FieldValueExpressionDriverI.H"

#endif
