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

Class
    pythonIntegrationFunctionObject

Description
    Integrates a python-interpreter that allows the execution of Python-scripts

SourceFiles
    pythonIntegrationFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef pythonIntegrationFunctionObject_H
#define pythonIntegrationFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "OFstream.H"
#include "HashPtrTable.H"
#include "boolList.H"
#include "Field.H"

#include "Python.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class pythonIntegrationFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class pythonIntegrationFunctionObject
:
    public functionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    pythonIntegrationFunctionObject(const pythonIntegrationFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const pythonIntegrationFunctionObject&);

    // Private data

    //- state of 'my' Python subinterpreter
    PyThreadState *pythonState_;

    //- should exceptions be tolerated or lead to a fatal error?
    bool tolerateExceptions_;

    //- Code to be executed at the start
    string startCode_;

    //- Code to be executed at write
    string writeCode_;
  
    //- Code to be executed at execute
    string executeCode_;

    //- reference to the time
    const Time& time_;

    //- set the python variable with the time to the current runTime
    void setRunTime();

protected:
    //- how many instances exist (we need a separate interpreter for each of those)
    static label interpreterCount;

    void executeCode(const string &code);

public:

    //- Runtime type information
    TypeName("pythonIntegration");


    // Constructors

    //- Construct from components
    pythonIntegrationFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    ~pythonIntegrationFunctionObject();

    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute();

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

    //- Sample and write
    virtual void write();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
