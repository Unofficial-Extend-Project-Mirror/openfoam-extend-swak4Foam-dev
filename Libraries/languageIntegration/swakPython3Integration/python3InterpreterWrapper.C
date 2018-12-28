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

Contributors/Copyright:
    2011-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "python3InterpreterWrapper.H"
#include "Python3FoamDictionaryParserDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "IFstream.H"

#include "GlobalVariablesRepository.H"

#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#ifdef FOAM_HAS_STRINGOPS
#include "stringOps.H"
#endif

#include "addToRunTimeSelectionTable.H"

#include "python3HelperTemplates.H"

// #include <fcntl.h>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(python3InterpreterWrapper, 0);
    addNamedToRunTimeSelectionTable(generalInterpreterWrapper,python3InterpreterWrapper,dictionary,python3);

    label python3InterpreterWrapper::interpreterCount=0;
    PyThreadState *python3InterpreterWrapper::mainThreadState=NULL;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

void python3InterpreterWrapper::initInteractiveSpecial() {
    initIPython();
}

void python3InterpreterWrapper::initIPython() {
    Dbug << "Initializing IPython" << endl;
    if(!triedIPython_) {
        Dbug << "For real" << endl;
        triedIPython_=true;

        if(useIPython_) {
    	    importLib("sys");
	    int result=PyRun_SimpleString("if 'argv' not in dir(sys): sys.argv=['OF-executable']");
            Pbug << "no argv result " << result << endl;

            Dbug << "Attempting to import IPython" << endl;

            //            int fail=PyRun_SimpleString("import IPython");
            bool fail=!importLib("IPython");
            if(fail) {
                WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
                    << "Importing of IPython failed. Falling back to regular shell"
                        << " for " << dict().name()
                        << endl;
                useIPython_=false;
            } else {
                PyObject *ipython=PyImport_AddModule("IPython");
                if(ipython==NULL) {
                    WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
                        << "Can't find IPython-module. Switching IPython off"
                            << " for " << dict().name()
                            << endl;
                    useIPython_=false;
                } else {
                    if(
                        PyObject_HasAttrString(
                            ipython,
                            "embed"
                        )
                    ) {
                        Dbug << "New style IPython embedding" << endl;
                    } else if(
                        PyObject_HasAttrString(
                            ipython,
                            "Shell"
                        )
                    ) {
                        oldIPython_=true;
                        WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
                            << "Old style IPython embedding"
                                << " for " << dict().name()
                                << endl;
                    } else {
                        useIPython_=false;
                        WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
                            << "Did not find a known way of embedding IPython. Using normal shell"
                                << " for " << dict().name()
                                << endl;

                    }
                }
            }
        }
        if(!useIPython_) {
            Dbug << "Preparing interpreter for convenient history editing" << endl;

            //            PyRun_SimpleString("import rlcompleter, readline");
            importLib(
                "rlcompleter"
#ifndef OLD_PYTHON3
                ,"rlcompleter",true
#endif
            );
            importLib(
                "readline"
#ifndef OLD_PYTHON3
                ,"readline",true
#endif
            );

            // this currently has no effect in the embedded shell
            int result=PyRun_SimpleString("readline.parse_and_bind('tab: complete')");
            Pbug << "Result readline " << result << endl;
        }
    }
}

python3InterpreterWrapper::python3InterpreterWrapper
(
    const objectRegistry& obr,
    const dictionary& dict,
    bool forceToNamespace
):
    generalInterpreterWrapperCRTP<python3InterpreterWrapper>(
        obr,
        dict,
        forceToNamespace,
        "python3"
    ),
    pythonState_(NULL),
    useNumpy_(dict.lookupOrDefault<bool>("useNumpy",true)),
    useIPython_(dict.lookupOrDefault<bool>("useIPython",true)),
    triedIPython_(false),
    oldIPython_(false)
{
    //    Py_Initialize();
    if(generalInterpreterWrapper::debug>debug) {
        debug=1;
    }
    //    Pbug << "Version: " << Py_GetVersion() << endl;

    Pbug << "Starting constructor: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;

    syncParallel();

#ifdef FOAM_HAS_LOCAL_DEBUGSWITCHES
    debug=dict.lookupOrDefault<label>("debugPythonWrapper",debug());
#else
    debug=dict.lookupOrDefault<label>("debugPythonWrapper",debug);
#endif

    if(!dict.found("useNumpy")) {
        WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
            << "Switch 'useNumpy' not found in " << dict.name() << nl
                << "Assuming it to be 'true' (if that is not what you want "
                << "set it. Also set it to make this warning go away)"
                << endl;
    }

    if(!dict.found("useIPython")) {
        WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
            << "Switch 'useIPython' not found in " << dict.name() << nl
                << "Assuming it to be 'true' (if that is not what you want "
                << "set it. Also set it to make this warning go away)"
                << endl;
    }

    if(interpreterCount==0) {
        Pbug << "Initializing Python" << endl;

        if(Py_IsInitialized()) {
            FatalErrorIn("python3InterpreterWrapper::python3InterpreterWrapper")
                << "Already initialized. Something is wrong"
                    << endl
                    << exit(FatalError);
        }
        Py_Initialize();
        //        Py_InitializeEx(0);

#ifdef OLD_PYTHON3
        if(debug) {
            PyThreadState *current=PyGILState_GetThisThreadState();
            Pbug << "GIL-state before thread: " << getHex(current) << endl;
        }
        PyEval_InitThreads();

        if(debug) {
            PyThreadState *current=PyGILState_GetThisThreadState();
            Pbug << "GIL-state after thread: " << getHex(current) << endl;
        }
#endif
        Pbug << "Before saving state: We have the GIL: " << PyGILState_Check()
            << " our state " << getHex(pythonState_) << " Main: "
            << getHex(mainThreadState) << " used: "
            << getHex(PyGILState_GetThisThreadState()) << endl;
#ifdef OLD_PYTHON3
        mainThreadState = PyEval_SaveThread();
#else
        mainThreadState = PyThreadState_Get();
#endif
         Pbug << "After saving state: We have the GIL: " << PyGILState_Check()
            << " our state " << getHex(pythonState_) << " Main: "
            << getHex(mainThreadState) << " used: "
            << getHex(PyGILState_GetThisThreadState()) << endl;
    }

    if(Pstream::parRun()) {
        Pbug << "This is a parallel run" << endl;

        parallelMasterOnly_=readBool(dict.lookup("parallelMasterOnly"));
    }

    if(parallelNoRun(true)) {
        Pbug << "Getting out because of 'parallelNoRun'" << endl;
        return;
    }

    interpreterCount++;

#ifdef OLD_PYTHON3
    getGIL();
#endif
    //    PyEval_AcquireLock();

    Pbug << "Getting new interpreter: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState())  << endl;
    pythonState_=Py_NewInterpreter();
    //    PyThreadState_Swap(pythonState_);
    Pbug << "Interpreter state: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState())  << endl;

    //    interactiveLoop("Clean");

    int unflush=PyRun_SimpleString(
        "class __unbufferedFile(object):\n"
        "    def __init__(self, stream):\n"
        "        self.stream = stream\n"
        "    def write(self, data):\n"
        "        self.stream.write(data)\n"
        "        self.stream.flush()\n"
        "    def writelines(self, datas):\n"
        "        self.stream.writelines(datas)\n"
        "        self.stream.flush()\n"
        "    def __getattr__(self, attr):\n"
        "        return getattr(self.stream, attr)\n"
        "import sys\n"
        "sys.stderr = __unbufferedFile(sys.stderr)\n"
        "sys.stdout = __unbufferedFile(sys.stdout)\n");
    Pbug << "Unflush " << unflush << endl;

    initIPython();

    Pbug << "Currently " << interpreterCount
        << " Python interpreters (created one)" << endl;

    if(
        interactiveAfterExecute_
        ||
        interactiveAfterException_
    ) {
    } else {
        if(useIPython_) {
            WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
                << "'useIPython' not needed in " << dict.name()
                    << " if there is no interactivity"
                    << endl;
        }
    }

    if(useNumpy_) {
        Dbug << "Attempting to import numpy" << endl;

        static bool warnedNumpy=false;
        if(!warnedNumpy) {
            if(getEnv("FOAM_SIGFPE")!="false") {
                WarningInFunction
                    << "Attempting to import numpy. On some platforms that will raise a "
                        << "(harmless) floating point exception. To avoid switch off "
                        << "by setting the environment variable 'FOAM_SIGFPE' to 'false'"
                        << endl;
            }
            warnedNumpy=true;
        }

        int fail=!importLib("numpy");
        if(fail) {
            FatalErrorIn("python3InterpreterWrapper::python3InterpreterWrapper")
                << "Problem during import of numpy." << nl
                    << "Switch if off with 'useNumpy false;' if it is not needed"
                    << endl
                    << exit(FatalError);
        }
        fail=PyRun_SimpleString(
            // "def _swak_wrapOpenFOAMField_intoNumpy(address,typestr,size,nr=None):\n"
            // "   class iWrap(object):\n"
            // "      def __init__(self):\n"
            // "         self.__array_interface__={}\n"
            // "         self.__array_interface__['data']=(int(address,16),False)\n"
            // "         if nr:\n"
            // "             self.__array_interface__['shape']=(size,nr)\n"
            // "         else:\n"
            // "             self.__array_interface__['shape']=(size,)\n"
            // "         self.__array_interface__['version']=3\n"
            // "         self.__array_interface__['typestr']=typestr\n"
            // "   return numpy.asarray(iWrap())\n"
            "class OpenFOAMFieldArray(numpy.ndarray):\n"
            "   def __new__(cls,address,typestr,size,nr=None,names=None):\n"
            "      obj=type('Temporary',(object,),{})\n"
            "      obj.__array_interface__={}\n"
            "      obj.__array_interface__['data']=(address,False)\n"
            "      if nr:\n"
            "          obj.__array_interface__['shape']=(size,nr)\n"
            "      else:\n"
            "          obj.__array_interface__['shape']=(size,)\n"
            //            "      obj.__array_interface__['descr']=[('x',typestr)]\n"
            "      obj.__array_interface__['version']=3\n"
            "      obj.__array_interface__['typestr']=typestr\n"
            "      obj=numpy.asarray(obj).view(cls)\n"
            "      if names:\n"
            "         for i,n in enumerate(names):\n"
            "            def f(ind):\n"
            "               return obj[:,ind]\n"
            "            setattr(obj,n,f(i))\n"
            "      return obj\n"
            "   def __array_finalize__(self,obj):\n"
            "      if obj is None: return\n"
        );
        Pbug << "OpenFOAMFieldArray " << fail << endl;
    }

    if(dict.found("importLibs")) {
        const dictionary &libList=dict.subDict("importLibs");
        forAllConstIter(dictionary,libList,iter) {
            word as=(*iter).keyword();
            word full((*iter).stream());
            if(full=="") {
                full=as;
            }
            importLib(full,as,true);
        }
    } else {
        WarningIn("python3InterpreterWrapper::python3InterpreterWrapper")
            << "No dictionary 'importLibs' found in " << dict.name()
                << endl << "If code hangs during importing try importing "
            "the libraries from here first"
                << endl;
    }

    //    releaseGIL();

#ifdef OLD_PYTHON3
    PyEval_ReleaseThread(pythonState_);
#endif

    //    releaseGIL();

    Pbug << "End constructor: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;
        //        << getHex(PyThreadState_Get()) << endl;
}

void python3InterpreterWrapper::getGIL() {
    Pbug << "Getting GIL: We have the GIL: " << PyGILState_Check() << endl;

    gilState_=PyGILState_Ensure();

    Pbug << "Getting GIL - post: We have the GIL: " << PyGILState_Check() << endl;
}

void python3InterpreterWrapper::releaseGIL() {
    Pbug << "Releasing GIL: We have the GIL: " << PyGILState_Check() << endl;

    PyGILState_Release(gilState_);

    Pbug << "Releasing GIL - post:We have the GIL: " << PyGILState_Check() << endl;
}


void python3InterpreterWrapper::initEnvironment(const Time &t)
{
    assertParallel("initEnvironment");

    Pbug << "initEnvironment" << endl;

    setInterpreter();

    PyObject *m = PyImport_AddModule("__main__");

    PyObject_SetAttrString(m,"functionObjectName",PyUnicode_FromString("notAFunctionObject"));
    PyObject_SetAttrString(m,"caseDir",PyUnicode_FromString(getEnv("FOAM_CASE").c_str()));
    PyObject_SetAttrString(m,"systemDir",PyUnicode_FromString((t.path()/t.caseSystem()).c_str()));
    PyObject_SetAttrString(m,"constantDir",PyUnicode_FromString((t.path()/t.caseConstant()).c_str()));
    PyObject_SetAttrString(m,"meshDir",PyUnicode_FromString((t.path()/t.constant()/"polyMesh").c_str()));
    if(Pstream::parRun()) {
        PyObject_SetAttrString(m,"procDir",PyUnicode_FromString(t.path().c_str()));
    }
    PyObject_SetAttrString(m,"parRun",PyBool_FromLong(Pstream::parRun()));
    PyObject_SetAttrString(m,"myProcNo",PyLong_FromLong(Pstream::myProcNo()));

    int fail=PyRun_SimpleString(
        "def makeDataDir(d):\n"
        "    import os\n"
        "    if not os.path.exists(d):\n"
        "        try:\n"
        "            os.makedirs(d)\n"
        "        except OSError:\n"
        "            if not os.path.exists(d):\n"
        "                import sys\n"
        "                raise sys.exc_info()[1]\n"
        "            else:\n"
        "                print('Possible race condition while creating',d)\n"

        "def timeDataFile(name):\n"
        "    import os\n"
        "    d=os.path.join(timeDir,functionObjectName+'_data')\n"
        "    makeDataDir(d)\n"
        "    return os.path.join(d,name)\n"

        "def dataFile(name):\n"
        "    import os\n"
        "    d=os.path.join(caseDir,'postProcessing',functionObjectName+'_data',timeName)\n"
        "    makeDataDir(d)\n"
        "    return os.path.join(d,name)\n"
    );
    Pbug << "makeDataDir: " << fail << endl;

    releaseInterpreter();

    Pbug << "initEnvironment - end" << endl;
}

python3InterpreterWrapper::~python3InterpreterWrapper()
{
    Pbug << "Starting destructor: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;

    if(parallelNoRun()) {
        Pbug << "Leaving early" << endl;
        return;
    }

    Dbug << "Currently " << interpreterCount
        << " Python interpreters (deleting one)" << endl;

    Pbug << "State: " << getHex(pythonState_) << endl;
    if(pythonState_) {
#ifdef OLD_PYTHON3
        getGIL();
#endif
        PyThreadState_Swap(pythonState_);
        Pbug << "Ending the interpreter" << endl;
        Py_EndInterpreter(pythonState_);
#ifdef OLD_PYTHON3
        PyEval_ReleaseLock();
#endif
        pythonState_=NULL;
    }

    interpreterCount--;
    if(interpreterCount==0) {
        Dbug << "Finalizing Python" << endl;

#ifdef OLD_PYTHON3
        PyEval_RestoreThread(mainThreadState);
        Dbug << "Actual finalizing" << endl;
#endif
        // This causes a segfault
        Py_Finalize();
        mainThreadState=NULL;
    }
    Pbug << "Ending destructor: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void python3InterpreterWrapper::setRunTime(const Time &time)
{
    assertParallel("setRunTime");

    Pbug << "setRunTime " << time.timeName() << endl;

    setInterpreter();

    PyObject *m = PyImport_AddModule("__main__");
    PyObject_SetAttrString(m,"deltaT",PyFloat_FromDouble(time.deltaT().value()));
    PyObject_SetAttrString(m,"deltaT0",PyFloat_FromDouble(time.deltaT0().value()));
    PyObject_SetAttrString(m,"endTime",PyFloat_FromDouble(time.endTime().value()));
    PyObject_SetAttrString(m,"runTime",PyFloat_FromDouble(time.value()));
    PyObject_SetAttrString(m,"timeName",PyUnicode_FromString(time.timeName().c_str()));
    PyObject_SetAttrString(m,"outputTime",PyBool_FromLong(time.outputTime()));
    PyObject_SetAttrString(m,"timeDir",PyUnicode_FromString((time.path()/time.timeName()).c_str()));
    PyObject_SetAttrString(m,"timeIndex",PyLong_FromLong(time.timeIndex()));

    releaseInterpreter();
}

void python3InterpreterWrapper::setInterpreter()
{
    assertParallel("setInterpreter");

    Pbug << "setInterpreter: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;

    if(debug) {
        PyThreadState *current=PyGILState_GetThisThreadState();
        Pbug << "Current GIL-state " << getHex(current) << endl;
    }

    if(pythonState_) {
        Pbug << "Setting state to " << getHex(pythonState_) << endl;
        PyThreadState *old=PyGILState_GetThisThreadState();
        Pbug << "Old state: " << getHex(old) << endl;
        if(old==NULL) {
            Pbug << "No current thread state" << endl;
            PyEval_RestoreThread(pythonState_);
        } else if(
            old==mainThreadState
            &&
            PyGILState_Check()==0
        ) {
            Pbug << "Main thread state" << endl;
            PyEval_RestoreThread(pythonState_);
        } else {
            Pbug << "Swapping out current" << endl;
            PyThreadState *prev=PyThreadState_Swap(pythonState_);
            Pbug << "Swapped state: " << getHex(prev) << endl;
        }
        //        PyThreadState *old=PyThreadState_Swap(mainThreadState);
    }
    // getGIL();
    Pbug << "setInterpreter - post: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;
}

void python3InterpreterWrapper::releaseInterpreter()
{
    assertParallel("releaseInterpreter");

    Pbug << "releaseInterpreter: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;
    if(
#ifdef OLD_PYTHON3
        true
#else
        PyGILState_GetThisThreadState()
        ==
        pythonState_
#endif
    ) {
        Pbug << "Releasing thread" << endl;
        PyEval_ReleaseThread(pythonState_);
    } else {
        Pbug << "We're not the current thread. Not releasing" << endl;
    }
    //     PyEval_ReleaseThread(mainThreadState);
    Pbug << "releaseInterpreter post: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState()) << endl;
}

bool python3InterpreterWrapper::executeCodeInternal(
    const string &code
) {
    Pbug << "executeCodeInternal: " << code << endl;
    Pbug << "executeCodeInternal: We have the GIL: " << PyGILState_Check()
        << " our state " << getHex(pythonState_) << " Main: "
        << getHex(mainThreadState) << " used: "
        << getHex(PyGILState_GetThisThreadState())  << endl;

    return PyRun_SimpleString(code.c_str())==0;
}

bool python3InterpreterWrapper::executeCodeCaptureOutputInternal(
    const string &code,
    string &stdout
) {
    Pbug << "executeCodeCaptureOutputInternal: " << code << endl;

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

    int result=PyRun_SimpleString(catcherCode);
    Pbug << "catcher Code: " << result << endl;

    int fail=PyRun_SimpleString(code.c_str());

    char const* postCatchCode =
"sys.stdout = __precatcherStdout\n";

    result=PyRun_SimpleString(postCatchCode);
    Pbug << "post catcher Code: " << result << endl;

    //    doAfterExecution(fail,code,putVariables,failOnException);

    PyObject* catcher = PyObject_GetAttrString(m, "__catcher");
    PyObject* output = PyObject_GetAttrString(catcher, "data");
    PyObject* ascii = PyUnicode_AsASCIIString(output);

    Pbug << "Catcher " << getHex(catcher) << " Output: " << getHex(output)
        << " Ascii " << getHex(ascii) << endl;
    stdout=string(PyBytes_AsString(ascii));

    Py_DECREF(ascii);

    return fail==0;
}

template <typename Result,class Func>
Result python3InterpreterWrapper::evaluateCodeInternal(
    const string &code,
    bool &success
)
{
    Pbug << "evaluateCodeInternal: " << code << endl;

    Result result=pTraits<Result>::zero;

    const word funcName("decisionFunction");
    string functionCode="def "+funcName+"():\n";

    std::stringstream ss(code.c_str());
    std::string line;
    while(std::getline(ss, line)) {
        functionCode+="    "+line+"\n";
    }

    Pbug << "Function code:" << endl
        << functionCode;

    PyObject *m = PyImport_AddModule("__main__");
    PyObject *d = PyModule_GetDict(m);

    PyObject *pResult=NULL;
    PyObject *pCode=(Py_CompileString(functionCode.c_str(),"<string from swak>",Py_file_input));

    if( pCode!=NULL && !PyErr_Occurred()) {
        Pbug << "Compiled " << code << endl;

        PyObject *pFunc=PyFunction_New(pCode,d);
        Dbug << "Is function: " << PyFunction_Check(pFunc) << endl;

        PyObject *pTemp=PyObject_CallFunction(pFunc,NULL);
        Py_DECREF(pTemp);

        pResult=PyRun_String((funcName+"()").c_str(),Py_eval_input,d,d);

        Py_DECREF(pFunc);
        Py_DECREF(pCode);
    }

    Pbug << "Has Python-result " << getHex(pResult) << endl;
    if(pResult!=NULL) {
        if(debug) {
            PyObject *str=PyObject_Str(pResult);
            Pbug << "String representation " << getHex(str) << endl;
            PyObject *ascii=PyUnicode_AsASCIIString(str);
            Pbug << " string at " << getHex(ascii)
                << endl;
            Pbug << "Result is " << PyBytes_AsString(ascii) << endl;

            Py_DECREF(ascii);
            Py_DECREF(str);
        }

        result=Func()(pResult);
        Pbug << "Evaluated to " << result << endl;

        Py_DECREF(pResult);
    }

    success=(pResult!=NULL && !PyErr_Occurred());

    Pbug << "Success of execution " << success << endl;

    return result;
}


void python3InterpreterWrapper::interactiveLoop(
    const string &banner
) {
    Dbug << "interactiveLoop" << endl;

    setInterpreter();

    if(!useIPython_) {
        if(banner!="") {
            Info << endl << banner.c_str() << endl;
        }
        Info << "Continue with Ctrl-D" << endl;

        PyRun_InteractiveLoop(stdin,"test");
        clearerr(stdin);
    } else {
        string cmdString="";
        if(oldIPython_) {
            cmdString=
                "_ipshell=IPython.Shell.IPythonShellEmbed()\n"
                "_ipshell.set_banner('"+banner+"')\n"
	        "IPython.completer.readline.parse_and_bind('tab: complete')\n"
                "_ipshell()\n";
            // this (parse and bind) currently has no effect in the embedded shell
        } else {
            cmdString=
                "IPython.embed(header='"+banner+"')\n";
        }
	Dbug << "Executing " << cmdString << " to start IPython" << endl;

        int fail=PyRun_SimpleString(cmdString.c_str());
        if(fail) {
            WarningIn("python3InterpreterWrapper::interactiveLoop")
                << "Problem executing "+cmdString
                    << endl;
        }
    }
}


bool python3InterpreterWrapper::importLib(
    const word &name,
    const word &asSpec,
    bool useMainThreadState
)
{
    word as=asSpec;
    if(as=="") {
        as=name;
    }

    Dbug << "Importing library " << name
        << " as " << as << endl;

    PyObject * mainModule = PyImport_AddModule("__main__");
    Pbug << "MainModule: " << getHex(mainModule) << endl;

    if(mainModule==NULL) {
        WarningIn("python3InterpreterWrapper::importLib(const word &name)")
            << "Could not get module __main__ when importing " << name
                << " in " << dict().name()
                << endl;
        return false;
    }

    PyThreadState *old=NULL;
    if(useMainThreadState) {
        Pbug << "Switch to main thread" << endl;
        old=PyThreadState_Swap(mainThreadState);
        Dbug << "Swapped from " << getHex(old) << " to "
            << getHex(mainThreadState) << "(main state)" << endl;
    }
    Pbug << "Actual import" << endl;
    PyObject * libModule = PyImport_ImportModule(name.c_str());
    Pbug << "LibModule: " << getHex(libModule) << endl;
    if(libModule==NULL) {
        WarningIn("python3InterpreterWrapper::importLib(const word &name)")
            << "Could not import " << name << " in " << dict().name()
                << endl;
        if(useMainThreadState) {
            Dbug << "Emergency swap of states" << endl;
            PyThreadState_Swap(old);
        }

        return false;
    }
    if(useMainThreadState) {
        Pbug << "Switch from main thread" << endl;
        PyObject * mainModule2 = PyImport_AddModule("__main__");
        if(mainModule2==NULL) {
            WarningIn("python3InterpreterWrapper::importLib(const word &name)")
                << "Could not get module __main__ when importing " << name
                    << " in " << dict().name()
                    << endl;
        } else {
            PyModule_AddObject(mainModule2, name.c_str(), libModule);
        }
        Dbug << "Swap of states - back" << endl;
        PyThreadState_Swap(old);
    }
    PyModule_AddObject(mainModule, as.c_str(), libModule);

    return true;
}

void python3InterpreterWrapper::doAfterExecution(
    bool fail,
    const string &code,
    bool putVariables,
    bool failOnException
)
{
    if(fail) {
        Info << "Python Exception" << endl;
        PyErr_Print();
    }

    if(
        interactiveAfterException_
        &&
        fail
    ) {
        Info << "Got an exception for "<< code
            << " now you can interact." << endl;
        interactiveLoop("Exception handling");
    }
    if(
        fail
        &&
        (
            !tolerateExceptions_
            ||
            failOnException
        )
    ) {
        FatalErrorIn("python3InterpreterWrapper::doAfterExecution")
            << "Python exception raised by " << nl
	    << code << endl
	    << "To debug set 'interactiveAfterException true;' in " << dict().name()
                << endl << exit(FatalError);
    }

    if(interactiveAfterExecute_) {
        Info << "Executed "<< code
            << " now you can interact." << endl;
        interactiveLoop("After execution");
    }

    if(putVariables) {
        setGlobals();
    }
}

void python3InterpreterWrapper::resultToPythonVariable(
    const word &var,
    const word &namespaceName,
    const ExpressionResult &value
) {
    PyObject *m = PyImport_AddModule("__main__");

    if(
        !useNumpy_
        ||
        value.isSingleValue()
    ) {
        Pbug << "Single value. No numpy" << endl;

        ExpressionResult val=value.getUniform(
            1,
            !warnOnNonUniform_,
            !Pstream::parRun() || !parallelMasterOnly_
        );

        Pbug << "Value: " << val;

        if(val.valueType()==pTraits<scalar>::typeName) {
            PyObject_SetAttrString
                (
                    m,
                    const_cast<char*>(var.c_str()),
                    PyFloat_FromDouble(
                        val.getResult<scalar>()()[0]
                    )
                );
        } else if(val.valueType()==pTraits<vector>::typeName) {
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
        } else if(val.valueType()==pTraits<symmTensor>::typeName) {
            const symmTensor v=val.getResult<symmTensor>()()[0];
            PyObject_SetAttrString
                (
                    m,
                    const_cast<char*>(var.c_str()),
                    Py_BuildValue(
                        "dddddd",
                        double(v.xx()),
                        double(v.xy()),
                        double(v.xz()),
                        double(v.yy()),
                        double(v.yz()),
                        double(v.zz())
                    )
                );
        } else if(val.valueType()==pTraits<tensor>::typeName) {
            const tensor v=val.getResult<tensor>()()[0];
            PyObject_SetAttrString
                (
                    m,
                    const_cast<char*>(var.c_str()),
                    Py_BuildValue(
                        "ddddddddd",
                        double(v.xx()),
                        double(v.xy()),
                        double(v.xz()),
                        double(v.yx()),
                        double(v.yy()),
                        double(v.yz()),
                        double(v.zx()),
                        double(v.zy()),
                        double(v.zz())
                    )
                );
        } else {
            FatalErrorIn("python3InterpreterWrapper::getGlobals()")
                << "The variable " << var << " in " << namespaceName
                    << " has the unsupported type "
                    << val.valueType() << endl
                    << exit(FatalError);
        }
    } else {
        const ExpressionResult &val=value;

        Pbug << "Building a numpy-Array for global " << var
            << " at address " << val.getAddressAsDecimal()
            << " with size " << val.size()
            << " and type " << val.valueType()
            << endl;

        OStringStream cmd;
        cmd << var << "=OpenFOAMFieldArray(";
        cmd << "address=" << val.getAddressAsDecimal() << ",";
        cmd << "typestr='<f" << label(sizeof(scalar)) << "',";
        cmd << "size=" << val.size();
        label nr=-1;
        if(val.valueType()==pTraits<scalar>::typeName) {
            nr=1;
        } else if(val.valueType()==pTraits<vector>::typeName) {
            nr=3;
            cmd << ",names=['x','y','z']";
        } else if(val.valueType()==pTraits<tensor>::typeName) {
            nr=9;
            cmd << ",names=['xx','xy','xz','yx','yy','yz','zx','zy','zz']";
        } else if(val.valueType()==pTraits<symmTensor>::typeName) {
            nr=6;
            cmd << ",names=['xx','xy','xz','yy','yz','zz']";
        } else if(val.valueType()==pTraits<sphericalTensor>::typeName) {
            nr=1;
        }
        if(nr>1) {
            cmd << ",nr=" << nr;
        }
        cmd << ")";

        Pbug << "Python: " << cmd.str() << endl;

        int result=PyRun_SimpleString(cmd.str().c_str());
        Pbug << "Result: " << result << endl;
    }
    Pbug << "Variable done" << endl;
}

void python3InterpreterWrapper::getGlobals()
{
    assertParallel("getGlobals");

    if(swakToInterpreterNamespaces_.size()==0) {
        return;
    }

    Pbug << "Getting global variables from namespaces "
        << swakToInterpreterNamespaces_ << endl;

    forAll(swakToInterpreterNamespaces_,nameI) {
        Pbug << "Namespace: " << swakToInterpreterNamespaces_[nameI] << endl;

        const GlobalVariablesRepository::ResultTable &vars=
            GlobalVariablesRepository::getGlobalVariables(
                obr()
            ).getNamespace(
               swakToInterpreterNamespaces_[nameI]
            );
        forAllConstIter(
            GlobalVariablesRepository::ResultTable,
            vars,
            iter
        ) {
            const word &var=iter.key();
            const ExpressionResult &value=*(*iter);

            Pbug << "Variable: " << var << endl;

            resultToPythonVariable(
                var,
                swakToInterpreterNamespaces_[nameI],
                value
            );
        }
        Pbug << "Namespace done" << endl;
    }
    Dbug << "End getGlobals" << endl;
}

void python3InterpreterWrapper::setGlobals()
{
    assertParallel("setGlobals");

    if(interpreterToSwakVariables_.size()==0) {
        return;
    }

    Dbug << "Writing variables " << interpreterToSwakVariables_
        << " to namespace " << interpreterToSwakNamespace_ << endl;

    PyObject *m = PyImport_AddModule("__main__");

    forAll(interpreterToSwakVariables_,i) {
        const word &name=interpreterToSwakVariables_[i];

        Dbug << "Getting variable "<< name << endl;

        if(
            !PyObject_HasAttrString(
                m,
                const_cast<char *>(name.c_str()))
        ) {
            FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                << "Variable " << name << " not found in Python __main__"
                    << exit(FatalError)
                    << endl;
        }
        PyObject *pVar=PyObject_GetAttrString(
            m,
            const_cast<char *>(name.c_str())
        );

        ExpressionResult eResult;

       if(
            PyNumber_Check(pVar)
            &&
            PySequence_Check(pVar)==0 // this rules out numpy-arrays
        ) {
            Dbug << name << " is a scalar" << endl;

            PyObject *val=PyNumber_Float(pVar);
            scalar result=PyFloat_AsDouble(val);
            Py_DECREF(val);

            Dbug << name << " is " << result << endl;

            eResult.setSingleValue(result);

        } else if(PySequence_Check(pVar)) {
            Dbug << name << " is a sequence" << endl;

            if(
                useNumpy_
                &&
                PyObject_HasAttrString(pVar,"__array_interface__")
            ) {
                Dbug << name << " is a numpy-array" << endl;

                PyObject *interface=PyObject_GetAttrString(
                    pVar,
                    "__array_interface__"
                );
                if(debug) {
                    PyObject *repr=PyObject_Str(interface);
                    PyObject *ascii=PyUnicode_AsASCIIString(repr);
                    Dbug << "__array__interface__"
                        << string(PyBytes_AsString(ascii)) << endl;
                    Py_DECREF(ascii);
                    Py_DECREF(repr);
                }
                OStringStream cmd;
                cmd << "<f" << label(sizeof(scalar));
                PyObject *typestrExpected=PyBytes_FromString(cmd.str().c_str());
                PyObject *typestrIsRepr=PyObject_Str(
                    PyMapping_GetItemString(
                        interface,
                        "typestr"
                    )
                );
                PyObject *typestrIs=PyUnicode_AsASCIIString(typestrIsRepr);

                Dbug << "Expected typestring "
                    << string(PyBytes_AsString(typestrExpected))
                    << " present typestring "
                    << string(PyBytes_AsString(typestrIs))
                    << endl;

                if(
                    string(PyBytes_AsString(typestrExpected))
                    !=
                    string(PyBytes_AsString(typestrIs))
                ) {
                    FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                        << "Expected typestring "
                            << string(PyBytes_AsString(typestrExpected))
                            << " and typestring "
                            << string(PyBytes_AsString(typestrIs))
                            << " of " << name << " are not the same"
                            << endl
                            << exit(FatalError);

                }
                PyObject *data=PyMapping_GetItemString(
                    interface,
                    "data"
                );
                void *dataPtr=(void*)(PyLong_AsLong(PySequence_GetItem(data,0)));

                PyObject *shape=PyMapping_GetItemString(
                    interface,
                    "shape"
                );
                label size=-1;
                label nrOfFloats=1;
                label rank=PyObject_Length(shape);
                if(rank!=1 && rank!=2) {
                    PyObject *repr=PyObject_Str(interface);
                    FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                        << "Shape in " << "__array__interface__"
                        << string(PyBytes_AsString(repr))
                            << " of " << name << " has wrong size."
                            << " Should have either 1 or 2 elements"
                            << endl
                            << exit(FatalError);
                    Py_DECREF(repr);
                }
                size=PyLong_AsLong(PySequence_GetItem(shape,0));
                if(rank==2) {
                    nrOfFloats=PyLong_AsLong(PySequence_GetItem(shape,1));
                }

                size_t bytes=size*nrOfFloats*sizeof(scalar);

                switch(nrOfFloats) {
                    case 1:
                        {
                            scalarField *field=new scalarField(size);
                            memcpy(field->data(),dataPtr,bytes);
                            eResult.setResult(field);
                        }
                        break;
                    case 3:
                        {
                            Field<vector> *field=new Field<vector>(size);
                            memcpy(field->data(),dataPtr,bytes);
                            eResult.setResult(field);
                        }
                        break;
                    case 6:
                        {
                            Field<symmTensor> *field=new Field<symmTensor>(size);
                            memcpy(field->data(),dataPtr,bytes);
                            eResult.setResult(field);
                        }
                        break;
                    case 9:
                        {
                            Field<tensor> *field=new Field<tensor>(size);
                            memcpy(field->data(),dataPtr,bytes);
                            eResult.setResult(field);
                        }
                        break;
                    default:
                        PyObject *repr=PyObject_Str(interface);
                        FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                            << "Number of values " << nrOfFloats
                                << " in " << "__array__interface__"
                                << string(PyBytes_AsString(repr))
                                << " of " << name << " unsupported."
                                << " Should be either 1 (scalar), 3 (vector) "
                                << " 6 (symmTensor) or 9 (tensor) elements"
                                << endl
                                << exit(FatalError);
                        Py_DECREF(repr);
                }

                Dbug << "Created result" << eResult << endl;

                Py_DECREF(typestrExpected);
                Py_DECREF(typestrIsRepr);
                Py_DECREF(typestrIs);
            } else {
                PyObject *tuple=PySequence_Tuple(pVar);
                if(PyTuple_GET_SIZE(tuple)==3) {
                    Dbug << name << " is a vector" << endl;

                    vector val;
                    bool success=PyArg_ParseTuple(tuple,"ddd",&(val.x()),&(val.y()),&(val.z()));
                    if(!success) {
                        FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                            << "Variable " << name << " is not a valid vector"
                                << exit(FatalError)
                                << endl;
                    }
                    Dbug << name << " is " << val << endl;

                    eResult.setSingleValue(val);
                } else {
                    FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                        << "Variable " << name << " is a tuple with the unknown size "
                            << PyTuple_GET_SIZE(tuple)
                            << exit(FatalError)
                            << endl;
                }
                Py_DECREF(tuple);
            }
        } else {
            FatalErrorIn("python3InterpreterWrapper::setGlobals()")
                << "Variable " << name << " is of an unknown type"
                    << exit(FatalError)
                    << endl;
        }

        ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
            obr()
        ).addValue(
            name,
            interpreterToSwakNamespace_,
            eResult
        );
        res.noReset();
    }
}

bool python3InterpreterWrapper::startDictionary() {
    Dbug << "Starting table " << endl;

    setInterpreter();

    PyObject *pyDict=PyDict_New();
    if(pyDict==NULL) {
        FatalErrorIn("python3InterpreterWrapper::startDictionary()")
            << "Could not allocate dictionary "
                << endl
                << exit(FatalError);
    }

    dictionaryStack_.push(pyDict);

    releaseInterpreter();

    return true;
}

PyObject *python3InterpreterWrapper::currentDictionary()
{
    setInterpreter();

    if(dictionaryStack_.empty()) {
        return PyImport_AddModule("__main__");
    } else {
        return dictionaryStack_.top();
    }
}

bool python3InterpreterWrapper::wrapUpDictionary(const word &name) {
    Dbug << "Setting global " << name << endl;

    setInterpreter();

    if(dictionaryStack_.empty()) {
        FatalErrorIn("python3InterpreterWrapper::wrapUpDictionary(const word &name)")
            << "Dictionary stack empty. This should not happen"
                << endl
                << exit(FatalError);
    }

    PyObject *pyDict=dictionaryStack_.pop();

    if(dictionaryStack_.empty()) {
        PyObject *parent = PyImport_AddModule("__main__");
        PyObject_SetAttrString(
            parent,
            name.c_str(),
            pyDict
        );
    } else {
        PyDict_SetItemString(
            dictionaryStack_.top(),
            name.c_str(),
            pyDict
        );
    }

    Py_DECREF(pyDict);

    releaseInterpreter();

    return true;
}

template<class T>
void makeFieldReference(const word &name,Field<T> &val) {
    OStringStream cmd;

    // OF-streams don't output pointers correctly
    std::ostringstream makeDec;
    makeDec << val.cdata();

    cmd << name << "=OpenFOAMFieldArray(";
    cmd << "address=" << word(makeDec.str()) << ",";
    cmd << "typestr='<f" << label(sizeof(scalar)) << "',";
    cmd << "size=" << val.size();
    label nr=-1;
    if(pTraits<T>::typeName==pTraits<scalar>::typeName) {
        nr=1;
    } else {
        nr=pTraits<T>::nComponents;
        cmd << ",names=[";
        for(label i=0;i<nr;i++) {
            if(i!=0) {
                cmd << ",";
            }
            cmd << "'" << pTraits<T>::componentNames[i] << "'";
        }
        cmd << "]";
    }
    if(nr>1) {
        cmd << ",nr=" << nr;
    }
    cmd << ")";

    PyRun_SimpleString(cmd.str().c_str());
}

void python3InterpreterWrapper::setReference(const word &name,Field<scalar> &value)
{
    makeFieldReference(name, value);
}

void python3InterpreterWrapper::setReference(const word &name,Field<vector> &value)
{
    makeFieldReference(name, value);
}

void python3InterpreterWrapper::setReference(const word &name,Field<tensor> &value)
{
    makeFieldReference(name, value);
}

void python3InterpreterWrapper::setReference(const word &name,Field<symmTensor> &value)
{
    makeFieldReference(name, value);
}

void python3InterpreterWrapper::setReference(const word &name,Field<sphericalTensor> &value)
{
    makeFieldReference(name, value);
}

#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
void python3InterpreterWrapper::setReference(const word &name,Field<symmTensor4thOrder> &value)
{
    makeFieldReference(name, value);
}

void python3InterpreterWrapper::setReference(const word &name,Field<diagTensor> &value)
{
    makeFieldReference(name, value);
}
#endif

void python3InterpreterWrapper::getVariablesFromDriver(
    CommonValueExpressionDriver &driver,
    const wordList &names
) {
    forAll(names,i) {
        const word &var=names[i];
        const ExpressionResult &value=const_cast<const CommonValueExpressionDriver&>(driver).variable(var);

        resultToPythonVariable(
            var,
            "variables",
            value
        );
    }
}

autoPtr<RawFoamDictionaryParserDriver> python3InterpreterWrapper::getParserInternal(
    RawFoamDictionaryParserDriver::ErrorMode mode
) {
    return autoPtr<RawFoamDictionaryParserDriver>(
        new Python3FoamDictionaryParserDriver(
            *this,
            mode
        )
    );
}

bool python3InterpreterWrapper::extractDictionary(
    const word &name,
    dictionary &dict
) {
    Dbug << "python3InterpreterWrapper::extractDictionary" << endl;

    setInterpreter();

    PyObject *main = PyImport_AddModule("__main__");

    if(
        !PyObject_HasAttrString(
            main,
            const_cast<char *>(name.c_str()))
    ) {
        WarningIn("python3InterpreterWrapper::extractDictionary(const word &name,dictionary &dict)")
            << "No dictionary " << name << " in Python-namespace"
                << endl;
        return false;
    }

    PyObject *input=PyObject_GetAttrString(
        main,
        name.c_str()
    );

    if(
        !PyDict_Check(input)
    ) {
        WarningIn("python3InterpreterWrapper::extractDictionary(const word &name,dictionary &dict)")
            << "The object " << name << " is not a dictionary but a "
                << Py_TYPE(input)->tp_name
                << endl;
    }

    extractDictionaryToDictionary(input, dict);

    releaseInterpreter();

    return true;
}

void python3InterpreterWrapper::extractDictionaryToDictionary(
    PyObject *pyDict,
    dictionary &dict
) {
    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(pyDict, &pos, &key, &value)) {
        PyObject *keyStr=PyObject_Str(key);
        word keyName(PyUnicode_AsString(keyStr));
        Py_DECREF(keyStr); keyStr=NULL;

        if(PyDict_Check(value)) {
            dictionary subdict;
            extractDictionaryToDictionary(value, subdict);
            dict.set(keyName,subdict);
        } else if(PyBool_Check(value)) {
            if(value==Py_True) {
                dict.set(keyName,true);
            } else {
                dict.set(keyName,false);
            }
        } else if(PyLong_Check(value)) {
            dict.set(keyName,PyLong_AsLong(value));
        } else if(
            PyNumber_Check(value)
            &&
            PySequence_Check(value)==0 // this rules out numpy-arrays
        ) {
            dict.set(keyName,PyFloat_AsDouble(value));
        } else if(PyUnicode_Check(value)) {
            string val(PyUnicode_AsString(value));
            if(ValidWord()(val)) {
                dict.set(keyName,word(val));
            } else {
                dict.set(keyName,val);
            }
        } else if(PySequence_Check(value)) {
            if(isList<checkBoolType>(value)) {
                dict.set(keyName,getList<getBoolValue,bool>(value)());
            } else if(isList<checkWordType>(value)) {
                dict.set(keyName,getList<getWordValue,word>(value)());
            } else if(isList<checkStringType>(value)) {
                dict.set(keyName,getList<getStringValue,string>(value)());
            } else if(isList<checkIntType>(value)) {
                dict.set(keyName,getList<getIntValue,label>(value)());
            } else if(isList<checkFloatType>(value)) {
                dict.set(keyName,getList<getFloatValue,scalar>(value)());
            } else if(isListList<checkBoolType>(value)) {
                dict.set(keyName,getListList<getBoolValue,bool>(value)());
            } else if(isListList<checkWordType>(value)) {
                dict.set(keyName,getListList<getWordValue,word>(value)());
            } else if(isListList<checkStringType>(value)) {
                dict.set(keyName,getListList<getStringValue,string>(value)());
            } else if(isListList<checkIntType>(value)) {
                dict.set(keyName,getListList<getIntValue,label>(value)());
            } else if(isListList<checkFloatType>(value)) {
                dict.set(keyName,getListList<getFloatValue,scalar>(value)());
            } else {
                notImplemented("Python sequence to Foam list");
            }
        } else {
            WarningIn("")
                << "Unsupported type for key " << keyName
                    << " : " << Py_TYPE(value)->tp_name
                    << endl;
        }
    }
}

} // namespace Foam

// ************************************************************************* //
