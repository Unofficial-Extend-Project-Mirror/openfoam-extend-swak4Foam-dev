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
    FaCommonValueExpressionDriver(orig),
    patch_(orig.patch_)
{}

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const faPatch& patch)
:
    FaCommonValueExpressionDriver(),
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
                << exit(FatalError);

    }
    return result;
}

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    FaCommonValueExpressionDriver(dict),
    patch_(
        faRegionMesh(
            regionMesh(
                dict,
                mesh,
                searchOnDisc()
            )).boundary()[
            getPatchID(
                faRegionMesh(
                    regionMesh(
                        dict,
                        mesh,
                        searchOnDisc()
                    )),
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
    FaCommonValueExpressionDriver(),
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
    FaCommonValueExpressionDriver(old),
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

vectorField *FaPatchValueExpressionDriver::makePointField()
{
    // if implemented go to the call in the grammar and reuse there
    notImplemented("FaPatchValueExpressionDriver::makePointField()");

    return new vectorField(0);
}

vectorField *FaPatchValueExpressionDriver::makeEdgeNormalField()
{
    return new vectorField(patch_.edgeNormals());
}

vectorField *FaPatchValueExpressionDriver::makeEdgeLengthField()
{
    return new vectorField(patch_.edgeLengths());
}

vectorField *FaPatchValueExpressionDriver::makeFaceNeighbourField()
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

const faMesh &FaPatchValueExpressionDriver::aMesh() const
{
    return patch_.boundaryMesh().mesh();
}

label FaPatchValueExpressionDriver::size() const
{
    return patch_.size();
}

label FaPatchValueExpressionDriver::pointSize() const
{
    return patch_.nPoints();
}

scalarField *FaPatchValueExpressionDriver::makeEdgeIdField()
{
    scalarField *result=new scalarField(patch_.size());
    forAll(*result,i) {
        (*result)[i]=i;
    }
    return result;
}

// ************************************************************************* //

} // namespace
