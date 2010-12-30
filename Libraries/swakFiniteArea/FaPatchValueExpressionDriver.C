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

#include "FaPatchValueExpressionDriver.H"

#include "Random.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(FaPatchValueExpressionDriver, 0);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaPatchValueExpressionDriver, dictionary, faPatch);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaPatchValueExpressionDriver, idName, faPatch);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const FaPatchValueExpressionDriver& orig)
:
    CommonValueExpressionDriver(orig),
    patch_(orig.patch_)
{}

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const faPatch& patch)
:
    CommonValueExpressionDriver(),
    patch_(patch)
{}

label getPatchID(const faMesh &mesh,const word &name)
{
    label result=mesh.boundary().findPatchID(name);
    if(result<0) {
        FatalErrorIn("getPatchID(const faMesh &mesh,const string &name)")
            << "The patch " << name << " was not found in "
                << mesh.boundary().names()
                << endl
                << abort(FatalError);

    }
    return result;
}

const faMesh &FaPatchValueExpressionDriver::faRegionMesh
(
    //    const dictionary &dict,
    const fvMesh &mesh
)
{
    // word regionName;

    // if(!dict.found("faRegion")) {
    //     if(debug) {
    //         Info << "Using original faMesh " << endl;
    //     }

    //     regionName="faMesh";
    // } else {
    //     regionName=word(dict.lookup("faRegion"));
    //     if(debug) {
    //         Info << "Using faMesh " << regionName  << endl;
    //     }
    // }

    
    //     return dynamicCast<const fvMesh&>( // doesn't work with gcc 4.2
    return dynamic_cast<const faMesh&>(
        //        mesh.subRegistry(
        mesh.lookupObject<edgeVectorField>(
            // this field must exist and is our only hint to the faMesh (as it doesn't seem to be registered)
            "edgeCentres"
        ).mesh()
    );    
}


FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    CommonValueExpressionDriver(dict),
    patch_(
        faRegionMesh(regionMesh(dict,mesh)).boundary()[
            getPatchID(
                faRegionMesh(regionMesh(dict,mesh)),
                dict.lookup(
                    "faPatchName"
                )
            )
        ]
    )
{
}

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const word& id,const fvMesh&mesh)
 :
    CommonValueExpressionDriver(),
    patch_(
        faRegionMesh(mesh).boundary()[
            getPatchID(
                faRegionMesh(mesh),
                id
            )
        ]
    )
{
}

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const faPatch& patch,const FaPatchValueExpressionDriver& old)
:
    CommonValueExpressionDriver(old),
    patch_(patch)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

FaPatchValueExpressionDriver::~FaPatchValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


void FaPatchValueExpressionDriver::parse (const std::string& f)
{
    content_ = f;
    scan_begin ();
    parserFaPatch::FaPatchValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing_);
    parser.parse ();
    scan_end ();
}

vectorField *FaPatchValueExpressionDriver::makePositionField()
{
    return new vectorField(patch_.edgeFaceCentres());
}

// vectorField *FaPatchValueExpressionDriver::makePointField()
// {
//     return new vectorField(patch_.patch().localPoints());
// }

vectorField *FaPatchValueExpressionDriver::makeFaceNormalField()
{
    return new vectorField(patch_.edgeNormals());
}

vectorField *FaPatchValueExpressionDriver::makeFaceAreaField()
{
    return new vectorField(patch_.edgeLengths());
}

vectorField *FaPatchValueExpressionDriver::makeCellNeighbourField()
{
    return new vectorField(patch_.edgeFaceCentres());
}

vectorField *FaPatchValueExpressionDriver::makeDeltaField()
{
    return new vectorField(patch_.delta());
}

scalarField *FaPatchValueExpressionDriver::makeWeightsField()
{
    return new scalarField(patch_.weights());
}

const fvMesh &FaPatchValueExpressionDriver::mesh() const
{
    return dynamic_cast<const fvMesh&>(patch_.boundaryMesh().mesh().thisDb());
}

label FaPatchValueExpressionDriver::size() const
{
    return patch_.size();
}

label FaPatchValueExpressionDriver::pointSize() const
{
    return patch_.nPoints();
}

scalarField *FaPatchValueExpressionDriver::makeFaceIdField()
{
    scalarField *result=new scalarField(patch_.size());
    forAll(*result,i) {
        (*result)[i]=i;
    }
    return result;
}

// ************************************************************************* //

} // namespace
