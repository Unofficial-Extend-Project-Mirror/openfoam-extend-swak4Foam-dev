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

Contributors/Copyright:
    2008-2011, 2013-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"
#ifdef FOAM_FVMESH_IS_DATAOBJECT

#include "listMeshDataFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(listMeshDataFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        listMeshDataFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

listMeshDataFunctionObject::listMeshDataFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict)
{
}

bool listMeshDataFunctionObject::start()
{
    Info << "Content of mesh data " << obr().name()
        << " at start" << endl;

    dumpData();

    return true;
}

void listMeshDataFunctionObject::writeSimple()
{
    Info << "Content of mesh data " << obr().name()
        << endl;

    dumpData();
}

void listMeshDataFunctionObject::dumpData()
{
    const data &theData=dynamicCast<const fvMesh&>(obr());

    Info << theData << endl;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

#endif

// ************************************************************************* //
