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
    )
{
    if(interpreterCount==0) {
        if(debug) {
            Info << "Initializing Python" << endl;
        }
        Py_Initialize();        
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
                << abort(FatalError);
    }

    interpreterCount++;

    pythonState_=Py_NewInterpreter();

    if(debug) {
        Info << "Currently " << interpreterCount 
            << " Python interpreters (created one)" << endl;
    }
}

pythonInterpreterWrapper::~pythonInterpreterWrapper()
{
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
}

void pythonInterpreterWrapper::setInterpreter()
{
    PyThreadState_Swap(pythonState_);
}

bool pythonInterpreterWrapper::executeCode(const string &code,bool failOnException)
{
    setInterpreter();

    getGlobals();

    int success=PyRun_SimpleString(code.c_str());
    if(
        success!=0
        &&
        (
            !tolerateExceptions_
            ||
            failOnException
        )
    ) {
        FatalErrorIn("pythonInterpreterWrapper::executeCode(const string &code)")
            << "Python exception raised by " << nl
                << code
                << endl << abort(FatalError);
    }

    setGlobals();

    return success==0;
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
                        var.c_str(),
                        PyFloat_FromDouble(
                            val.getResult<scalar>()()[0]
                        )
                    );                
            } else if(val.type()==pTraits<vector>::typeName) {
                const vector v=val.getResult<vector>()()[0];
                PyObject_SetAttrString
                    (
                        m,
                        var.c_str(),
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
                        << abort(FatalError);
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
                << abort(FatalError);
    }
    if(
        !dict.found(prefix+"Code")
        &&
        !dict.found(prefix+"File")
    ) {
        FatalErrorIn("pythonInterpreterWrapper::readCode")
            << "Neither " << prefix+"Code" << " nor " 
                << prefix+"File" << " specified" << endl
                << abort(FatalError);
    }
    if(dict.found(prefix+"Code")) {
        code=string(dict.lookup(prefix+"Code"));
    } else {
        fileName fName(dict.lookup(prefix+"File"));
        fName.expand();
        if(!exists(fName)) {
            FatalErrorIn("pythonInterpreterWrapper::readCode")
                << "Can't find source file " << fName 
                    << endl << abort(FatalError);
        }

        IFstream in(fName);
        code="";
        while(in.good()) {
            char c;
            in.get(c);
            code+=c;
        }
    }
}


} // namespace Foam

// ************************************************************************* //
