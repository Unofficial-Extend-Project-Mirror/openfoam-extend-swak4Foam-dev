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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "meshLayersFromPatchPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(meshLayersFromPatchPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, meshLayersFromPatchPluginFunction , name, meshLayersFromPatch);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

meshLayersFromPatchPluginFunction::meshLayersFromPatchPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    meshLayersGeneralPluginFunction(
        parentDriver,
        name,
        string("patchName primitive word")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void meshLayersFromPatchPluginFunction::setArgument(
    label index,
    const word &patchName
) {
    assert(index==0);

    patchName_=patchName;
}

void meshLayersFromPatchPluginFunction::initFacesAndCells()
{
    label patchI=mesh().boundaryMesh().findPatchID(patchName_);
    if(patchI<0) {
        FatalErrorIn("meshLayersFromPatchPluginFunction::initFacesAndCells()")
            << "Patch name " << patchName_ << " not in valid names"
                << mesh().boundaryMesh().names()
                << endl
                << exit(FatalError);
    }

    startFaces_=labelList(mesh().boundaryMesh()[patchI].size());
    for(label i=0;i<mesh().boundaryMesh()[patchI].size();i++) {
        startFaces_[i]=mesh().boundaryMesh()[patchI].start()+i;
    }
    startValues_=List<MeshLayersDistFromPatch>(
        mesh().boundaryMesh()[patchI].size(),
        MeshLayersDistFromPatch(1)
    );
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
