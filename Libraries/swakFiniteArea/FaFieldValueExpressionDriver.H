//  ICE Revision: $Id$ 

#ifndef FAFIELD_VALUE_EXPRESSION_DRIVER_H
#define FAFIELD_VALUE_EXPRESSION_DRIVER_H

#include <string.H>
#include <scalar.H>

#include <areaFields.H>
#include <edgeFields.H>

namespace Foam {
    class FaFieldValueExpressionDriver;
}

using Foam::FaFieldValueExpressionDriver;

#include "FaFieldValueExpressionParser.tab.hh"

namespace parserField {
    class location;
    class FaFieldValueExpressionParser;
}

#include "FaCommonValueExpressionDriver.H"

namespace Foam {

class FaFieldValueExpressionDriver
:
    public FaCommonValueExpressionDriver
{

    // forbid copy
    FaFieldValueExpressionDriver(const FaFieldValueExpressionDriver &);
    void operator=(const FaFieldValueExpressionDriver&);

public:
    enum ExpressionType{
        NO_TYPE,
        SCALAR_TYPE,
        VECTOR_TYPE,
        LOGICAL_TYPE
    };

private:
    const faMesh &mesh_;

    autoPtr<areaScalarField> sresult_;
    autoPtr<areaVectorField> vresult_;

    ExpressionType typ_;

    dimensionSet resultDimension_;

public:
    TypeName("FaFieldValueExpressionDriver");

    //- simpler Constructor
    FaFieldValueExpressionDriver(
        const fvMesh &mesh,
        bool cacheReadFields=false,
        bool searchInMemory=false,
        bool searchOnDisc=true
    );

    //- Construct from dictionary
    FaFieldValueExpressionDriver(const dictionary &,const fvMesh &);

    //- Construct from an id
    FaFieldValueExpressionDriver(const word& id,const fvMesh&mesh);
    
    virtual ~FaFieldValueExpressionDriver();

    // necessary to override Common
    inline label size() const;
    inline label pointSize() const;
    inline const fvMesh &mesh() const;
    inline const faMesh &aMesh() const;

    // Check the result type
    bool resultIsVector() { return typ_==VECTOR_TYPE; }
    bool resultIsScalar() { return typ_==SCALAR_TYPE; }
    bool resultIsLogical() { return typ_==LOGICAL_TYPE; }

    // Setting the result
    void setScalarResult(areaScalarField *);
    void setLogicalResult(areaScalarField *);
    void setVectorResult(areaVectorField *);

    // Getting the results
    const areaScalarField &getScalar() { return sresult_; }
    const areaVectorField &getVector() { return vresult_; }

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();

    // Handling the parser.
    void parse (const std::string& f);

    // returning fields
    template<class T>
    T *getField(const string &name);

    template<class FType>
    inline FType *makeConstantField(const typename FType::value_type &val);

    template<class FType>
    inline FType *makeField(const Field<typename FType::value_type> &val);

    // make constant fields
    areaVectorField *makeVectorField(areaScalarField *x,areaScalarField *y,areaScalarField *z);
    edgeVectorField *makeEdgeVectorField(edgeScalarField *x,edgeScalarField *y,edgeScalarField *z);
    // areaScalarField *makeCellSetField(const string &name);
    // areaScalarField *makeCellZoneField(const string &name);
    // edgeScalarField *makeFaceSetField(const string &name);
    // edgeScalarField *makeFaceZoneField(const string &name);

    // modulo operator
    areaScalarField *makeModuloField(const areaScalarField &a,const areaScalarField &b);

    // make special fields
    edgeVectorField *makeEdgePositionField();
    edgeVectorField *makeEdgeProjectionField();
    edgeVectorField *makeEdgeField();
    edgeScalarField *makeLengthField();
    areaVectorField *makePositionField();
    areaScalarField *makeRDistanceField(const areaVectorField &r);
    areaScalarField *makeAreaField();
    areaScalarField *makeRandomField(label seed=0);
    areaScalarField *makeGaussRandomField(label seed=0);
    areaScalarField *makeCellIdField();
    
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
    static void makePatches(GeometricField<T,faPatchField,areaMesh> &field,bool keepPatches,const wordList &fixedPatches);

    template<class T>
    static void setValuePatches(GeometricField<T,faPatchField,areaMesh> &field,bool keepPatches,const wordList &fixedPatches);

};

} // end namespace

#include "FaFieldValueExpressionDriverI.H"

#endif
