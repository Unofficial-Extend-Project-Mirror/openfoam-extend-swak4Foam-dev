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

Class
    generalInterpreterWrapper

Description
    General wrapper for interpreters

SourceFiles
    generalInterpreterWrapper.C

Contributors/Copyright:
    2011-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef generalInterpreterWrapper_H
#define generalInterpreterWrapper_H

#include "dictionary.H"
#include "objectRegistry.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

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

    const dictionary& dict_;

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

    //- should we get an interactive Interpreter-Shell after every code-Execution?

    //- used for debugging the Interpreter-program
    bool interactiveAfterExecute_;

    //- used for debugging of failed Interpreter-program
    bool interactiveAfterException_;

    virtual void getGlobals() = 0;

    virtual void setGlobals() = 0;

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

    //- run an interactive sesssion
    virtual void interactiveLoop(const string &banner) = 0;

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

public:
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

    //- set up the environment with the basic variables
    virtual void initEnvironment(const Time &t) = 0;

    //- set the variable 'runTime' in the interpreter
    virtual void setRunTime(const Time &time) = 0;

    //- Runtime type information
    TypeName("generalInterpreterWrapper");

    const word &interpreterName() {
        return interpreterName_;
    }

    const word InterpreterName();

    const objectRegistry &obr() {
        return obr_;
    }

    const dictionary &dict() {
        return dict_;
    }

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
