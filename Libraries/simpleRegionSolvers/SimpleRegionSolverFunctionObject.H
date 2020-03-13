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

Class
    SimpleRegionSolverFunctionObject

Description
    Basis for the other function objects here. Evaluates the most common options

SourceFiles
    SimpleRegionSolverFunctionObject.C

Contributors/Copyright:
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef SimpleRegionSolverFunctionObject_H
#define SimpleRegionSolverFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"

#include "autoPtr.H"
#include "objectRegistry.H"
#include "volFields.H"

#include "swak.H"
#include "switchableNotImplemented.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SimpleRegionSolverFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class SimpleRegionSolverFunctionObject
:
    public functionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    SimpleRegionSolverFunctionObject(const SimpleRegionSolverFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const SimpleRegionSolverFunctionObject&);

    //- Input dictionary
    dictionary dict_;

    //- name of the master region
    word masterRegion_;

    //- name of the mesh region
    word meshRegion_;

    //- mesh where the set goes
    const objectRegistry& obr_;

    //- the mesh
    autoPtr<fvMesh> mesh_;

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    //- make sure that execute is not called twice at one time-step
    label lastTimeStepExecute_;
#endif

protected:

    fvMesh &mesh() { return mesh_(); }

    const fvMesh &mesh() const { return mesh_; }

    const objectRegistry &obr() const { return obr_; }

    const word &meshRegion() { return meshRegion_; }

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    virtual bool start() = 0;
#endif

    //- actual solving
    virtual bool solveRegion() = 0;

public:

    // Constructors

    //- Construct from components
    SimpleRegionSolverFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    TypeName("SimpleRegionSolverFunctionObject");

    // Member Functions

    // //- start is called at the start of the time-loop
    // virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute(bool forceWrite);

#ifdef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
    bool execute()
        {
            return execute(false);
        }
#endif

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    virtual bool execute() {
        return execute(false);
    }

    virtual bool write() {
        return execute(true);
    }
#endif

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

#ifdef FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH
    // make functionObject happy but fail because this is not an implementation
    virtual void updateMesh(const mapPolyMesh&) {
        switchableNotImplemented(
            "SimpleRegionSolverFunctionObject::updateMesh",
            dict_
        );
    }

    // make functionObject happy but fail because this is not an implementation
    virtual void movePoints(
#ifdef FOAM_MOVEPOINTS_GETS_POLYMESH
        const polyMesh&
#else
        const pointField&
#endif
    ) {
        switchableNotImplemented(
            "SimpleRegionSolverFunctionObject::movePoints",
            dict_
        );
    }
#endif
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
