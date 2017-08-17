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
    python3IntegrationFunctionObject

Description
    Integrates a python3-interpreter that allows the execution of Python3-scripts

SourceFiles
    python3IntegrationFunctionObject.C

Contributors/Copyright:
    2011-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef python3IntegrationFunctionObject_H
#define python3IntegrationFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "OFstream.H"
#include "HashPtrTable.H"
#include "boolList.H"
#include "Field.H"

#include "python3InterpreterWrapper.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class python3IntegrationFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class python3IntegrationFunctionObject
:
    public functionObject,
    public python3InterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    python3IntegrationFunctionObject(const python3IntegrationFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const python3IntegrationFunctionObject&);

    // Private data

    //- Code to be executed at the start
    string startCode_;

    //- Code to be executed at the end
    string endCode_;

    //- Code to be executed at execute
    string executeCode_;

    //- Code to be executed when data is written to disc
    string writeCode_;

    //- reference to the time
    const Time& time_;

    //- set the python variable with the time to the current runTime
    void setRunTime();

public:

    //- Runtime type information
    TypeName("python3Integration");


    // Constructors

    //- Construct from components
    python3IntegrationFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    virtual ~python3IntegrationFunctionObject();

    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute(bool);

#ifdef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
    bool execute()
        {
            return execute(false);
        }
#else
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    virtual bool execute()
        {
            return execute(false);
        }

    virtual bool write()
        {
            return execute(true);
        }
#endif
#endif

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

    //- Execute in the end
    virtual bool end();

#ifdef FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH
    virtual void updateMesh(const mapPolyMesh&) {
    }

    virtual void movePoints(const polyMesh&) {
    }
#endif
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
