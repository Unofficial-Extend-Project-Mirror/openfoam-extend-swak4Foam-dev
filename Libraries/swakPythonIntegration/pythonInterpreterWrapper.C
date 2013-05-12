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
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

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
    const objectRegistry& obr,
    const dictionary& dict
):
    obr_(obr),
    useNumpy_(dict.lookupOrDefault<bool>("useNumpy",true)),
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
    debug=dict.lookupOrDefault<label>("debugPythonWrapper",debug);

    if(!dict.found("useNumpy")) {
        WarningIn("pythonInterpreterWrapper::pythonInterpreterWrapper")
            << "Switch 'useNumpy' not found in " << dict.name() << nl
                << "Assuming it to be 'true' (if that is not what you want "
                << "set it. Also set it to make this warning go away)"
                << endl;

    }

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

    if(useNumpy_) {
        if(debug) {
            Info << "Attempting to import numpy" << endl;
        }
        int fail=PyRun_SimpleString("import numpy");
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
}

void pythonInterpreterWrapper::initEnvironment(const Time &t)
{
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
        "        os.makedirs(d)\n"

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
    PyObject_SetAttrString(m,"deltaT",PyFloat_FromDouble(time.deltaT().value()));
    PyObject_SetAttrString(m,"endTime",PyFloat_FromDouble(time.endTime().value()));
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

class pyToBool {
public:
    bool operator()(PyObject *&pResult) {
        return PyObject_IsTrue(pResult);
    }
};

bool pythonInterpreterWrapper::evaluateCodeTrueOrFalse(const string &code,bool failOnException)
{
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
    return evaluateCode<label,pyToLabel>(code,failOnException);
}

template <typename T,class Func>
T pythonInterpreterWrapper::evaluateCode(const string &code,bool failOnException)
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
    //    bool result=false;
    T result=pTraits<T>::zero;

    if(pResult!=NULL) {
        if(debug) {
            PyObject *str=PyObject_Str(pResult);

            Info << "Result is " << PyString_AsString(str) << endl;

            Py_DECREF(str);
        }
        //        result=PyObject_IsTrue(pResult);
        result=Func()(pResult);
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

            if(
                !useNumpy_
                ||
                value.isSingleValue()
            ) {
                ExpressionResult val=value.getUniform(
                    1,
                    !warnOnNonUniform_
                );
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
                if(debug) {
                    Info << "Building a numpy-Array for global " << var
                        << " at address " << val.getAddressAsDecimal()
                        << " with size " << val.size()
                        << " and type " << val.valueType()
                        << endl;
                }
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
                if(debug) {
                    Info << "Python: " << cmd.str() << endl;
                }
                PyRun_SimpleString(cmd.str().c_str());
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

       if(
            PyNumber_Check(pVar)
            &&
            PySequence_Check(pVar)==0 // this rules out numpy-arrays
        ) {
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
            if(
                useNumpy_
                &&
                PyObject_HasAttrString(pVar,"__array_interface__")
            ) {
                if(debug) {
                    Info << name << " is a numpy-array" << endl;
                }
                PyObject *interface=PyObject_GetAttrString(
                    pVar,
                    "__array_interface__"
                );
                if(debug) {
                    PyObject *repr=PyObject_Str(interface);
                    Info << "__array__interface__"
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
                if(debug) {
                    Info << "Expected typestring "
                        << string(PyString_AsString(typestrExpected))
                        << " present typestring "
                        << string(PyString_AsString(typestrIs))
                        << endl;
                }
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

               if(debug) {
                    Info << "Created result" << eResult << endl;
                }

                Py_DECREF(typestrExpected);
                Py_DECREF(typestrIs);
            } else {
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
                << prefix+"File" << " specified" << endl
                << exit(FatalError);
    }
    if(dict.found(prefix+"Code")) {
        code=string(dict.lookup(prefix+"Code"));
    } else {
        if(dict.found(prefix+"File")) {
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
        } else {
            assert(mustRead==false);
            code="";
        }
    }
    code=stringOps::trim(code);
}


} // namespace Foam

// ************************************************************************* //
