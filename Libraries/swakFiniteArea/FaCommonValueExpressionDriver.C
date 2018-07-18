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
    2010-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FaCommonValueExpressionDriver.H"

#include "Random.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(FaCommonValueExpressionDriver, 0);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


FaCommonValueExpressionDriver::FaCommonValueExpressionDriver(const FaCommonValueExpressionDriver& orig)
:
    CommonValueExpressionDriver(orig)
{}

const faMesh &FaCommonValueExpressionDriver::faRegionMesh
(
    const fvMesh &mesh
)
{
    //     return dynamicCast<const fvMesh&>( // doesn't work with gcc 4.2
    return dynamic_cast<const faMesh&>(
        //        mesh.subRegistry(
        mesh.lookupObject<edgeVectorField>(
            // this field must exist and is our only hint to the faMesh (as it doesn't seem to be registered)
            "edgeCentres"
        ).mesh()
    );
}


FaCommonValueExpressionDriver::FaCommonValueExpressionDriver(
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
 :
    CommonValueExpressionDriver(cacheReadFields,searchInMemory,searchOnDisc)
{
}

FaCommonValueExpressionDriver::FaCommonValueExpressionDriver(const dictionary& dict)
 :
    CommonValueExpressionDriver(dict)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

FaCommonValueExpressionDriver::~FaCommonValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// ************************************************************************* //

} // namespace
