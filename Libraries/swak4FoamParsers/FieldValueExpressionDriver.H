//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_H
#define VALUE_EXPRESSION_DRIVER_H

#include <string.H>
#include <scalar.H>

#include <volFields.H>
#include <surfaceFields.H>

class FieldValueExpressionDriver;

#include "FieldValueExpressionParser.tab.hh"

namespace parserField {
    class location;
    class FieldValueExpressionParser;
}

class FieldValueExpressionDriver {
public:
    enum ExpressionType{
        NO_TYPE,
        SCALAR_TYPE,
        VECTOR_TYPE,
        LOGICAL_TYPE
    };

private:
    Foam::string getTypeOfField(const Foam::string &name);
    Foam::string getTypeOfSet(const Foam::string &name);

    const Foam::string &time_;
    const Foam::fvMesh &mesh_;
    const Foam::Time& runTime_;

    Foam::volScalarField *result_;
    Foam::volVectorField *vresult_;

    ExpressionType typ_;

    // keep fields read from disc in memory
    bool cacheReadFields_;

    // should we search for the field in the registry before looking on disc
    bool searchInMemory_;

    // should we look on disc? does not make sense in the application
    bool searchOnDisc_;

public:
    FieldValueExpressionDriver(
        const Foam::string& time,
        const Foam::Time& runTime,
        const Foam::fvMesh &mesh,
        bool cacheReadFields=false,
        bool searchInMemory=false,
        bool searchOnDisc=true
    );
    virtual ~FieldValueExpressionDriver();

    // Output debug-info
    static bool debug;

    // Check the result type
    bool resultIsVector() { return typ_==VECTOR_TYPE; }
    bool resultIsScalar() { return typ_==SCALAR_TYPE; }
    bool resultIsLogical() { return typ_==LOGICAL_TYPE; }

    // Setting the result
    void setScalarResult(Foam::volScalarField *);
    void setLogicalResult(Foam::volScalarField *);
    void setVectorResult(Foam::volVectorField *);

    // Getting the results
    Foam::volScalarField *getScalar() { return result_; }
    Foam::volVectorField *getVector() { return vresult_; }

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();
    bool trace_scanning;

    // Handling the parser.
    void parse (const std::string& f);
    Foam::string content;
    bool trace_parsing;

    // Error handling.
    void error (const parserField::location& l, const std::string& m);
    void error (const std::string& m);

    // checking for variables
    template<class T>
    bool isThere(const Foam::string &name);

    bool isCellSet(const Foam::string &name);
    bool isCellZone(const Foam::string &name);

    // returning fields
    template<class T>
    T *getField(const Foam::string &name);

    // make constant fields
    Foam::volScalarField *makeScalarField(const Foam::scalar &val);
    Foam::volVectorField *makeVectorField(const Foam::vector &vec);
    Foam::volVectorField *makeVectorField(Foam::volScalarField *x,Foam::volScalarField *y,Foam::volScalarField *z);
    Foam::surfaceScalarField *makeSurfaceScalarField(const Foam::scalar &val);
    Foam::surfaceVectorField *makeSurfaceVectorField(const Foam::vector &vec);
    Foam::surfaceVectorField *makeSurfaceVectorField(Foam::surfaceScalarField *x,Foam::surfaceScalarField *y,Foam::surfaceScalarField *z);
    Foam::volScalarField *makeCellSetField(const Foam::string &name);
    Foam::volScalarField *makeCellZoneField(const Foam::string &name);

    // modulo operator
    Foam::volScalarField *makeModuloField(const Foam::volScalarField &a,const Foam::volScalarField &b);

    // make special fields
    Foam::surfaceVectorField *makeFacePositionField();
    Foam::surfaceVectorField *makeFaceProjectionField();
    Foam::surfaceVectorField *makeFaceField();
    Foam::surfaceScalarField *makeAreaField();
    Foam::volVectorField *makePositionField();
    Foam::volScalarField *makeDistanceField();
    Foam::volScalarField *makeRDistanceField(const Foam::volVectorField &r);
    Foam::volScalarField *makeVolumeField();
    Foam::volScalarField *makeRandomField();
    Foam::volScalarField *makeGaussRandomField();
    Foam::volScalarField *makeCellIdField();
    
    // return Time object
    const Foam::Time& runTime() const { return runTime_; }

    // Logical expressions
    template <class Field>
    Field *makeLogicalField(Foam::scalar v);

    template <class Op,class Field>
    Field *doCompare(Field *a,Op op,Field *b);

    template <class Op,class Field>
    Field *doLogicalOp(Field *a,Op op,Field *b);

    template <class Field>
    Field *doLogicalNot(Field *a);
    
    template <class T,class Field>
    T * doConditional(Field *l,T* yes,T* no,T* result);

    template<class T>
    static void makePatches(Foam::GeometricField<T,Foam::fvPatchField,Foam::volMesh> &field,bool keepPatches,const Foam::wordList &fixedPatches);

    template<class T>
    static void setValuePatches(Foam::GeometricField<T,Foam::fvPatchField,Foam::volMesh> &field,bool keepPatches,const Foam::wordList &fixedPatches);

};

#include "FieldValueExpressionDriverI.H"

#endif
