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

#include "pythonIntegrationFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(pythonIntegrationFunctionObject, 1);

    addToRunTimeSelectionTable
    (
        functionObject,
        pythonIntegrationFunctionObject,
        dictionary
    );

    label pythonIntegrationFunctionObject::interpreterCount=0;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

pythonIntegrationFunctionObject::pythonIntegrationFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(name)
{
    if(interpreterCount==0) {
        if(debug) {
            Info << "Initializing Python" << endl;
        }
        Py_Initialize();        
    }
    interpreterCount++;

    pythonState_=Py_NewInterpreter();

    if(debug) {
        Info << "Currently " << interpreterCount 
            << " Python interpreters (created one)" << endl;
    }
}

pythonIntegrationFunctionObject::~pythonIntegrationFunctionObject()
{
    PyThreadState_Swap(pythonState_);
    Py_EndInterpreter(pythonState_);
    if(debug) {
        Info << "Currently " << interpreterCount 
            << " Python interpreters (deleted one)" << endl;
    }

    interpreterCount--;
    if(interpreterCount==0) {
        if(debug) {
            Info << "Finalizing Python" << endl;
        }
        PyThreadState_Swap(NULL);
        Py_Finalize();        
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool pythonIntegrationFunctionObject::start()
{
    PyThreadState_Swap(pythonState_);

    return true;
}

bool pythonIntegrationFunctionObject::execute()
{
    PyThreadState_Swap(pythonState_);

    return true;
}

void pythonIntegrationFunctionObject::write()
{
    PyThreadState_Swap(pythonState_);
}

bool pythonIntegrationFunctionObject::read(const dictionary& dict)
{
    return start();
}

} // namespace Foam

// ************************************************************************* //
