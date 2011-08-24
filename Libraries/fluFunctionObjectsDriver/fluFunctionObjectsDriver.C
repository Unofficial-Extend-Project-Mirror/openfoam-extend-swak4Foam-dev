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
class fluFunctionObjectsDriver
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    fluFunctionObjectsDriver( const fluFunctionObjectsDriver& );
    
    //- Disallow default bitwise assignment
    void operator = ( const fluFunctionObjectsDriver& );

    // Private data

    //- state of 'my' Python subinterpreter
    bool _wasInitialized;

public:
    //- Runtime type information
    TypeName( "fluFunctionObjectsDriver" );

    //- Execute code. Return true if there was no problem
    bool executeCode( const string &code, bool failOnException = false );

    // Constructors

    fluFunctionObjectsDriver( const string &code );

    virtual ~fluFunctionObjectsDriver();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
} // End namespace Foam


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug( fluFunctionObjectsDriver, 1 );

    std::auto_ptr< fluFunctionObjectsDriver > 
    DRIVER( new fluFunctionObjectsDriver( "import Foam.functionObjects" ) );


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fluFunctionObjectsDriver::fluFunctionObjectsDriver( const string &code )
    : _wasInitialized( Py_IsInitialized() )
{
    if ( !this->_wasInitialized ) {
        if(debug) {
	    std::cout << "Initializing Python - " << this << std::endl;
	}
	Py_Initialize();        
    }

    executeCode( code );
}

fluFunctionObjectsDriver::~fluFunctionObjectsDriver()
{
    if ( !this->_wasInitialized ) {
        Py_Finalize();        

	if(debug) {
          std::cout << "Finalizing Python - " << this << std::endl;
	}
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool fluFunctionObjectsDriver::executeCode( const string &code, bool failOnException )
{
    int success = PyRun_SimpleString( code.c_str() );
    if( success != 0 && failOnException ){
        FatalErrorIn("fluFunctionObjectsDriver::executeCode(const string &code)")
            << "Python exception raised by " << nl
                << code
                << endl << abort(FatalError);
    }

    return success == 0;
}

} // namespace Foam


// ************************************************************************* //
