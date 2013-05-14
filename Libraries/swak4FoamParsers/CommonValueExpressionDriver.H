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
    Foam::CommonValueExpressionDriver

Description

SourceFiles
    CommonValueExpressionDriverI.H
    CommonValueExpressionDriver.C

Contributors/Copyright:
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CommonValueExpressionDriver_H
#define CommonValueExpressionDriver_H

#include "volFields.H"
#include "interpolationTable.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"
#include "StoredExpressionResult.H"
#include "DelayedExpressionResult.H"

#include "MeshesRepository.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class CommonValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class ExpressionDriverWriter;

class initSwakFunctionObject;

class CommonPluginFunction;

class CommonValueExpressionDriver
{
    // Private data

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const CommonValueExpressionDriver&);

    //- variables from a dictionary
    stringList variableStrings_;

    //- this one is in charge for writing and restoring

    //- by using it this class doesn't have to inherit from regIOobject
    //- (which would be an overkill and inconvenient)
    autoPtr<ExpressionDriverWriter> writer_;

    //- pointer to the "default" mesh
    static const fvMesh *defaultMeshPtr_;

    //- getting our own pointMesh
    autoPtr<pointMesh> pMesh_;

    //- the position to which the parser progressed
    label parserLastPos_;

    //- string that allows more detailed allocation of the context
    string contextString_;

    //- string that stores the name of the other mesh (if that is going to be used)
    word otherMeshName_;

    //- list with aliases. Meant to workaround situations where fields have names that don't fit what swak accepts as a field name
    HashTable<word,word> aliases_;

protected:
    //- get the string of the context in a form that is usable for an error message
    std::string getContextString();

    const string contextString() const { return contextString_; }

    //- little helper method to print hex adresses
    word getHex(const void *) const;

    //- access last position
    const label &parserLastPos() const { return parserLastPos_; }

    //- access last position
    label &parserLastPos() { return parserLastPos_; }

    //- get the result from another driver. Overrideable to allow mapping
    virtual autoPtr<ExpressionResult> getRemoteResult(
        CommonValueExpressionDriver &otherDriver
    );

    //- is there an alias with that name
    bool hasAlias(const word &name) const;

    //- return the actual name for an alias
    word getAlias(const word &name) const;

    const HashTable<word,word> &aliases() const
        {
            return aliases_;
        }

    void setAliases(const HashTable<word,word> &orig) {
        aliases_=orig;
    }
private:
    ExpressionResult result_;

    HashTable<ExpressionResult,word> variables_;

    List<StoredExpressionResult> storedVariables_;

    HashTable<DelayedExpressionResult,word> delayedVariables_;

    label specialVariablesIndex_;

    wordList globalVariableScopes_;

    HashTable<interpolationTable<scalar>,word> lines_;

    HashTable<interpolationTable<scalar>,word> lookup_;

    Foam::string content_;

    bool trace_scanning_;

    bool trace_parsing_;

    bool variableNameIdenticalToField_;

protected:
    const ExpressionResult &lookupGlobal(const word &name) const;

    template<class T>
    bool isGlobal(const word &name,bool isPoint,label expectedSize=-1) const;

    void setGlobalScopes(const wordList &);

    //- scanner state for reentrant lexer
    void *scanner_;

    // Handling the scanner
    virtual void scan_begin()=0;
    virtual void scan_end()=0;

    //- update stuff that needs updating
    virtual bool update();

    //- weights of the entries if it is not a point field

    //- parametert size is the expected size of the weight field and
    //- will be used to determine whether this is correct.
    virtual tmp<scalarField> weightsNonPoint(
        label size
    ) const = 0;

public:

    ExpressionResult &result() { return result_; }

    const ExpressionResult &result() const { return result_; }

    const string &content() const { return content_; }

    //- actual size of the field
    virtual label size() const = 0;

    //- actual size of the supporting point-field
    virtual label pointSize() const = 0;

    //- weights of the entries

    //- parametert size is the expected size of the weight field and
    //- will be used to determine whether this is correct.
    //- point determines whether this is a point field or not
    tmp<scalarField> weights(
        label size,
        bool point=false
    ) const;

   //- access the other mesh name
    const word &otherMeshName() const { return otherMeshName_; }
    word &otherMeshName() { return otherMeshName_; }

protected:
    //- get the mesh we're living on
    virtual const fvMesh &mesh() const = 0;

    //- get a point mesh (if needed)
    inline const pointMesh & pMesh();

    //- get the current time
    virtual const word time() const;

    //- set the default Mesh (if it is unset)
    static bool setUnsetDefaultMesh(const fvMesh &);

    //- set the default Mesh
    static bool resetDefaultMesh(const fvMesh &);

    //- get the default Mesh (if one is defined)
    static const fvMesh &getDefaultMesh();

    //- find out which mesh is meant from the dictionary
    static const fvMesh &regionMesh
    (
        const dictionary &dict,
        const fvMesh &mesh,
        bool readIfNecessary
    );

    //- look through the current variable values and update the stored variables
    void updateSpecialVariables(bool force=false);

    //- read data from the writer
    virtual void getData(const dictionary &);

    //- write data
    virtual void prepareData(dictionary &) const;

    //- do we need a datafile written
    virtual bool hasDataToWrite() const;

private:
    //- keep fields read from disc in memory
    bool cacheReadFields_;

    //- should we search for the field in the registry before looking on disc
    bool searchInMemory_;

    //- should we look on disc? does not make sense in the application
    bool searchOnDisc_;

    //- should cellSets and faceSets be cached instead of read from disc every time
    static bool cacheSets_;

    //- use the value of the previous iteration if the old time ist asked for
    bool prevIterIsOldTime_;

protected:

    inline bool cacheReadFields() const { return cacheReadFields_; }

    inline bool searchInMemory() const { return searchInMemory_; }

    inline bool searchOnDisc() const { return searchOnDisc_; }

    inline bool cacheSets() const { return cacheSets_; }

    inline bool prevIterIsOldTime() const { return prevIterIsOldTime_; }

    word getTypeOfFieldInternal(
        const fvMesh &mesh,
        const word &name
    ) const;

    word getTypeOfField(
        const word &name
    ) const;

    word getTypeOfSet(
        const word &name
    ) const;

    template<class T,class Mesh>
    inline tmp<T> readAndRegister(
        const word &name,
        const Mesh &actualMesh
    );

    enum SetOrigin {
        NEW,
        FILE,
        MEMORY,
        CACHE,
        INVALID
    };

    template<class T>
    inline autoPtr<T> getSet(
        const fvMesh &,
        const word &,
        SetOrigin &origin
    ) const;

    template<class T>
    inline bool updateSet(
        autoPtr<T> &theSet,
        const word &,
        SetOrigin origin
    ) const;

    template<class T>
    autoPtr<T> getOrReadField(
        const word &name,
        bool failIfNoneFound=true,
        bool getOldTime=false
    );

    template<class T>
    autoPtr<T> getOrReadPointField(
        const word &name,
        bool failIfNoneFound=true,
        bool getOldTime=false
    );

    template<class T,class Mesh>
    autoPtr<T> getOrReadFieldInternal(
        const word &name,
        const Mesh &mesh,
        bool failIfNoneFound=true,
        bool getOldTime=false
    );

    template<class T>
    inline void correctField(T &f) const;

    typedef HashTable<int> StartupSymbols;

    template<class T>
    class SymbolTable
    :
    public StartupSymbols
    {
    public:
        SymbolTable();
    };

    virtual int startupSymbol(const word &name) = 0;

    virtual void parseInternal(int startSymbol) = 0;

    //- only subclasses have write-access
    //    inline  HashTable<ExpressionResult,word> &variables();
    ExpressionResult &variable(const word &name);

public:
    //- Runtime type information
    TypeName("CommonValueExpressionDriver");

    // Declare run-time constructor selection table

    declareRunTimeSelectionTable
    (
        autoPtr,
        CommonValueExpressionDriver,
        dictionary,
        (
            const dictionary& dict,
            const fvMesh& mesh
        ),
        (dict,mesh)
    );

    declareRunTimeSelectionTable
    (
        autoPtr,
        CommonValueExpressionDriver,
        idName,
        (
            const word& id,
            const fvMesh& mesh
        ),
        (id,mesh)
    );

    // Constructors

    //- Construct from components
    CommonValueExpressionDriver(
        bool cacheReadFields=false,
        bool searchInMemory=true,
        bool searchOnDisc=false
    );

    //- Copy construct
    CommonValueExpressionDriver(
        const CommonValueExpressionDriver&
    );

    virtual autoPtr<CommonValueExpressionDriver> clone() const = 0;

    //- Construct from a dictionary
    CommonValueExpressionDriver(const dictionary&);

    //- Return a reference to the selected value driver
    static autoPtr<CommonValueExpressionDriver> New
    (
        const dictionary& dict,
        const fvMesh& mesh
    );

    //- Return a reference to the selected value driver
    static autoPtr<CommonValueExpressionDriver> New
    (
        const dictionary& dict
    );

    //- Return a reference to the selected value driver
    static autoPtr<CommonValueExpressionDriver> New
    (
        const word& type,
        const word& id,
        const fvMesh& mesh
    );

    // Destructor

    virtual ~CommonValueExpressionDriver();


    // Member Functions

    //    inline const HashTable<ExpressionResult,word> &variables() const;
    const ExpressionResult &variable(const word &name) const;

    bool hasVariable(const word &name) const;

    void setSearchBehaviour(
        bool cacheReadFields,
        bool searchInMemory,
        bool searchOnDisc
    );

    template<class Type>
    tmp<Field<Type> > evaluate(const string &expr,bool qPoint=false);

    template<class Type>
    Type evaluateUniform(const string &expr,bool qPoint=false);

    void evaluateVariable(const word &name,const string &expr);

    void evaluateVariableRemote(
        const string &remoteExpr,
        const word &name,
        const string &expr
    );

    //- allows adding uniform variables from outside
    template<class T>
    void addUniformVariable(
        const word &name,
        const T &val
    );

    //- calculate the weighted average of a field
    template<class Type>
    Type calcWeightedAverage(const Field<Type> &f) const;

    //- read information about the foreign meshes and create them
    static label readForeignMeshInfo(const dictionary &dict,bool verbose=true);

   //- read the list of variable strings (or initialize with a single string)
    static stringList readVariableStrings(const dictionary &dict);

    //- read the list of variable strings (or initialize with a single string)
    void setVariableStrings(const dictionary &dict);

    Ostream &writeVariableStrings(Ostream &out) const;

    Ostream &writeCommon(Ostream &out,bool debug=false) const;

    void addVariables(const string &exprList,bool clear=true);

    void addVariables(const stringList &exprList,bool clear=true);

    template<class Type>
    void setResult(Field<Type> *,bool isPoint=false);

    template<class Type>
    tmp<Field<Type> > getResult(bool qPoint=false);

    void outputResult(Ostream &);

    string outputEntry();

    virtual word getResultType();

    ExpressionResult getUniform(label size,bool noWarning);

    template<template<class> class BinOp,class Type>
    inline Type getReduced(BinOp<Type> op,Type initial=pTraits<Type>::zero);

    void clearResult();

    template <class Op,class Type>
    tmp<Field<bool> > doCompare(
        const Field<Type> &a,
        Op op,
        const Field<Type> &b
    );

    template <class Op>
    tmp<Field<bool> > doLogicalOp(
        const Field<bool> &a,
        Op op,
        const Field<bool> &b
    );

    inline tmp<Field<bool> > doLogicalNot(const Field<bool> &a);

    template<class Type>
    tmp<Field<Type> > doConditional(
        const Field<bool> &d,
        const Field<Type> &yes,
        const Field<Type> &no
    );

    template<class Type>
    tmp<Field<Type> > makeField(Type,bool isPoint=false);

//     template<class Type>
//     tmp<Field<Type> > getField(const string &name);

    template<class Type>
    bool is(
        const word &name,
        bool isPoint=false
    ) const;

    template<class Type>
    bool isVariable(
        const word &name,
        bool isPoint=false,
        label expectSize=-1
    ) const;

    template<class Type>
    bool isField(
        const word &name,
        bool isPoint=false
    ) const;

    // checking for variables
    template<class T>
    bool isThere(
        const word &name
    ) const;

    //- is this a timeline
    inline bool isLine(
        const word &name
    ) const;

    //- get the value for a timeline for a specific time
    tmp<scalarField> getLine(
        const word &name,
        scalar t
    );

    //- value of the timeline
    scalar getLineValue(
        const word &name,
        scalar t
    );

    //- read some interpolation tables
    void readTables(
        Istream &is,
        HashTable<interpolationTable<scalar> > &,
        bool clear=true
    );

    //- is this mesh in the repository
    bool isForeignMesh(
        const word &name
    ) const;

    template<class T>
    bool isForeignField(
        const word &meshName,
        const word &fieldName
    ) const;

    template<class T>
    tmp<T> interpolateForeignField(
        const word &meshName,
        const word &fieldName,
        meshToMesh::order theOrder=meshToMesh::INTERPOLATE
    ) const;

    //- read all the interpolation tables known to man
    void readTables(const dictionary &);

    //- read variables and tables (for objects that were not constructed by a dictionary)
    virtual void readVariablesAndTables(const dictionary &);

    //- write some interpolation tables
    void writeTables(
        Ostream &os,
        const HashTable<interpolationTable<scalar> > &
    ) const;

    //- is this a lookup table
    inline bool isLookup(
        const word &name
    ) const;

    //- get the value for a lookuptable for a set of values
    tmp<scalarField> getLookup(
        const word &name,
        const scalarField &val
    );

    tmp<scalarField> makeModuloField(
        const scalarField &a,
        const scalarField &b
    ) const;
    tmp<scalarField> makeRandomField(label seed=0) const;
    tmp<scalarField> makeGaussRandomField(label seed=0) const;

    vector getPositionOfMinimum(const scalarField &,const vectorField &) const;

    vector getPositionOfMaximum(const scalarField &,const vectorField &) const;

    const Time& runTime() const;

    tmp<vectorField> composeVectorField(
        const scalarField &,
        const scalarField &,
        const scalarField &
    );

    tmp<tensorField> composeTensorField(
        const scalarField &,const scalarField &,const scalarField &,
        const scalarField &,const scalarField &,const scalarField &,
        const scalarField &,const scalarField &,const scalarField &
    );

    tmp<symmTensorField> composeSymmTensorField(
        const scalarField &,const scalarField &,const scalarField &,
        const scalarField &,const scalarField &,const scalarField &
    );

    tmp<sphericalTensorField> composeSphericalTensorField(const scalarField &);

    // Error handling.

    template<class Location>
    void error (const Location& l, const std::string& m);
    void error (const std::string& m);

    //- Handling the parser
    label parse(const std::string& f,const word &start="");

    // Handling temporary variables
    void clearVariables();

    // Set the trace variables
    void setTrace(
        bool scanning,
        bool parsing
    );

    bool traceScanning() const { return trace_scanning_; }

    bool traceParsing() const { return trace_parsing_; }

    //- create a writer for this object
    void createWriterAndRead(const word &name);

    //- Write data if apropriate
    //- Should enable exact restarts
    void tryWrite() const;

    //- return a new plugin-function
    virtual autoPtr<CommonPluginFunction> newPluginFunction(
        const word &name
    ) = 0;

    //- tests for a plugin-function
    virtual bool existsPluginFunction(
        const word &name
    ) = 0;

    //- append a string to the conext for error messages
    void appendToContext(const string &value);

    friend class ExpressionDriverWriter;

    friend class initSwakFunctionObject;

    friend class CommonPluginFunction;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "CommonValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
