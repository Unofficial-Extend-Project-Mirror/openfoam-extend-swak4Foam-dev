/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    generalInterpreterWrapper

Description
    General wrapper for interpreters

SourceFiles
    generalInterpreterWrapper.C

Contributors/Copyright:
    2011-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef generalInterpreterWrapper_H
#define generalInterpreterWrapper_H

#include "dictionary.H"
#include "objectRegistry.H"
#include "swakTime.H"

#include "runTimeSelectionTables.H"

#include "RawFoamDictionaryParserDriver.H"

#include "CommonValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class CommonValueExpressionDriver;

/*---------------------------------------------------------------------------*\
                           Class generalInterpreterWrapper Declaration
\*---------------------------------------------------------------------------*/


class generalInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    generalInterpreterWrapper(const generalInterpreterWrapper&);

    //- Disallow default bitwise assignment
    void operator=(const generalInterpreterWrapper&);

    // Private data

    word interpreterName_;

    const objectRegistry& obr_;

    const dictionary dict_;

    autoPtr<RawFoamDictionaryParserDriver> parser_;

protected:
    //- should exceptions be tolerated or lead to a fatal error?
    bool tolerateExceptions_;

    //- Should a non-uniform variable result in a warning?
    bool warnOnNonUniform_;

    //- Is this correctly parallelized?
    bool isParallelized_;

    //- Should parallel runs only run on the master
    bool parallelMasterOnly_;

    //- List of global swak namespaces to get values

    //- variables in those namespaces should be inserted into the
    //- Interpreter namespace before running the snipplet
    wordList swakToInterpreterNamespaces_;

    //- Namespace to write values to

    //- this global swak-namespace will get all values (if the user request such)
    word interpreterToSwakNamespace_;

    //- Interpreter variables that swak will receive

    //- the values of these variables will get written to the global swak namespace
    //- only variables that can be either interpreted as a scalar or a vector are
    //- supported
    wordList interpreterToSwakVariables_;

    //- dictionaries in memory that should be read into the interpreter namespacs

    //- key is the name of the dictionary under which it will be
    //- inserted into the interpreter
    //- value is either the name under which it is found in the current region
    //- or a dictionary with region and name
    dictionary dictionariesIntoMemory_;

    //- structures in memory that should update OpenFOAM-dictionaries

    //- key is the name of the struct in memory
    //- value is the specification of the OpenFOAM-dictionary
    dictionary memoryIntoDictionaries_;

    //- should we get an interactive Interpreter-Shell after every code-Execution?

    //- used for debugging the Interpreter-program
    bool interactiveAfterExecute_;

    //- used for debugging of failed Interpreter-program
    bool interactiveAfterException_;

    //- used for debugging of the parser
    int debugLevelParser_;

    //- specification of references
    const List<dictionary> referenceSpec_;

    //- get global variables to the interpreter
    virtual void getGlobals() = 0;

    //- put interpreter variables into the global namespaces
    virtual void setGlobals() = 0;

    void dictionariesToInterpreterStructs();

    void interpreterStructsToDictionaries();

    //- Execute after the interpreter-code has been called
    virtual void doAfterExecution(
        bool fail,
        const string &code,
        bool putVariables,
        bool failOnException=false
    ) = 0;

    //- Check whether we want to continue (parallel)
    bool parallelNoRun(bool doWarning=false);

    //- make sure that no functions are called that are no meant to be called in parallel
    void assertParallel(const word &name);

    //- whether the result has to be broadcast from the master processor
    bool parallelMustBroadcast();

    //- copy the global variables to other processors if only the master has them
    void scatterGlobals();

    //- wait for all processors
    void syncParallel() const;

    // Special initialization for interactive mode
    virtual void initInteractiveSpecial() = 0;

    //- set the state to "my" interpreter
    virtual void setInterpreter() = 0;

    //- release the state to "my" interpreter
    virtual void releaseInterpreter() = 0;


    virtual bool executeCodeInternal(
        const string &code
    ) = 0;

    virtual bool executeCodeCaptureOutputInternal(
        const string &code,
        string &stdout
    ) = 0;

    virtual autoPtr<RawFoamDictionaryParserDriver> getParserInternal(
        RawFoamDictionaryParserDriver::ErrorMode mode
    );

    virtual bool startDictionary();

    virtual bool wrapUpDictionary(const word &name);

    static void checkConflicts();

    //- set up the interpreter and variables
    void setUpEnvironment();

    //- set references to internal fields
    void setReferences();

public:
    class ValidWord {
    public:
        bool operator()(const string &s) {
            for(unsigned int i=0;i<s.size();i++) {
                if(!word::valid(s[i])) {
                    return false;
                }
            }
            return true;
        }
    };

    //- get the parser on demand
    RawFoamDictionaryParserDriver &getParser();

    //- run an interactive session
    virtual void interactiveLoop(const string &banner) = 0;

    void setInteractive(
        bool interactiveAfterExecute,
        bool interactiveAfterException
    );

    //- Execute code. Return true if there was no problem
    bool executeCode(
        const string &code,
        bool putVariables,
        bool failOnException=false
    );

    //- Execute code and capture the stdout. Return true if there was no problem
    virtual bool executeCodeCaptureOutput(
        const string &code,
        string &stdout,
        bool putVariables,
        bool failOnException=false
    );

    //- Execute code and see if it returns to true or false
    virtual bool evaluateCodeTrueOrFalse(
        const string &code,
        bool failOnException=false
    ) = 0;

     //- Execute code and use the return value as a float value
    virtual scalar evaluateCodeScalar(
        const string &code,
        bool failOnException=false
    ) = 0;

      //- Execute code and use the return value as a integer value
    virtual label evaluateCodeLabel(
        const string &code,
        bool failOnException=false
    ) = 0;

    //- Read code according to the dictionary
    void readCode(
        const dictionary &dict,
        const word &prefix,
        string &code,
        bool mustRead=true
    );

    //- shorthand for reading
    string readCode(
        const word &prefix,
        bool mustRead=true
    );

    //- get an entry with code from the data and write it to the stream
    void writeCode(
        const word &prefix,
        Ostream &os,
        bool mustRead=true
    ) const;

    //- transform contents of a OpenFOAM-dictionary and insert into local namespace
    bool insertDictionary(
        const word &name,
        const dictionary &dict
    );

    //- get a data structure and insert it into a dictionary
    virtual bool extractDictionary(
        const word &name,
        dictionary &dict
    );

    // set up the environment with the basic variables
    virtual void initEnvironment(const Time &t) = 0;

    //- set the variable 'runTime' in the interpreter
    virtual void setRunTime(const Time &time) = 0;

    //- Runtime type information
    TypeName("generalInterpreterWrapper");

    const word &interpreterName() const {
        return interpreterName_;
    }

    const word InterpreterName() const;

    const objectRegistry &obr() {
        return obr_;
    }

    const dictionary &dict() {
        return dict_;
    }

    //- write data to reconstruct the settings of this interpreter
    virtual void write(Ostream &os) const;

    //- get variables from an expression driver and put them into the interpreter
    virtual void getVariablesFromDriver(
        CommonValueExpressionDriver &driver,
        const wordList &names
    );

    //- get variables from the interpreter and put them into an expression driver
    virtual void setVariablesInGlobal(
        const word &globalNamespace,
        const wordList &names
    );

    //- Setting a reference to a field under a specific name ... if the interpreter supports it
    virtual void setReference(const word &name,Field<scalar> &value);
    virtual void setReference(const word &name,Field<vector> &value);
    virtual void setReference(const word &name,Field<tensor> &value);
    virtual void setReference(const word &name,Field<symmTensor> &value);
    virtual void setReference(const word &name,Field<sphericalTensor> &value);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    virtual void setReference(const word &name,Field<symmTensor4thOrder> &value);
    virtual void setReference(const word &name,Field<diagTensor> &value);
#endif

    declareRunTimeSelectionTable
    (
        autoPtr,
        generalInterpreterWrapper,
        dictionary,
        (
            const objectRegistry& obr,
            const dictionary& dict,
            bool forceToNamespace
        ),
        (obr,dict,forceToNamespace)
    );

    static autoPtr<generalInterpreterWrapper> New
    (
        const objectRegistry& obr,
        const dictionary& dict,
        bool forceToNamespace=false
    );

    // Constructors

    //- Construct from components
    generalInterpreterWrapper
    (
        const objectRegistry& obr,
        const dictionary&,
        bool forceToNamespace,
        const word interpreterName
    );

    virtual ~generalInterpreterWrapper();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
