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
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "GeneralSurfacesPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "SurfacesRepository.H"

#include "meshSearch.H"

namespace Foam {

defineTypeNameAndDebug(GeneralSurfacesPluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

GeneralSurfacesPluginFunction::GeneralSurfacesPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &resultType,
    const string &arguments
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        resultType,
        arguments
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void GeneralSurfacesPluginFunction::setArgument(
    label index,
    const word &value
) {
    assert(index==0);

    name_=value;
}

const sampledSurface &GeneralSurfacesPluginFunction::theSurface() const
{
    return SurfacesRepository::getRepository(mesh()).getSurface(
        name_,
        mesh()
    );
}

    // Would be nicer if this functionality was part of sampledSurface
const labelList &GeneralSurfacesPluginFunction::meshCells()
{
    const sampledSurface &surf=theSurface();
    const vectorField& fc = surf.Cf();

    meshCells_.set(
        new labelList(fc.size())
    );

    meshSearch meshSearcher(mesh());

    label seedCell=-1;

    label wrongCellNr=0;

    forAll(fc, triI)
    {
        seedCell=meshSearcher.findCell(
            fc[triI],
            seedCell,
            true
        );
        meshCells_()[triI]=seedCell;
        if(meshCells_()[triI]<0) {
            wrongCellNr++;
        }
    }

    reduce(wrongCellNr,plusOp<label>());
    if(wrongCellNr>0) {
        WarningIn("GeneralSurfacesPluginFunction::meshCells()")
            << "No cell found for " << wrongCellNr << " faces"
                << endl;

    }
    return meshCells_();
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
