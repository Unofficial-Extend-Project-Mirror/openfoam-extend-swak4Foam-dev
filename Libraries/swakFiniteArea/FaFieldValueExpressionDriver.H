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

namespace parserFaField {
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

// public:
//     enum ExpressionType{
//         NO_TYPE,
//         SCALAR_TYPE,
//         VECTOR_TYPE,
//         LOGICAL_TYPE
//     };

private:
    const faMesh &mesh_;

//     autoPtr<areaScalarField> sresult_;
//     autoPtr<areaVectorField> vresult_;

    autoPtr<regIOobject> resultField_;

    //    ExpressionType typ_;
    word typ_;

    bool isLogical_;

    bool isSurfaceField_;

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

    //- check if field is of requested type
    template<class T>
    bool resultIsTyp(bool isLogical=false);

protected:

    //- set the result field

    //. Not everyone should be allowed to use this
    template<class T>
    void setResult(T *,bool isSurfaceField);

    template<class T>
    void setLogicalResult(T *,bool isSurfaceField);

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

    // returning fields
    template<class T>
    autoPtr<T> getField(
        const string &name,
        bool getOldTime=false
    );

    template<class FType>
    inline FType *makeConstantField(const typename FType::value_type &val);

    template<class FType>
    inline FType *makeField(const Field<typename FType::value_type> &val);

    // make constant fields
    areaVectorField *makeVectorField(
        areaScalarField *x,
        areaScalarField *y,
        areaScalarField *z
    );

    edgeVectorField *makeEdgeVectorField(
        edgeScalarField *x,
        edgeScalarField *y,
        edgeScalarField *z
    );

    areaTensorField *makeTensorField(
        areaScalarField *,areaScalarField *,areaScalarField *,
        areaScalarField *,areaScalarField *,areaScalarField *,
        areaScalarField *,areaScalarField *,areaScalarField *
    );
    edgeTensorField *makeEdgeTensorField(
        edgeScalarField *,edgeScalarField *,edgeScalarField *,
        edgeScalarField *,edgeScalarField *,edgeScalarField *,
        edgeScalarField *,edgeScalarField *,edgeScalarField *
    );

    areaSymmTensorField *makeSymmTensorField(
        areaScalarField *,areaScalarField *,areaScalarField *,
        areaScalarField *,areaScalarField *,areaScalarField *
    );

    edgeSymmTensorField *makeEdgeSymmTensorField(
        edgeScalarField *,edgeScalarField *,edgeScalarField *,
        edgeScalarField *,edgeScalarField *,edgeScalarField *
    );

    areaSphericalTensorField *makeSphericalTensorField(areaScalarField *);

    edgeSphericalTensorField *makeEdgeSphericalTensorField(edgeScalarField *);

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
    
    //- what type
    const word &typ() const { return typ_; }

    //- is this a logical field
    bool isLogical() const { return isLogical_; }

    //- is this a surface field
    bool isSurfaceField() const { return isSurfaceField_; }

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
    static void makePatches(
        GeometricField<T,faPatchField,areaMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void setValuePatches(
        GeometricField<T,faPatchField,areaMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void makePatches(
        GeometricField<T,faePatchField,edgeMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void setValuePatches(
        GeometricField<T,faePatchField,edgeMesh> &field,
        bool keepPatches,
        const wordList &fixedPatches
    );

    template<class T>
    static void copyCalculatedPatches(
        GeometricField<T,faPatchField,areaMesh> &field,
        const GeometricField<T,faPatchField,areaMesh> &orig
    );

    template<class T>
    static void copyCalculatedPatches(
        GeometricField<T,faePatchField,edgeMesh> &field,
        const GeometricField<T,faePatchField,edgeMesh> &orig
    );

    // allow acces to the setResult-method
    friend class parserFaField::FaFieldValueExpressionParser;
};

} // end namespace

#include "FaFieldValueExpressionDriverI.H"

#endif
