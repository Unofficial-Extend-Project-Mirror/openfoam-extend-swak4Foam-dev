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
    2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SimpleRegionSolverFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#ifdef FOAM_MESHOBJECT_GRAVITY
# include "gravityMeshObject.H"
#else
# include "uniformDimensionedFields.H"
#endif

#include "MeshesRepository.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(SimpleRegionSolverFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

SimpleRegionSolverFunctionObject::SimpleRegionSolverFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(
        name
    ),
    dict_(dict),
    masterRegion_(
        dict_.found("region")
        ? word(dict_.lookup("region"))
        : polyMesh::defaultRegion
    ),
    meshRegion_(
        dict_.lookup("meshRegion")
    ),
    obr_(t.lookupObject<objectRegistry>(masterRegion_)),
    mesh_(
        MeshesRepository::getRepository().addCoupledMesh(
            meshRegion_,
            masterRegion_,
            meshRegion_
        )
    )
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    ,lastTimeStepExecute_(-1)
#endif
{
    Dbug << "SimpleRegionSolverFunctionObject::SimpleRegionSolverFunctionObject" << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool SimpleRegionSolverFunctionObject::execute(bool forceWrite)
{
    Dbug << "SimpleRegionSolverFunctionObject::execute" << endl;

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    this->start();

    if(
        lastTimeStepExecute_
        !=
        obr().time().timeIndex()
    ) {
        lastTimeStepExecute_=obr().time().timeIndex();
    } else {
        return false;
    }
#endif
    if(
        obr().time().outputTime()
        ||
        forceWrite
    ) {
        solveRegion();
    }

    return true;
}

bool SimpleRegionSolverFunctionObject::read(const dictionary& dict)
{
    Dbug << "SimpleRegionSolverFunctionObject::read" << endl;

    this->start();

    if(dict_!=dict) {
        WarningIn("SimpleRegionSolverFunctionObject::read(const dictionary& dict)")
            << "Can't change the solver of " << this->name()
                << " during the run"
                << endl;

    }

    return true;
}

} // namespace Foam

// ************************************************************************* //
