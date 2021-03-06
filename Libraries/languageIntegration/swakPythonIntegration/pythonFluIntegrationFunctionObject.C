/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2011-2013, 2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "pythonFluIntegrationFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"
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
    if(!executeCode("import Foam",false)) {
        FatalErrorIn("pythonFluIntegrationFunctionObject::pythonFluIntegrationFunctionObject")
            << "Python can not import module Foam. Probably no pythonFlu installed"
                << endl
                << exit(FatalError);
    }
    executeCode("import Foam.OpenFOAM as OpenFOAM",false,true);
    executeCode("import Foam.finiteVolume as finiteVolume",false,true);

    executeCode("from Foam.integrationHelpers.getObjectsFromPointers import getTimeFromPtr",false,true); // this should work, but doesn't
    // executeCode("from Foam.src.OpenFOAM.db.Time.Time import getTimeFromPtrOld as getTimeFromPtr",false,true); // This works

    if(!parallelNoRun()) {
        setInterpreter();

        PyObject *time=PyCObject_FromVoidPtr((void*)(&t),NULL);

        PyObject *m = PyImport_AddModule("__main__");
        PyObject_SetAttrString(m,"theTime",time);
        executeCode("time=getTimeFromPtr(theTime)",true,false);

        // Get rid of the helper stuff
        PyRun_SimpleString("del theTime");
        PyRun_SimpleString("del getTimeFromPtr");

        releaseInterpreter();
    }
}

pythonFluIntegrationFunctionObject::~pythonFluIntegrationFunctionObject()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
