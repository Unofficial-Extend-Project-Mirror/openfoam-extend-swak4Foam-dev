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
    python3InterpreterWrapper

Description
    Makes sure that the object receives a python-Interpreter on its own

SourceFiles
    python3InterpreterWrapper.C

Contributors/Copyright:
    2011-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef python3InterpreterWrapper_H
#define python3InterpreterWrapper_H

#include "dictionary.H"
#include "objectRegistry.H"
#include "swakTime.H"
#include "LIFOStack.H"

#include "Python.h"

#if PY_VERSION_HEX<0x03060000
#define OLD_PYTHON3
#endif

#include "generalInterpreterWrapperCRTP.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class Python3FoamDictionaryParserDriver;

/*---------------------------------------------------------------------------*\
                           Class python3InterpreterWrapper Declaration
\*---------------------------------------------------------------------------*/


class python3InterpreterWrapper
:
    public generalInterpreterWrapperCRTP<python3InterpreterWrapper>
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    python3InterpreterWrapper(const python3InterpreterWrapper&);

    //- Disallow default bitwise assignment
    void operator=(const python3InterpreterWrapper&);

    // Private data

    //- state of 'my' Python subinterpreter
    PyThreadState *pythonState_;

    //- remember the state of the gil
    PyGILState_STATE gilState_;

    void getGIL();

    void releaseGIL();

    //- whether or not to use the numpy library
    bool useNumpy_;

    //- whether or not to use IPython as the shell if it is installed
    bool useIPython_;

    //- whether or not we tried IPython
    bool triedIPython_;

    //- embed IPython using the old convention
    bool oldIPython_;

    void getGlobals();

    void getVariablesFromDriver(
        CommonValueExpressionDriver &driver,
        const wordList &names
    );

    void setGlobals();

    void initIPython();

    void initInteractiveSpecial();

    //- stack for nested directories
    LIFOStack<PyObject*> dictionaryStack_;

protected:
    //- how many instances exist (we need a separate interpreter for each of those)
    static label interpreterCount;
    static PyThreadState *mainThreadState;

    //- Execute after the python-code has been called
    void doAfterExecution(
        bool fail,
        const string &code,
        bool putVariables,
        bool failOnException=false
    );

    //- set the state to "my" interpreter
    void setInterpreter();

    //- release the state to "my" interpreter
    void releaseInterpreter();

    //- run an interactive sesssion
    void interactiveLoop(const string &banner);

    //- import a single library
    bool importLib(
        const word &name,
        const word &as="",
        bool useMainThreadStat=false
    );

    bool executeCodeInternal(
        const string &code
    );

    bool executeCodeCaptureOutputInternal(
        const string &code,
        string &stdout
    );

    bool startDictionary();

    bool wrapUpDictionary(const word &name);

    autoPtr<RawFoamDictionaryParserDriver> getParserInternal(
        RawFoamDictionaryParserDriver::ErrorMode mode
    );

    PyObject *currentDictionary();

    void extractDictionaryToDictionary(
        PyObject *pyDict,
        dictionary &dict
    );

    bool useNumpy() {
        return useNumpy_;
    }

    void resultToPythonVariable(
        const word &var,
        const word &namespaceName,
        const ExpressionResult &value
    );

public:

    bool extractDictionary(
        const word &name,
        dictionary &dict
    );

    //- Execute code and return a value
    template <typename T,class Func>
    T evaluateCodeInternal(
        const string &code,
        bool &success
    );

    class toBool {
    public:
        bool operator()(PyObject *&pResult) {
            return PyObject_IsTrue(pResult);
        }
    };

    class toScalar {
    public:
        scalar operator()(PyObject *&pResult) {
            return PyFloat_AsDouble(pResult);
        }
    };

    class toLabel {
    public:
        label operator()(PyObject *&pResult) {
            return PyLong_AsLong(pResult);
        }
    };

    //- set up the environment with the basic variables
    void initEnvironment(const Time &t);

    //- set the variable 'runTime' in the interpreter
    void setRunTime(const Time &time);

    void setReference(const word &name,Field<scalar> &value);
    void setReference(const word &name,Field<vector> &value);
    void setReference(const word &name,Field<tensor> &value);
    void setReference(const word &name,Field<symmTensor> &value);
    void setReference(const word &name,Field<sphericalTensor> &value);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    void setReference(const word &name,Field<diagTensor> &value);
    void setReference(const word &name,Field<symmTensor4thOrder> &value);
#endif

    //- Runtime type information
    TypeName("python3InterpreterWrapper");


    // Constructors

    //- Construct from components
    python3InterpreterWrapper
    (
        const objectRegistry& obr,
        const dictionary&,
        bool forceToNamespace=false
    );

    virtual ~python3InterpreterWrapper();

    friend class Python3FoamDictionaryParserDriver;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
