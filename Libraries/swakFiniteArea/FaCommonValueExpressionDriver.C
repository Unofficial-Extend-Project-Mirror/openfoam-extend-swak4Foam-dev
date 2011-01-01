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

 ICE Revision: $Id$ 
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
