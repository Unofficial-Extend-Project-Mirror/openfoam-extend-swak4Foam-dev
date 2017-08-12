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
    pythonInterpreterWrapper

Description
    Makes sure that the object receives a python-Interpreter on its own

SourceFiles
    pythonInterpreterWrapper.C

Contributors/Copyright:
    2011-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef pythonInterpreterWrapper_H
#define pythonInterpreterWrapper_H

#include "dictionary.H"
#include "objectRegistry.H"
#include "swakTime.H"

#include "Python.h"

#include "generalInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class pythonInterpreterWrapper Declaration
\*---------------------------------------------------------------------------*/


class pythonInterpreterWrapper
:
    public generalInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    pythonInterpreterWrapper(const pythonInterpreterWrapper&);

    //- Disallow default bitwise assignment
    void operator=(const pythonInterpreterWrapper&);

    // Private data

    //- state of 'my' Python subinterpreter
    PyThreadState *pythonState_;

    //- whether or not to use the numpy library
    bool useNumpy_;

    //- whether or not to use IPython as the shell if it is installed
    bool useIPython_;

    //- whether or not we tried IPython
    bool triedIPython_;

    //- embed IPython using the old convention
    bool oldIPython_;

    void getGlobals();

    void setGlobals();

    void initIPython();

    void initInteractiveSpecial();

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

public:

    //- Execute code and return a value
    template <typename T,class Func>
    T evaluateCode(
        const string &code,
        bool failOnException=false
    );

    //- Execute code and see if it returns to true or false
    bool evaluateCodeTrueOrFalse(
        const string &code,
        bool failOnException=false
    );

     //- Execute code and use the return value as a float value
    scalar evaluateCodeScalar(
        const string &code,
        bool failOnException=false
    );

      //- Execute code and use the return value as a integer value
    label evaluateCodeLabel(
        const string &code,
        bool failOnException=false
    );

    //- set up the environment with the basic variables
    void initEnvironment(const Time &t);

    //- set the variable 'runTime' in the interpreter
    void setRunTime(const Time &time);

    //- Runtime type information
    TypeName("pythonInterpreterWrapper");


    // Constructors

    //- Construct from components
    pythonInterpreterWrapper
    (
        const objectRegistry& obr,
        const dictionary&,
        bool forceToNamespace=false
    );

    virtual ~pythonInterpreterWrapper();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
