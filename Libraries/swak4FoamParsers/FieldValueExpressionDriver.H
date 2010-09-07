//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_H
#define VALUE_EXPRESSION_DRIVER_H

#include <string.H>
#include <scalar.H>

#include <volFields.H>
#include <surfaceFields.H>

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
    FieldValueExpressionDriver(const FieldValueExpressionDriver &);
    void operator=(const FieldValueExpressionDriver&);

public:
    enum ExpressionType{
        NO_TYPE,
        SCALAR_TYPE,
        VECTOR_TYPE,
        LOGICAL_TYPE
    };

private:
    const string time_;
    const fvMesh &mesh_;
    const Time& runTime_;

    autoPtr<volScalarField> sresult_;
    autoPtr<volVectorField> vresult_;

    ExpressionType typ_;

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

    virtual ~FieldValueExpressionDriver();

    // necessary to override Common
    inline label size() const;
    inline label pointSize() const;
    inline const fvMesh &mesh() const;
    const word time() const;

    // Check the result type
    bool resultIsVector() { return typ_==VECTOR_TYPE; }
    bool resultIsScalar() { return typ_==SCALAR_TYPE; }
    bool resultIsLogical() { return typ_==LOGICAL_TYPE; }

    // Setting the result
    void setScalarResult(volScalarField *);
    void setLogicalResult(volScalarField *);
    void setVectorResult(volVectorField *);

    // Getting the results
    const volScalarField &getScalar() { return sresult_; }
    const volVectorField &getVector() { return vresult_; }

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();

    // Handling the parser.
    void parse (const std::string& f);

    bool isCellSet(const string &name);
    bool isCellZone(const string &name);

    // returning fields
    template<class T>
    T *getField(const string &name);

    // make constant fields
    volScalarField *makeScalarField(const scalar &val);
    volVectorField *makeVectorField(const vector &vec);
    volVectorField *makeVectorField(volScalarField *x,volScalarField *y,volScalarField *z);
    surfaceScalarField *makeSurfaceScalarField(const scalar &val);
    surfaceVectorField *makeSurfaceVectorField(const vector &vec);
    surfaceVectorField *makeSurfaceVectorField(surfaceScalarField *x,surfaceScalarField *y,surfaceScalarField *z);
    volScalarField *makeCellSetField(const string &name);
    volScalarField *makeCellZoneField(const string &name);

    // modulo operator
    volScalarField *makeModuloField(const volScalarField &a,const volScalarField &b);

    // make special fields
    surfaceVectorField *makeFacePositionField();
    surfaceVectorField *makeFaceProjectionField();
    surfaceVectorField *makeFaceField();
    surfaceScalarField *makeAreaField();
    volVectorField *makePositionField();
    volScalarField *makeDistanceField();
    volScalarField *makeRDistanceField(const volVectorField &r);
    volScalarField *makeVolumeField();
    volScalarField *makeRandomField();
    volScalarField *makeGaussRandomField();
    volScalarField *makeCellIdField();
    
    // return Time object
    const Time& runTime() const { return runTime_; }

    // Logical expressions
    template <class Field>
    Field *makeLogicalField(scalar v);

    template <class Op,class Field>
    Field *doCompare(Field *a,Op op,Field *b);

    template <class Op,class Field>
    Field *doLogicalOp(Field *a,Op op,Field *b);

    template <class Field>
    Field *doLogicalNot(Field *a);
    
    template <class T,class Field>
    T * doConditional(Field *l,T* yes,T* no,T* result);

    template<class T>
    static void makePatches(GeometricField<T,fvPatchField,volMesh> &field,bool keepPatches,const wordList &fixedPatches);

    template<class T>
    static void setValuePatches(GeometricField<T,fvPatchField,volMesh> &field,bool keepPatches,const wordList &fixedPatches);

};

} // end namespace

#include "FieldValueExpressionDriverI.H"

#endif
