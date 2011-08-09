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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef CommonValueExpressionDriver_H
#define CommonValueExpressionDriver_H

#include "volFields.H"
#include "interpolationTable.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ExpressionResult.H"
#include "StoredExpressionResult.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class CommonValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class CommonValueExpressionDriver
{
    // Private data

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const CommonValueExpressionDriver&);

    //- variables from a dictionary
    stringList variableStrings_;

protected:

    ExpressionResult result_;

    HashTable<ExpressionResult,word> variables_;

    List<StoredExpressionResult> storedVariables_;

    label storedVariablesIndex_;

    wordList globalVariableScopes_;

    const ExpressionResult &lookupGlobal(const word &name) const;

    template<class T>
    bool isGlobal(const word &name,bool isPoint) const;

    void setGlobalScopes(const wordList &);

    HashTable<interpolationTable<scalar>,word> lines_;

    HashTable<interpolationTable<scalar>,word> lookup_;

    Foam::string content_;

    bool trace_scanning_;

    bool trace_parsing_;

    // Handling the scanner 
    virtual void scan_begin()=0;
    virtual void scan_end()=0;

    //- update stuff that needs updating
    virtual bool update();

    //- actual size of the field
    virtual label size() const = 0;

    //- actual size of the supporting point-field
    virtual label pointSize() const = 0;

    //- get the mesh we're living on
    virtual const fvMesh &mesh() const = 0;

    //- get the current time
    virtual const word time() const;

    //- find out which mesh is meant from the dictionary
    static const fvMesh &regionMesh
    (
        const dictionary &dict,
        const fvMesh &mesh
    );

    //- look through the current variable values and update the stored variables
    void updateStoredVariables(bool force=false);

private:
    //- keep fields read from disc in memory
    bool cacheReadFields_;

    //- should we search for the field in the registry before looking on disc
    bool searchInMemory_;

    //- should we look on disc? does not make sense in the application
    bool searchOnDisc_;

    //- should cellSets and faceSets be cached instead of read from disc every time
    static bool cacheSets_;

protected:
   
    inline bool cacheReadFields() const { return cacheReadFields_; }

    inline bool searchInMemory() const { return searchInMemory_; }

    inline bool searchOnDisc() const { return searchOnDisc_; }

    inline bool cacheSets() const { return cacheSets_; }

    string getTypeOfField(const string &name) const;
    string getTypeOfSet(const string &name) const;

    template<class T>
    inline tmp<T> readAndRegister(const word &name);

    enum SetOrigin {
        NEW,
        FILE,
        MEMORY,
        CACHE,
        INVALID
    };

    template<class T>
    inline autoPtr<T> getSet(const fvMesh &,const word &,SetOrigin &origin);

    template<class T>
    inline bool updateSet(autoPtr<T> &theSet,const word &,SetOrigin origin);

    template<class T>
    autoPtr<T> getOrReadField(const string &name,bool failIfNoneFound=true);

    template<class T>
    inline void correctField(T &f) const;
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
        const word& type,
        const word& id,
        const fvMesh& mesh
    );

    // Destructor

    virtual ~CommonValueExpressionDriver();


    // Member Functions

    inline const HashTable<ExpressionResult,word> &variables() const;

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

    void evaluateVariableRemote(const string &remoteExpr,const word &name,const string &expr);

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

//     template<class Type>
//     Field<Type> *getField(const string &name);

    template<class Type>
    bool is(const string &name,bool isPoint=false) const;

    template<class Type>
    bool isVariable(const string &name,bool isPoint=false) const;

    template<class Type>
    bool isField(const string &name,bool isPoint=false) const;

    // checking for variables
    template<class T>
    bool isThere(const string &name) const;

    //- is this a timeline
    inline bool isLine(const string &name) const;

    //- get the value for a timeline for a specific time
    scalarField *getLine(const string &name,scalar t);

    //- value of the timeline
    scalar getLineValue(const string &name,scalar t);

    //- read some interpolation tables
    void readTables(Istream &is,HashTable<interpolationTable<scalar> > &,bool clear=true);

    //- read all the interpolation tables known to man
    void readTables(const dictionary &);

    //- read variables and tables (for objects that were not constructed by a dictionary)
    void readVariablesAndTables(const dictionary &);

    //- write some interpolation tables
    void writeTables(Ostream &os,const HashTable<interpolationTable<scalar> > &) const;

    //- is this a lookup table
    inline bool isLookup(const string &name) const;

    //- get the value for a lookuptable for a set of values
    tmp<scalarField> getLookup(const string &name,const scalarField &val);

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

    template<class Location>
    void error (const Location& l, const std::string& m);
    void error (const std::string& m);

    // Handling the parser
    virtual void parse(const std::string& f)=0;

    // Handling temporary variables
    void clearVariables();

    // Set the trace variables
    void setTrace(
        bool scanning,
        bool parsing
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "CommonValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
