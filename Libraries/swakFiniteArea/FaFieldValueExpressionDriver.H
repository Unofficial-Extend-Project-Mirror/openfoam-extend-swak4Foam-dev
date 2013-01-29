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
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

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

    static const SymbolTable<FaFieldValueExpressionDriver> &symbolTable();

    int startupSymbol(const word &name);

    static word driverName_;

public:
    TypeName("FaFieldValueExpressionDriver");

    static const word &driverName() { return driverName_; }

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
    label size() const;
    label pointSize() const;
    const fvMesh &mesh() const;
    const faMesh &aMesh() const;

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

    //- set the state to "eat characters"
    void startEatCharacters();

    //- Handling the parser.
    void parseInternal(int startToken);

    //- execute a plugin-function
    template<class T>
    autoPtr<T> evaluatePluginFunction(
        const string &name,
        const parserFaField::location &loc,
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
    const T &getResult() const;

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();

    // returning fields
    template<class T>
    autoPtr<T> getField(
        const string &name,
        bool getOldTime=false
    );

    template<class FType>
    inline tmp<FType> makeConstantField(
        const typename FType::value_type &val
    ) const;

    template<class FType>
    inline tmp<FType> makeField(const Field<typename FType::value_type> &val);

    // make constant fields
    tmp<areaVectorField> makeVectorField(
        const areaScalarField &x,
        const areaScalarField &y,
        const areaScalarField &z
    );

    tmp<edgeVectorField> makeEdgeVectorField(
        const edgeScalarField &x,
        const edgeScalarField &y,
        const edgeScalarField &z
    );

    tmp<areaTensorField> makeTensorField(
        const areaScalarField &,const areaScalarField &,const areaScalarField &,
        const areaScalarField &,const areaScalarField &,const areaScalarField &,
        const areaScalarField &,const areaScalarField &,const areaScalarField &
    );
    tmp<edgeTensorField> makeEdgeTensorField(
        const edgeScalarField &,const edgeScalarField &,const edgeScalarField &,
        const edgeScalarField &,const edgeScalarField &,const edgeScalarField &,
        const edgeScalarField &,const edgeScalarField &,const edgeScalarField &
    );

    tmp<areaSymmTensorField> makeSymmTensorField(
        const areaScalarField &,const areaScalarField &,const areaScalarField &,
        const areaScalarField &,const areaScalarField &,const areaScalarField &
    );

    tmp<edgeSymmTensorField> makeEdgeSymmTensorField(
        const edgeScalarField &,const edgeScalarField &,const edgeScalarField &,
        const edgeScalarField &,const edgeScalarField &,const edgeScalarField &
    );

    tmp<areaSphericalTensorField> makeSphericalTensorField(const areaScalarField &);

    tmp<edgeSphericalTensorField> makeEdgeSphericalTensorField(const edgeScalarField &);

    // modulo operator
    tmp<areaScalarField> makeModuloField(
        const areaScalarField &a,
        const areaScalarField &b
    ) const;

    // make special fields
    tmp<edgeVectorField> makeEdgePositionField() const;
    tmp<edgeVectorField> makeEdgeProjectionField() const;
    tmp<edgeVectorField> makeEdgeField() const;
    tmp<edgeScalarField> makeLengthField() const;
    tmp<areaVectorField> makePositionField() const;
    tmp<areaScalarField> makeRDistanceField(const areaVectorField &r) const;
    tmp<areaScalarField> makeAreaField() const;
    tmp<areaScalarField> makeRandomField(label seed=0) const;
    tmp<areaScalarField> makeGaussRandomField(label seed=0) const;
    tmp<areaScalarField> makeCellIdField() const;

    //- what type
    const word &typ() const { return typ_; }

    //- override type reporting from common class
    word getResultType() { return typ(); }

    //- is this a logical field
    bool isLogical() const { return isLogical_; }

    //- is this a surface field
    bool isSurfaceField() const { return isSurfaceField_; }

    // Logical expressions
    template <class Field>
    tmp<Field> makeLogicalField(scalar v);

    template <class Op,class Field>
    tmp<Field> doCompare(const Field &a,Op op,const Field &b);

    template <class Op,class Field>
    tmp<Field> doLogicalOp(const Field &a,Op op,const Field &b);

    template <class Field>
    tmp<Field> doLogicalNot(const Field &a);

    template <class T,class Field>
    tmp<T>  doConditional(const Field &l,const T &yes,const T &no);

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
    static void setCalculatedPatches(
        GeometricField<T,faPatchField,areaMesh> &field,
        T unusedValue=pTraits<T>::zero
    );

    template<class T>
    static void setCalculatedPatches(
        GeometricField<T,faePatchField,edgeMesh> &field,
        T value=pTraits<T>::zero
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

    //- return a new plugin-function
    virtual autoPtr<CommonPluginFunction> newPluginFunction(
        const word &name
    );

    //- tests for a plugin-function
    virtual bool existsPluginFunction(
        const word &name
    );

    // allow acces to the setResult-method
    friend class parserFaField::FaFieldValueExpressionParser;
};

} // end namespace

#include "FaFieldValueExpressionDriverI.H"

#endif
