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

Contributors/Copyright:
    2011-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "pythonInterpreterWrapper.H"
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

// #include <fcntl.h>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(pythonInterpreterWrapper, 0);

    label pythonInterpreterWrapper::interpreterCount=0;
    PyThreadState *pythonInterpreterWrapper::mainThreadState=NULL;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

void pythonInterpreterWrapper::initIPython() {
    Dbug << "Initializing IPython" << endl;
    if(!triedIPython_) {
        Dbug << "For real" << endl;
        triedIPython_=true;

        if(useIPython_) {
    	    importLib("sys");
	    PyRun_SimpleString("if 'argv' not in dir(sys): sys.argv=['OF-executable']");

            Dbug << "Attempting to import IPython" << endl;

            //            int fail=PyRun_SimpleString("import IPython");
            bool fail=!importLib("IPython");
            if(fail) {
                WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
                    << "Importing of IPython failed. Falling back to regular shell"
                        << " for " << dict_.name()
                        << endl;
                useIPython_=false;
            } else {
                PyObject *ipython=PyImport_AddModule("IPython");
                if(ipython==NULL) {
                    WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
                        << "Can't find IPython-module. Switching IPython off"
                            << " for " << dict_.name()
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
                        WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
                            << "Old style IPython embedding"
                                << " for " << dict_.name()
                                << endl;
                    } else {
                        useIPython_=false;
                        WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
                            << "Did not find a known way of embedding IPython. Using normal shell"
                                << " for " << dict_.name()
                                << endl;

                    }
                }
            }
        }
        if(!useIPython_) {
            Dbug << "Preparing interpreter for convenient history editing" << endl;

            //            PyRun_SimpleString("import rlcompleter, readline");
            importLib("rlcompleter");
            importLib("readline");

            // this currently has no effect in the embedded shell
            PyRun_SimpleString("readline.parse_and_bind('tab: complete')");
        }
    }
}

pythonInterpreterWrapper::pythonInterpreterWrapper
(
    const objectRegistry& obr,
    const dictionary& dict,
    bool forceToNamespace
):
    obr_(obr),
    dict_(dict),
    pythonState_(NULL),
    useNumpy_(dict.lookupOrDefault<bool>("useNumpy",true)),
    useIPython_(dict.lookupOrDefault<bool>("useIPython",true)),
    triedIPython_(false),
    oldIPython_(false),
    tolerateExceptions_(dict.lookupOrDefault<bool>("tolerateExceptions",false)),
    warnOnNonUniform_(dict.lookupOrDefault<bool>("warnOnNonUniform",true)),
    isParallelized_(dict.lookupOrDefault<bool>("isParallelized",false)),
    parallelMasterOnly_(false),
    swakToPythonNamespaces_(
        forceToNamespace
        ?
        wordList(dict.lookup("swakToPythonNamespaces"))
        :
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
    Pbug << "Starting constructor" << endl;

    syncParallel();

#ifdef FOAM_HAS_LOCAL_DEBUGSWITCHES
    debug=dict.lookupOrDefault<label>("debugPythonWrapper",debug());
#else
    debug=dict.lookupOrDefault<label>("debugPythonWrapper",debug);
#endif

    if(!dict.found("useNumpy")) {
        WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
            << "Switch 'useNumpy' not found in " << dict.name() << nl
                << "Assuming it to be 'true' (if that is not what you want "
                << "set it. Also set it to make this warning go away)"
                << endl;
    }

    if(!dict.found("useIPython")) {
        WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
            << "Switch 'useIPython' not found in " << dict.name() << nl
                << "Assuming it to be 'true' (if that is not what you want "
                << "set it. Also set it to make this warning go away)"
                << endl;
    }

    if(interpreterCount==0) {
        Pbug << "Initializing Python" << endl;

        Py_Initialize();

        if(debug) {
            PyThreadState *current=PyGILState_GetThisThreadState();
            Pbug << "GIL-state before thread" << getHex(current) << endl;
        }
        PyEval_InitThreads();
         if(debug) {
            PyThreadState *current=PyGILState_GetThisThreadState();
            Pbug << "GIL-state after thread" << getHex(current) << endl;
        }
        // importLib("scipy.stats","stats"); - OK
        mainThreadState = PyEval_SaveThread();
        // importLib("scipy.stats","stats"); - segFault

        Pbug << "Main thread state: " << getHex(mainThreadState) << endl;
        // PyRun_SimpleString("import IPython\n" // here it works as expected
        // "IPython.embed()\n");
    }

    if(Pstream::parRun()) {
        Pbug << "This is a parallel run" << endl;

        parallelMasterOnly_=readBool(dict.lookup("parallelMasterOnly"));
    }

    if(parallelNoRun(true)) {
        Pbug << "Getting out because of 'parallelNoRun'" << endl;
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

    Pbug << "Getting new interpreter" << endl;
    pythonState_=Py_NewInterpreter();
    Pbug << "Interpreter state: " << getHex(pythonState_) << endl;

    //    interactiveLoop("Clean");

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
            WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
                << "'useIPython' not needed in " << dict.name()
                    << " if there is no interactivity"
                    << endl;
        }
    }

    if(useNumpy_) {
        Dbug << "Attempting to import numpy" << endl;

        //        int fail=PyRun_SimpleString("import numpy");
        int fail=!importLib("numpy");
        if(fail) {
            FatalErrorIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
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
        WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
            << "No dictionary 'importLibs' found in " << dict.name()
                << endl << "If code hangs during importing try importing "
            "the libraries from here first"
                << endl;

    }

    PyEval_ReleaseThread(pythonState_);

    Pbug << "End constructor" << endl;
 }

void pythonInterpreterWrapper::initEnvironment(const Time &t)
{
    assertParallel("initEnvironment");

    Pbug << "initEnvironment" << endl;

    setInterpreter();

    PyObject *m = PyImport_AddModule("__main__");

    PyObject_SetAttrString(m,"functionObjectName",PyString_FromString("notAFunctionObject"));
    PyObject_SetAttrString(m,"caseDir",PyString_FromString(getEnv("FOAM_CASE").c_str()));
    PyObject_SetAttrString(m,"systemDir",PyString_FromString((t.path()/t.caseSystem()).c_str()));
    PyObject_SetAttrString(m,"constantDir",PyString_FromString((t.path()/t.caseConstant()).c_str()));
    PyObject_SetAttrString(m,"meshDir",PyString_FromString((t.path()/t.constant()/"polyMesh").c_str()));
    if(Pstream::parRun()) {
        PyObject_SetAttrString(m,"procDir",PyString_FromString(t.path().c_str()));
    }
    PyObject_SetAttrString(m,"parRun",PyBool_FromLong(Pstream::parRun()));
    PyObject_SetAttrString(m,"myProcNo",PyInt_FromLong(Pstream::myProcNo()));

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
        "                print 'Possible race condition while creating',d\n"

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

    releaseInterpreter();
}

bool pythonInterpreterWrapper::parallelMustBroadcast()
{
    return Pstream::parRun() && parallelMasterOnly_;
}

bool pythonInterpreterWrapper::parallelNoRun(bool doWarning)
{
    if(Pstream::parRun()) {
        if(isParallelized_ && doWarning) {
            WarningIn("bool pythonInterpreterWrapper::parallelNoRun()")
                << "The parameter 'isParallelized' was set in " << dict_.name()
                    << ". This means that the "
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
                        << " the 'isParallelized'-flag to true in " << dict_.name()
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

    Dbug << "Currently " << interpreterCount
        << " Python interpreters (deleting one)" << endl;

    if(pythonState_) {
        PyThreadState_Swap(pythonState_);
        Py_EndInterpreter(pythonState_);
        PyEval_ReleaseLock();
        pythonState_=NULL;
    }

    interpreterCount--;
    if(interpreterCount==0) {
        Dbug << "Finalizing Python" << endl;

        PyEval_RestoreThread(mainThreadState);
        // This causes a segfault
        //        Py_Finalize();
        WarningIn("pythonInterpreterWrapper::~pythonInterpreterWrapper()")
            << "Python not properly finalized (alternative would have been a "
                << "segmentation fault)." << endl
                << "This shouldn't be a problem as the program has finished anyway"
                << endl;

    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void pythonInterpreterWrapper::assertParallel(const word &name)
{
    if(Pstream::parRun() && parallelNoRun()) {
        FatalErrorIn("pythonInterpreterWrapper::"+name)
            << "This function is not meant to be called on this processor" << endl
                << "Programmers error"
                << endl
                << exit(FatalError);

    }
}

void pythonInterpreterWrapper::setRunTime(const Time &time)
{
    assertParallel("setRunTime");

    Pbug << "setRunTime " << time.timeName() << endl;

    setInterpreter();

    PyObject *m = PyImport_AddModule("__main__");
    PyObject_SetAttrString(m,"deltaT",PyFloat_FromDouble(time.deltaT().value()));
    PyObject_SetAttrString(m,"endTime",PyFloat_FromDouble(time.endTime().value()));
    PyObject_SetAttrString(m,"runTime",PyFloat_FromDouble(time.value()));
    PyObject_SetAttrString(m,"timeName",PyString_FromString(time.timeName().c_str()));
    PyObject_SetAttrString(m,"outputTime",PyBool_FromLong(time.outputTime()));
    PyObject_SetAttrString(m,"timeDir",PyString_FromString((time.path()/time.timeName()).c_str()));

    releaseInterpreter();
}

void pythonInterpreterWrapper::setInterpreter()
{
    assertParallel("setInterpreter");

    Pbug << "setInterpreter" << endl;

    if(debug) {
        PyThreadState *current=PyGILState_GetThisThreadState();
        Pbug << "Current GIL-state " << getHex(current) << endl;
    }

    if(pythonState_) {
        Pbug << "Setting state to " << getHex(pythonState_) << endl;
        PyThreadState *old=PyThreadState_Swap(pythonState_);
        //        PyThreadState *old=PyThreadState_Swap(mainThreadState);
        Pbug << "Old state: " << getHex(old) << endl;
    }
}

void pythonInterpreterWrapper::releaseInterpreter()
{
    assertParallel("releaseInterpreter");

    Pbug << "releaseInterpreter" << endl;
    PyEval_ReleaseThread(pythonState_);
    //     PyEval_ReleaseThread(mainThreadState);
}

bool pythonInterpreterWrapper::executeCode(
    const string &code,
    bool putVariables,
    bool failOnException
)
{
    Pbug << "ExecuteCode: " << code << endl;
    if(code.size()==0) {
        Pbug << "No code. Exiting" << endl;
        return true;
    }
    syncParallel();

    int fail=0;

    if(!parallelNoRun()) {
        setInterpreter();

        getGlobals();

        Pbug << "Execute" << endl;
        fail=PyRun_SimpleString(code.c_str());
        Pbug << "Fail: " << fail << endl;

        doAfterExecution(fail,code,putVariables,failOnException);

        releaseInterpreter();
    }

    if(parallelMustBroadcast()) {
        Pbug << "Prescatter: " << fail << endl;
        Pstream::scatter(fail);
        Pbug << "Postscatter: " << fail << endl;
        if(putVariables) {
            scatterGlobals();
        }
    }

    return fail==0;
}

bool pythonInterpreterWrapper::executeCodeCaptureOutput(
    const string &code,
    string &stdout,
    bool putVariables,
    bool failOnException
) {
    Pbug << "ExecuteCodeCaptureOutput: " << code << endl;
    syncParallel();

    int fail=0;

    if(!parallelNoRun()) {
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

        fail=PyRun_SimpleString(code.c_str());

        char const* postCatchCode =
"sys.stdout = __precatcherStdout\n";

        PyRun_SimpleString(postCatchCode);

        doAfterExecution(fail,code,putVariables,failOnException);

        PyObject* catcher = PyObject_GetAttrString(m, "__catcher");
        PyObject* output = PyObject_GetAttrString(catcher, "data");

        stdout=string(PyString_AsString(output));

        Pbug << "Fail: " << fail << " Captured: " << stdout << endl;

        releaseInterpreter();
    }

    if(parallelMustBroadcast()) {
        Pbug << "Prescatter: " << fail << endl;
        Pstream::scatter(fail);
        Pstream::scatter(stdout);
        Pbug << "Postscatter: " << fail << endl;
        if(putVariables) {
            scatterGlobals();
        }
    }

    return fail==0;
}

class pyToBool {
public:
    bool operator()(PyObject *&pResult) {
        return PyObject_IsTrue(pResult);
    }
};

bool pythonInterpreterWrapper::evaluateCodeTrueOrFalse(const string &code,bool failOnException)
{
    Pbug << "evaluateCodeTrueOrFalse" << endl;

    return evaluateCode<bool,pyToBool>(code,failOnException);
}

class pyToScalar {
public:
    scalar operator()(PyObject *&pResult) {
        return PyFloat_AsDouble(pResult);
    }
};

scalar pythonInterpreterWrapper::evaluateCodeScalar(const string &code,bool failOnException)
{
    Pbug << "evaluateCodeScalar" << endl;

    return evaluateCode<scalar,pyToScalar>(code,failOnException);
}

class pyToLabel {
public:
    label operator()(PyObject *&pResult) {
        return PyInt_AsLong(pResult);
    }
};

label pythonInterpreterWrapper::evaluateCodeLabel(const string &code,bool failOnException)
{
    Pbug << "evaluateCodeLabel" << endl;

    return evaluateCode<label,pyToLabel>(code,failOnException);
}

template <typename T,class Func>
T pythonInterpreterWrapper::evaluateCode(
    const string &code,
    bool failOnException
)
{
    Pbug << "evaluateCode: " << code << endl;
    syncParallel();

    T result=pTraits<T>::zero;
    if(!parallelNoRun()) {
        setInterpreter();

        getGlobals();

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

        if(pResult!=NULL) {
            if(debug) {
                PyObject *str=PyObject_Str(pResult);

                Pbug << "Result is " << PyString_AsString(str) << endl;

                Py_DECREF(str);
            }

            result=Func()(pResult);
            Pbug << "Evaluated to " << result << endl;

            Py_DECREF(pResult);
        }

        bool success=(pResult!=NULL && !PyErr_Occurred());

        Pbug << "Success of execution " << success << endl;

        doAfterExecution(!success,code,false,failOnException);

        releaseInterpreter();
    }

    if(parallelMustBroadcast()) {
        Pbug << "Prescatter: " << result << endl;
        Pstream::scatter(result);
        Pbug << "Postscatter: " << result << endl;
        //        scatterGlobals();
    }

    return result;
}


void pythonInterpreterWrapper::setInteractive(
    bool interactiveAfterExecute,
    bool interactiveAfterException
)
{
    interactiveAfterExecute_=interactiveAfterExecute;
    interactiveAfterException_=interactiveAfterException;
    if(
        interactiveAfterExecute_
        ||
        interactiveAfterException_
    ) {
        initIPython();
    }
}

void pythonInterpreterWrapper::interactiveLoop(
    const string &banner
) {
    Dbug << "interactiveLoop" << endl;

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
            WarningIn("pythonInterpreterWrapper::interactiveLoop")
                << "Problem executing "+cmdString
                    << endl;

        }
    }
}


bool pythonInterpreterWrapper::importLib(
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
    if(mainModule==NULL) {
        WarningIn("pythonInterpreterWrapper::importLib(const word &name)")
            << "Could not get module __main__ when importing " << name
                << " in " << dict_.name()
                << endl;
        return false;
    }

    PyThreadState *old=NULL;
    if(useMainThreadState) {
        old=PyThreadState_Swap(mainThreadState);
        Dbug << "Swapped from " << getHex(old) << " to "
            << getHex(mainThreadState) << "(main state)" << endl;
    }
    PyObject * libModule = PyImport_ImportModule(name.c_str());
    if(libModule==NULL) {
        WarningIn("pythonInterpreterWrapper::importLib(const word &name)")
            << "Could not import " << name << " in " << dict_.name()
                << endl;
        if(useMainThreadState) {
            Dbug << "Emergency swap of states" << endl;
            PyThreadState_Swap(old);
        }

        return false;
    }
    if(useMainThreadState) {
        PyObject * mainModule2 = PyImport_AddModule("__main__");
        if(mainModule2==NULL) {
            WarningIn("pythonInterpreterWrapper::importLib(const word &name)")
                << "Could not get module __main__ when importing " << name
                    << " in " << dict_.name()
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

void pythonInterpreterWrapper::doAfterExecution(
    bool fail,
    const string &code,
    bool putVariables,
    bool failOnException
)
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
            << " now you can interact." << endl;
        interactiveLoop("Exception handling");
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
	    << code << endl
	    << "To debug set 'interactiveAfterException true;' in " << dict_.name()
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

void pythonInterpreterWrapper::getGlobals()
{
    assertParallel("getGlobals");

    if(swakToPythonNamespaces_.size()==0) {
        return;
    }

    Pbug << "Getting global variables from namespaces "
        << swakToPythonNamespaces_ << endl;

    PyObject *m = PyImport_AddModule("__main__");

    forAll(swakToPythonNamespaces_,nameI) {
        Pbug << "Namespace: " << swakToPythonNamespaces_[nameI] << endl;

        const GlobalVariablesRepository::ResultTable &vars=
            GlobalVariablesRepository::getGlobalVariables(
                obr_
            ).getNamespace(
               swakToPythonNamespaces_[nameI]
            );
        forAllConstIter(
            GlobalVariablesRepository::ResultTable,
            vars,
            iter
        ) {
            const word &var=iter.key();
            const ExpressionResult &value=*(*iter);

            Pbug << "Variable: " << var << endl;

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
                } else {
                    FatalErrorIn("pythonInterpreterWrapper::getGlobals()")
                        << "The variable " << var << " has the unsupported type "
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
                cmd << "address='" << val.getAddressAsDecimal() << "',";
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

                PyRun_SimpleString(cmd.str().c_str());
            }
            Pbug << "Variable done" << endl;
        }
        Pbug << "Namespace done" << endl;
    }
    Dbug << "End getGlobals" << endl;
}

void pythonInterpreterWrapper::setGlobals()
{
    assertParallel("setGlobals");

    if(pythonToSwakVariables_.size()==0) {
        return;
    }

    Dbug << "Writing variables " << pythonToSwakVariables_
        << " to namespace " << pythonToSwakNamespace_ << endl;

    PyObject *m = PyImport_AddModule("__main__");

    forAll(pythonToSwakVariables_,i) {
        const word &name=pythonToSwakVariables_[i];

        Dbug << "Getting variable "<< name << endl;

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
                    Dbug << "__array__interface__"
                        << string(PyString_AsString(repr)) << endl;
                    Py_DECREF(repr);
                }
                OStringStream cmd;
                cmd << "<f" << label(sizeof(scalar));
                PyObject *typestrExpected=PyString_FromString(cmd.str().c_str());
                PyObject *typestrIs=PyObject_Str(
                    PyMapping_GetItemString(
                        interface,
                        "typestr"
                    )
                );

                Dbug << "Expected typestring "
                    << string(PyString_AsString(typestrExpected))
                    << " present typestring "
                    << string(PyString_AsString(typestrIs))
                    << endl;

                if(
                    string(PyString_AsString(typestrExpected))
                    !=
                    string(PyString_AsString(typestrIs))
                ) {
                    FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                        << "Expected typestring "
                            << string(PyString_AsString(typestrExpected))
                            << " and typestring "
                            << string(PyString_AsString(typestrIs))
                            << " of " << name << " are not the same"
                            << endl
                            << exit(FatalError);

                }
                PyObject *data=PyMapping_GetItemString(
                    interface,
                    "data"
                );
                void *dataPtr=(void*)(PyInt_AsLong(PySequence_GetItem(data,0)));

                PyObject *shape=PyMapping_GetItemString(
                    interface,
                    "shape"
                );
                label size=-1;
                label nrOfFloats=1;
                label rank=PyObject_Length(shape);
                if(rank!=1 && rank!=2) {
                    PyObject *repr=PyObject_Str(interface);
                    FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                        << "Shape in " << "__array__interface__"
                        << string(PyString_AsString(repr))
                            << " of " << name << " has wrong size."
                            << " Should have either 1 or 2 elements"
                            << endl
                            << exit(FatalError);
                    Py_DECREF(repr);
                }
                size=PyInt_AsLong(PySequence_GetItem(shape,0));
                if(rank==2) {
                    nrOfFloats=PyInt_AsLong(PySequence_GetItem(shape,1));
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
                        FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                            << "Number of values " << nrOfFloats
                                << " in " << "__array__interface__"
                                << string(PyString_AsString(repr))
                                << " of " << name << " unsupported."
                                << " Should be either 1 (scalar), 3 (vector) "
                                << " 6 (symmTensor) or 9 (tensor) elements"
                                << endl
                                << exit(FatalError);
                        Py_DECREF(repr);
                }

                Dbug << "Created result" << eResult << endl;

                Py_DECREF(typestrExpected);
                Py_DECREF(typestrIs);
            } else {
                PyObject *tuple=PySequence_Tuple(pVar);
                if(PyTuple_GET_SIZE(tuple)==3) {
                    Dbug << name << " is a vector" << endl;

                    vector val;
                    bool success=PyArg_ParseTuple(tuple,"ddd",&(val.x()),&(val.y()),&(val.z()));
                    if(!success) {
                        FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                            << "Variable " << name << " is not a valid vector"
                                << exit(FatalError)
                                << endl;
                    }
                    Dbug << name << " is " << val << endl;

                    eResult.setSingleValue(val);
                } else {
                    FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                        << "Variable " << name << " is a tuple with the unknown size "
                            << PyTuple_GET_SIZE(tuple)
                            << exit(FatalError)
                            << endl;
                }
                Py_DECREF(tuple);
            }
        } else {
            FatalErrorIn("pythonInterpreterWrapper::setGlobals()")
                << "Variable " << name << " is of an unknown type"
                    << exit(FatalError)
                    << endl;
        }

        ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
            obr_
        ).addValue(
            name,
            pythonToSwakNamespace_,
            eResult
        );
        res.noReset();
    }
}

string pythonInterpreterWrapper::readCode(
    const word &prefix,
    bool mustRead
) {
    string result;
    readCode(
        dict_,
        prefix,
        result,
        mustRead
    );
    return result;
}

void pythonInterpreterWrapper::readCode(
    const dictionary &dict,
    const word &prefix,
    string &code,
    bool mustRead
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
        mustRead
        &&
        (
            !dict.found(prefix+"Code")
            &&
            !dict.found(prefix+"File")
        )
    ) {
        FatalErrorIn("pythonInterpreterWrapper::readCode")
            << "Neither " << prefix+"Code" << " nor "
                << prefix+"File" << " specified in " << dict.name() << endl
                << exit(FatalError);
    }
    if(dict.found(prefix+"Code")) {
        code=string(dict.lookup(prefix+"Code"));
    } else {
        if(dict.found(prefix+"File")) {
            fileName fName(dict.lookup(prefix+"File"));
            fName.expand();
            if(!exists(fName)) {
                fileName oldFName=fName;
                fName=dict.name().path() / fName;
                if(!exists(fName)) {
                    FatalErrorIn("pythonInterpreterWrapper::readCode")
                        << "Can't find source file " << oldFName
                            << " or "<< fName
                            << endl << exit(FatalError);
                }
            }

            IFstream in(fName);
            code="";
            while(in.good()) {
                char c;
                in.get(c);
                code+=c;
            }
        } else {
            assert(mustRead==false);
            code="";
        }
    }
#ifdef FOAM_HAS_STRINGOPS
    code=stringOps::trim(code);
#endif
}

void pythonInterpreterWrapper::scatterGlobals()
{
    Pbug << "scatterGlobals" << endl;

    forAll(pythonToSwakVariables_,i) {
        const word &name=pythonToSwakVariables_[i];
        Pbug << "Scattering " << name << endl;

        ExpressionResult result;
        if(!parallelNoRun()) {
            result=*GlobalVariablesRepository::getGlobalVariables(
                obr_
            ).getNamespace(
                pythonToSwakNamespace_
            )[name];
            Pbug << "Value to scatter: " << result << endl;
        }

        // this doesn't work for some dubious reason
        // Pstream::scatter(result);

        //- do the scattering by ourself
        List<Pstream::commsStruct> comms;
        if (Pstream::nProcs() < Pstream::nProcsSimpleSum)
        {
            comms=Pstream::linearCommunication();
        }
        else
        {
            comms=Pstream::treeCommunication();
        }
        const Pstream::commsStruct& myComm = comms[Pstream::myProcNo()];
        if (myComm.above() != -1)
        {
            string incoming="";
            IPstream fromAbove(Pstream::scheduled, myComm.above());
            fromAbove >> incoming;
            IStringStream inStream(incoming);
            inStream >> result;
        }
        forAll(myComm.below(), belowI)
        {
            OPstream toBelow(Pstream::scheduled,myComm.below()[belowI]);
            OStringStream outgoing;
            outgoing << result;
            toBelow << outgoing.str();
        }

        if(parallelNoRun()) {
            Info << "Setting value of " << name << endl;

            ExpressionResult &res=GlobalVariablesRepository::getGlobalVariables(
                obr_
            ).addValue(
                name,
                pythonToSwakNamespace_,
                result
            );
            res.noReset();
        }
    }
}

void pythonInterpreterWrapper::syncParallel() const
{
    static label nr=0;

    if(Pstream::parRun()) {
        nr++;
        label id=nr;
        Pbug << "syncParallel: wait nr " << nr << endl;
        reduce(id,plusOp<label>());
        Pbug << "syncParallel: all here " << id << endl;
        if(id!=nr*Pstream::nProcs()) {
            Pout << "syncParallel. Id: " << id
                << " expected: " << nr*Pstream::nProcs() << endl;
            FatalErrorIn("pythonInterpreterWrapper::syncParallel()")
                << "Wrong sequence number. Out of sync"
                    << endl
                    << exit(FatalError);

         }
    } else {
        Dbug << "syncParallel: no wait" << endl;
    }
}

} // namespace Foam

// ************************************************************************* //
