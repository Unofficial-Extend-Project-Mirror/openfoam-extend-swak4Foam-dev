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

#include "addToRunTimeSelectionTable.H"

#include "dictionary.H"
#include "string.H"
#include "word.H"

#include <Python.h>

#include <memory>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class pythonInterpreterWrapper Declaration
\*---------------------------------------------------------------------------*/
class pyFunctionObjectDriver
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    pyFunctionObjectDriver( const pyFunctionObjectDriver& );
    
    //- Disallow default bitwise assignment
    void operator = ( const pyFunctionObjectDriver& );

    // Private data

    //- state of 'my' Python subinterpreter
    PyThreadState *pythonState_;

public:
    //- Runtime type information
    TypeName( "pyFunctionObjectDriver" );

    //- Execute code. Return true if there was no problem
    bool executeCode( const string &code, bool failOnException = false );

    // Constructors

    pyFunctionObjectDriver( const string &code );

    virtual ~pyFunctionObjectDriver();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
} // End namespace Foam


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug( pyFunctionObjectDriver, 0 );

    std::auto_ptr< pyFunctionObjectDriver > 
    DRIVER( new pyFunctionObjectDriver( "import Foam.functionObjects" ) );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

pyFunctionObjectDriver::pyFunctionObjectDriver( const string &code )
{
    if(debug) {
        Info << "Initializing Python" << endl;
    }
    Py_Initialize();        

    pythonState_ = Py_NewInterpreter();

    if(debug) {
        Info << "Currently " << pythonState_ 
	     << " Python interpreters (created one)" << endl;
    }

    executeCode( code );
}

pyFunctionObjectDriver::~pyFunctionObjectDriver()
{
    PyThreadState_Swap( pythonState_ );
    Py_EndInterpreter( pythonState_ );

    if(debug) {
        Info << "Currently " << pythonState_ 
	     << " Python interpreters (deleting one)" << endl;
    }

    if(debug) {
        Info << "Finalizing Python" << endl;
    }
    pythonState_ = NULL;
    PyThreadState_Swap( NULL );

    // This causes a segfault
    // Py_Finalize();        
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool pyFunctionObjectDriver::executeCode( const string &code, bool failOnException )
{
    int success = PyRun_SimpleString( code.c_str() );
    if( success != 0 && failOnException ){
        FatalErrorIn("pyFunctionObjectDriver::executeCode(const string &code)")
            << "Python exception raised by " << nl
                << code
                << endl << abort(FatalError);
    }

    return success == 0;
}

} // namespace Foam


// ************************************************************************* //
