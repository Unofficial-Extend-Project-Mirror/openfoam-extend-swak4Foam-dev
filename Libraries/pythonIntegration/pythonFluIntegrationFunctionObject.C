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

#include "pythonFluIntegrationFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"
#include "IFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(pythonFluIntegrationFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        pythonFluIntegrationFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

pythonFluIntegrationFunctionObject::pythonFluIntegrationFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    pythonIntegrationFunctionObject(name,t,dict)
{
    if(!executeCode("import Foam")) {
        FatalErrorIn("pythonFluIntegrationFunctionObject::pythonFluIntegrationFunctionObject")
            << "Python can not import module Foam. Probably no pythonFlu installed"
                << endl
                << abort(FatalError);
    }
    executeCode("import Foam.OpenFOAM as OpenFOAM",true);
    executeCode("import Foam.finiteVolume as finiteVolume",true);

    // This code snipplet from http://www.swig.org/Doc1.3/Python.html#Python_nn64
    // doesn't work
    // a) because these functions are unknown (and there is no header)
    // b) can't find an equivalent for $input
//     const Time *f=&t;
//     if (SWIG_ConvertPtr($input, (void **) &f, SWIGTYPE_p_Foo, SWIG_POINTER_EXCEPTION) == -1)
//         return NULL;

//     PyObject *obj;
//     obj = SWIG_NewPointerObj(f, SWIGTYPE_p_Foo, 0);
}

pythonFluIntegrationFunctionObject::~pythonFluIntegrationFunctionObject()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
