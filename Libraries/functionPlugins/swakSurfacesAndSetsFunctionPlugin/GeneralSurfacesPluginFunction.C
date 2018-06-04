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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
