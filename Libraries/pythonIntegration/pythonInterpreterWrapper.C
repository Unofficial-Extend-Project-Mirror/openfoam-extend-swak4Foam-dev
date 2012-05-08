//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
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

\*---------------------------------------------------------------------------*/

#include "pythonInterpreterWrapper.H"
#include "addToRunTimeSelectionTable.H"

#include "IFstream.H"

#include "GlobalVariablesRepository.H"

#include "vector.H"

#include "stringOps.H"

// #include <fcntl.h>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(pythonInterpreterWrapper, 0);

    label pythonInterpreterWrapper::interpreterCount=0;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

pythonInterpreterWrapper::pythonInterpreterWrapper
(
    const dictionary& dict
):
    tolerateExceptions_(dict.lookupOrDefault<bool>("tolerateExceptions",false)),
    warnOnNonUniform_(dict.lookupOrDefault<bool>("warnOnNonUniform",true)),
    isParallelized_(dict.lookupOrDefault<bool>("isParallelized",false)),
    parallelMasterOnly_(false),
    swakToPythonNamespaces_(
        dict.lookupOrDefault<wordList>(
            "swakToPythonNamespaces",
            wordList(0)
        )
    ),
    pythonToSwakNamespace_(
        dict.lookupOrDefault<word>(
            "pythonToSwakNamespace",
            word("")
        )
    ),
    pythonToSwakVariables_(
        dict.lookupOrDefault<wordList>(
            "pythonToSwakVariables",
            wordList(0)
        )
    ),
    interactiveAfterExecute_(
        dict.lookupOrDefault<bool>("interactiveAfterExecute",false)
    ),
    interactiveAfterException_(
        dict.lookupOrDefault<bool>("interactiveAfterException",false)
    )
{
    if(interpreterCount==0) {
        if(debug) {
            Info << "Initializing Python" << endl;
        }
        Py_Initialize();        
    }

    if(Pstream::parRun()) {
        if(debug) {
            Info << "This is a parallel run" << endl;
        } 
        parallelMasterOnly_=readBool(dict.lookup("parallelMasterOnly"));
    }

    if(parallelNoRun(true)) {
        return;
    }

    if(
        pythonToSwakVariables_.size()>0
        &&
        pythonToSwakNamespace_==""
    ) {
        FatalErrorIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
            << "There are outgoing variables " << pythonToSwakVariables_
                << " defined, but no namespace 'pythonToSwakNamespace'"
                << " to write them to"
                << endl
                << exit(FatalError);
    }

    interpreterCount++;

    pythonState_=Py_NewInterpreter();

    if(debug) {
        Info << "Currently " << interpreterCount 
            << " Python interpreters (created one)" << endl;
    }

    if(
        interactiveAfterExecute_
        ||
        interactiveAfterException_
    ) {
        if(debug) {
            Info << "Preparing interpreter for convenient history editing" << endl;
        }
        PyRun_SimpleString("import rlcompleter, readline");
        // this currently has no effect in the embedded shell
        PyRun_SimpleString("readline.parse_and_bind('tab: complete')"); 
    }
}

void pythonInterpreterWrapper::initEnvironment(const Time &t)
{
    PyObject *m = PyImport_AddModule("__main__");

    PyObject_SetAttrString(m,"caseDir",PyString_FromString(getEnv("FOAM_CASE").c_str()));
    PyObject_SetAttrString(m,"systemDir",PyString_FromString((t.path()/t.caseSystem()).c_str()));
    PyObject_SetAttrString(m,"constantDir",PyString_FromString((t.path()/t.caseConstant()).c_str()));
    PyObject_SetAttrString(m,"meshDir",PyString_FromString((t.path()/t.constant()/"polyMesh").c_str()));
    if(Pstream::parRun()) {
        PyObject_SetAttrString(m,"procDir",PyString_FromString(t.path().c_str()));
    }
    PyObject_SetAttrString(m,"parRun",PyBool_FromLong(Pstream::parRun()));
    PyObject_SetAttrString(m,"myProcNo",PyInt_FromLong(Pstream::myProcNo()));
}

bool pythonInterpreterWrapper::parallelNoRun(bool doWarning)
{
    if(Pstream::parRun()) {
        if(isParallelized_ && doWarning) {
            WarningIn("bool pythonInterpreterWrapper::parallelNoRun()")
                << "The parameter 'isParallelized' was set. This means that the "
                    << "Python code has no adverse side effects in parallel"
                    << endl;
        }
        if(parallelMasterOnly_) {
            return !Pstream::master();
        } else {
            if(!isParallelized_) {
                FatalErrorIn("pythonInterpreterWrapper::parallelNoRun(bool doWarning)")
                    << "This is a parallel run and the Python-snipplets may have"
                        << " adverse side effects. If you do not think so set"
                        << " the 'isParallelized'-flag to true"
                        << endl
                        << exit(FatalError);
            }
            return false;
        }
    } else {
        return false;
    }
}

pythonInterpreterWrapper::~pythonInterpreterWrapper()
{
    if(parallelNoRun()) {
        return;
    }

    PyThreadState_Swap(pythonState_);
    Py_EndInterpreter(pythonState_);
    pythonState_=NULL;

    if(debug) {
        Info << "Currently " << interpreterCount 
            << " Python interpreters (deleting one)" << endl;
    }

    interpreterCount--;
    if(interpreterCount==0) {
        if(debug) {
            Info << "Finalizing Python" << endl;
        }
        PyThreadState_Swap(NULL);

        // This causes a segfault
        //        Py_Finalize();        
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void pythonInterpreterWrapper::setRunTime(const Time &time)
{
    setInterpreter();

    PyObject *m = PyImport_AddModule("__main__");
    PyObject_SetAttrString(m,"runTime",PyFloat_FromDouble(time.value()));
    PyObject_SetAttrString(m,"timeName",PyString_FromString(time.timeName().c_str()));
    PyObject_SetAttrString(m,"outputTime",PyBool_FromLong(time.outputTime()));
    PyObject_SetAttrString(m,"timeDir",PyString_FromString((time.path()/time.timeName()).c_str()));
}

void pythonInterpreterWrapper::setInterpreter()
{
    PyThreadState_Swap(pythonState_);
}

bool pythonInterpreterWrapper::executeCode(const string &code,bool putVariables,bool failOnException)
{
    setInterpreter();

    getGlobals();

    int fail=PyRun_SimpleString(code.c_str());

    doAfterExecution(fail,code,putVariables,failOnException);

    return fail==0;
}

bool pythonInterpreterWrapper::executeCodeCaptureOutput(
    const string &code,
    string &stdout,
    bool putVariables,
    bool failOnException
) {
    setInterpreter();

    getGlobals();

    PyObject *m = PyImport_AddModule("__main__");

    char const* catcherCode = 
"# catcher code\n"
"import sys\n"
"class __StdoutCatcher:\n"
"   def __init__(self):\n"
"      self.data = ''\n"
"   def write(self, stuff):\n"
"      self.data = self.data + stuff\n"
"__catcher = __StdoutCatcher()\n"
"__precatcherStdout = sys.stdout\n"
"sys.stdout = __catcher\n";

    PyRun_SimpleString(catcherCode);

    int fail=PyRun_SimpleString(code.c_str());

    char const* postCatchCode = 
"sys.stdout = __precatcherStdout\n";

    PyRun_SimpleString(postCatchCode);

    doAfterExecution(fail,code,putVariables,failOnException);

    PyObject* catcher = PyObject_GetAttrString(m, "__catcher");
    PyObject* output = PyObject_GetAttrString(catcher, "data");

    stdout=string(PyString_AsString(output));

    return fail==0;
}

bool pythonInterpreterWrapper::evaluateCodeTrueOrFalse(const string &code,bool failOnException)
{
    setInterpreter();

    getGlobals();

    const word funcName("decisionFunction");
    string functionCode="def "+funcName+"():\n";

    std::stringstream ss(code.c_str());
    std::string line;
    while(std::getline(ss, line)) {
        functionCode+="    "+line+"\n";
    }

    if(debug) {
        Info << "Function code:" << endl
            << functionCode;
    }

    PyObject *m = PyImport_AddModule("__main__");
    PyObject *d = PyModule_GetDict(m);

    PyObject *pResult=NULL;
    PyObject *pCode=(Py_CompileString(functionCode.c_str(),"<string from swak>",Py_file_input));

    if( pCode!=NULL && !PyErr_Occurred()) {
        if(debug) {
            Info << "Compiled " << code << endl;
        }
        PyObject *pFunc=PyFunction_New(pCode,d);
        if(debug) {
            Info << "Is function: " << PyFunction_Check(pFunc) << endl;
        }

        PyObject *pTemp=PyObject_CallFunction(pFunc,NULL);
        Py_DECREF(pTemp);

        pResult=PyRun_String((funcName+"()").c_str(),Py_eval_input,d,d);

        Py_DECREF(pFunc);
        Py_DECREF(pCode);
    }
    bool result=false;

    if(pResult!=NULL) {
        if(debug) {
            PyObject *str=PyObject_Str(pResult);
            
            Info << "Result is " << PyString_AsString(str) << endl;

            Py_DECREF(str);
        }
        result=PyObject_IsTrue(pResult);
        if(debug) {
            Info << "Evaluated to " << result << endl;
        }
        Py_DECREF(pResult);
    }

    bool success=(pResult!=NULL && !PyErr_Occurred());
    if(debug) {
        Info << "Success of execution " << success << endl;
    }
    doAfterExecution(!success,code,false,failOnException);
    return result;
}

void pythonInterpreterWrapper::doAfterExecution(bool fail,const string &code,bool putVariables,bool failOnException)
{
    if(fail!=0) {
        Info << "Python Exception" << endl;
        PyErr_Print();
    }

    if(
        interactiveAfterException_
        &&
        fail!=0
    ) {
        Info << "Got an exception for "<< code
            << " now you can interact. Continue with Ctrl-D" << endl;
        PyRun_InteractiveLoop(stdin,"test");
        clearerr(stdin);
    }
    if(
        fail!=0
        &&
        (
            !tolerateExceptions_
            ||
            failOnException
        )
    ) {
        FatalErrorIn("pythonInterpreterWrapper::doAfterExecution")
            << "Python exception raised by " << nl
                << code
                << endl << exit(FatalError);
    }

    if(interactiveAfterExecute_) {
        Info << "Executed "<< code
            << " now you can interact. Continue with Ctrl-D" << endl;
        PyRun_InteractiveLoop(stdin,"test");
        clearerr(stdin);
    }

    if(putVariables) {
        setGlobals();
    }
}

void pythonInterpreterWrapper::getGlobals()
{
    if(swakToPythonNamespaces_.size()==0) {
        return;
    }

    if(debug) {
        Info << "Getting global variables from namespaces " 
            << swakToPythonNamespaces_ << endl;
    }

    PyObject *m = PyImport_AddModule("__main__");

    forAll(swakToPythonNamespaces_,nameI) {
        const GlobalVariablesRepository::ResultTable &vars=
            GlobalVariablesRepository::getGlobalVariables().getNamespace(
               swakToPythonNamespaces_[nameI]
            );
        forAllConstIter(
            GlobalVariablesRepository::ResultTable,
            vars,
            iter
        ) {
            ExpressionResult val=(*iter).getUniform(
                1,
                !warnOnNonUniform_
            );
            const word &var=iter.key();
            if(val.type()==pTraits<scalar>::typeName) {
                PyObject_SetAttrString
                    (
                        m,
                        const_cast<char*>(var.c_str()),
                        PyFloat_FromDouble(
                            val.getResult<scalar>()()[0]
                        )
                    );                
            } else if(val.type()==pTraits<vector>::typeName) {
                const vector v=val.getResult<vector>()()[0];
                PyObject_SetAttrString
                    (
                        m,
                        const_cast<char*>(var.c_str()),
                        Py_BuildValue(
                            "ddd",
                            double(v.x()),
                            double(v.y()),
                            double(v.z())
                        )
                    );
            } else {
                FatalErrorIn("pythonInterpreterWrapper::getGlobals()")
                    << "The variable " << var << " has the unsupported type " 
                        << val.type() << endl
                        << exit(FatalError);
            }
        }        
    } 
}

void pythonInterpreterWrapper::setGlobals()
{
    if(pythonToSwakVariables_.size()==0) {
        return;
    }

    if(debug) {
        Info << "Writing variables " << pythonToSwakVariables_ 
            << " to namespace " << pythonToSwakNamespace_ << endl;
    }

    PyObject *m = PyImport_AddModule("__main__");

    forAll(pythonToSwakVariables_,i) {
        const word &name=pythonToSwakVariables_[i];
        if(debug) {
            Info << "Getting variable "<< name << endl;
        }

        if(
            !PyObject_HasAttrString(
                m,
                const_cast<char *>(name.c_str()))
        ) {
            FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                << "Variable " << name << " not found in Python __main__"
                    << exit(FatalError)
                    << endl;
        }
        PyObject *pVar=PyObject_GetAttrString(
            m,
            const_cast<char *>(name.c_str())
        );

        ExpressionResult eResult;
        
        if(PyNumber_Check(pVar)) {
            if(debug) {
                Info << name << " is a scalar" << endl;
            }
            PyObject *val=PyNumber_Float(pVar);
            scalar result=PyFloat_AsDouble(val);
            Py_DECREF(val);
            if(debug) {
                Info << name << " is " << result << endl;
            }
        
            eResult.setResult(result,1);
                
        } else if(PySequence_Check(pVar)) {
            if(debug) {
                Info << name << " is a sequence" << endl;
            }
            PyObject *tuple=PySequence_Tuple(pVar);
            if(PyTuple_GET_SIZE(tuple)==3) {
                if(debug) {
                    Info << name << " is a vector" << endl;
                }
                vector val;
                bool success=PyArg_ParseTuple(tuple,"ddd",&(val.x()),&(val.y()),&(val.z()));
                if(!success) {
                    FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                        << "Variable " << name << " is not a valid vector"
                            << exit(FatalError)
                            << endl;
                }
                if(debug) {
                    Info << name << " is " << val << endl;
                }

                eResult.setResult(val,1);
            } else {
                FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                    << "Variable " << name << " is a tuple with the unknown size "
                        << PyTuple_GET_SIZE(tuple)
                        << exit(FatalError)
                        << endl;
            }
            Py_DECREF(tuple);
        } else {
            FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                << "Variable " << name << " is of an unknown type"
                    << exit(FatalError)
                    << endl;
        }

        GlobalVariablesRepository::getGlobalVariables().addValue(
            name,
            pythonToSwakNamespace_,
            eResult
        );
    }
}

void pythonInterpreterWrapper::readCode(
    const dictionary &dict,
    const word &prefix,
    string &code
) {
    if(
        dict.found(prefix+"Code")
        &&
        dict.found(prefix+"File")
    ) {
        FatalErrorIn("pythonInterpreterWrapper::readCode")
            << "Either specify " << prefix+"Code" << " or " 
                << prefix+"File" << " but not both" << endl
                << exit(FatalError);
    }
    if(
        !dict.found(prefix+"Code")
        &&
        !dict.found(prefix+"File")
    ) {
        FatalErrorIn("pythonInterpreterWrapper::readCode")
            << "Neither " << prefix+"Code" << " nor " 
                << prefix+"File" << " specified" << endl
                << exit(FatalError);
    }
    if(dict.found(prefix+"Code")) {
        code=string(dict.lookup(prefix+"Code"));
    } else {
        fileName fName(dict.lookup(prefix+"File"));
        fName.expand();
        if(!exists(fName)) {
            FatalErrorIn("pythonInterpreterWrapper::readCode")
                << "Can't find source file " << fName 
                    << endl << exit(FatalError);
        }

        IFstream in(fName);
        code="";
        while(in.good()) {
            char c;
            in.get(c);
            code+=c;
        }
    }
    code=stringOps::trim(code);
}


} // namespace Foam

// ************************************************************************* //
