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

#include "PatchValueExpressionDriver.H"
#include "PatchValuePluginFunction.H"

#include "FieldValueExpressionDriver.H"

#include "SubsetValueExpressionDriver.H"

#include "Random.H"

#include "addToRunTimeSelectionTable.H"

#include <nearWallDist.H>

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

word PatchValueExpressionDriver::driverName_="patch";

defineTypeNameAndDebug(PatchValueExpressionDriver, 0);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, PatchValueExpressionDriver, dictionary, patch);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, PatchValueExpressionDriver, idName, patch);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


PatchValueExpressionDriver::PatchValueExpressionDriver(
    const PatchValueExpressionDriver& orig
)
:
    CommonValueExpressionDriver(orig),
    patch_(orig.patch_)
{}

PatchValueExpressionDriver::PatchValueExpressionDriver(const fvPatch& patch)
:
    CommonValueExpressionDriver(),
    patch_(patch)
{}

PatchValueExpressionDriver::PatchValueExpressionDriver(
    const dictionary& dict,
    const fvPatch& patch
)
:
    CommonValueExpressionDriver(dict),
    patch_(patch)
{}

label getPatchID(const fvMesh &mesh,const word &name)
{
    label result=mesh.boundaryMesh().findPatchID(name);
    if(result<0) {
        FatalErrorIn("getPatchID(const fvMesh &mesh,const string &name)")
            << "The patch " << name << " was not found in "
                << mesh.boundaryMesh().names()
                << endl
                << exit(FatalError);

    }
    return result;
}

PatchValueExpressionDriver::PatchValueExpressionDriver(
    const dictionary& dict,
    const fvMesh& mesh
)
 :
    CommonValueExpressionDriver(dict),
    patch_(
        regionMesh(
            dict,
            mesh,
            searchOnDisc()
        ).boundary()[
            getPatchID(
                regionMesh(
                    dict,
                    mesh,
                    searchOnDisc()
                ),
                dict.lookup(
                    "patchName"
                )
            )
        ]
    )
{
}

PatchValueExpressionDriver::PatchValueExpressionDriver(
    const word& id,
    const fvMesh&mesh
)
 :
    CommonValueExpressionDriver(),
    patch_(
        mesh.boundary()[
            getPatchID(
                mesh,
                id
            )
        ]
    )
{
}

PatchValueExpressionDriver::PatchValueExpressionDriver(
    const fvPatch& patch,
    const PatchValueExpressionDriver& old
)
:
    CommonValueExpressionDriver(old),
    patch_(patch)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

PatchValueExpressionDriver::~PatchValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


void PatchValueExpressionDriver::parseInternal (int startToken)
{
    parserPatch::PatchValueExpressionParser parser (
        scanner_,
        *this,
        startToken,
        0
    );
    parser.set_debug_level (traceParsing());
    parser.parse ();
}

vectorField *PatchValueExpressionDriver::makePositionField()
{
    return new vectorField(patch_.Cf());
}

vectorField *PatchValueExpressionDriver::makePointField()
{
    return new vectorField(patch_.patch().localPoints());
}

vectorField *PatchValueExpressionDriver::makeFaceNormalField()
{
    return new vectorField(patch_.nf());
}

vectorField *PatchValueExpressionDriver::makeFaceAreaField()
{
    return new vectorField(patch_.Sf());
}

vectorField *PatchValueExpressionDriver::makeCellNeighbourField()
{
    return new vectorField(patch_.Cn());
}

vectorField *PatchValueExpressionDriver::makeDeltaField()
{
    return new vectorField(patch_.delta());
}

scalarField *PatchValueExpressionDriver::makeWeightsField()
{
    return new scalarField(patch_.weights());
}

const fvMesh &PatchValueExpressionDriver::mesh() const
{
    return patch_.boundaryMesh().mesh();
}

label PatchValueExpressionDriver::size() const
{
    return patch_.size();
}

label PatchValueExpressionDriver::pointSize() const
{
    return patch_.patch().nPoints();
}

scalarField *PatchValueExpressionDriver::makeFaceIdField()
{
    scalarField *result=new scalarField(patch_.size());
    forAll(*result,i) {
        (*result)[i]=i;
    }
    return result;
}

scalarField *PatchValueExpressionDriver::makeNearDistField()
{
    scalarField *result=new scalarField(patch_.size());
    nearWallDist dist(this->mesh());
    (*result)=dist[patch_.index()];
    return result;
}

template<>
PatchValueExpressionDriver::SymbolTable<PatchValueExpressionDriver>::SymbolTable()
:
StartupSymbols()
{
    // default value
    insert("",parserPatch::PatchValueExpressionParser::token::START_DEFAULT);

    insert(
        "scalar_SC",
        parserPatch::PatchValueExpressionParser::token::START_FACE_SCALAR_COMMA
    );
    insert(
        "scalar_CL",
        parserPatch::PatchValueExpressionParser::token::START_FACE_SCALAR_CLOSE
    );
    insert(
        "point_scalar_SC",
        parserPatch::PatchValueExpressionParser::token::START_POINT_SCALAR_COMMA
    );
    insert(
        "point_scalar_CL",
        parserPatch::PatchValueExpressionParser::token::START_POINT_SCALAR_CLOSE
    );
    insert(
        "vector_SC",
        parserPatch::PatchValueExpressionParser::token::START_FACE_VECTOR_COMMA
    );
    insert(
        "vector_CL",
        parserPatch::PatchValueExpressionParser::token::START_FACE_VECTOR_CLOSE
    );
    insert(
        "point_vector_SC",
        parserPatch::PatchValueExpressionParser::token::START_POINT_VECTOR_COMMA
    );
    insert(
        "point_vector_CL",
        parserPatch::PatchValueExpressionParser::token::START_POINT_VECTOR_CLOSE
    );
    insert(
        "tensor_SC",
        parserPatch::PatchValueExpressionParser::token::START_FACE_TENSOR_COMMA
    );
    insert(
        "tensor_CL",
        parserPatch::PatchValueExpressionParser::token::START_FACE_TENSOR_CLOSE
    );
    insert(
        "point_tensor_SC",
        parserPatch::PatchValueExpressionParser::token::START_POINT_TENSOR_COMMA
    );
    insert(
        "point_tensor_CL",
        parserPatch::PatchValueExpressionParser::token::START_POINT_TENSOR_CLOSE
    );
    insert(
        "symmTensor_SC",
        parserPatch::PatchValueExpressionParser::token::START_FACE_YTENSOR_COMMA
    );
    insert(
        "symmTensor_CL",
        parserPatch::PatchValueExpressionParser::token::START_FACE_YTENSOR_CLOSE
    );
    insert(
        "point_symmTensor_SC",
        parserPatch::PatchValueExpressionParser::token::START_POINT_YTENSOR_COMMA
    );
    insert(
        "point_symmTensor_CL",
        parserPatch::PatchValueExpressionParser::token::START_POINT_YTENSOR_CLOSE
    );
    insert(
        "sphericalTensor_SC",
        parserPatch::PatchValueExpressionParser::token::START_FACE_HTENSOR_COMMA
    );
    insert(
        "sphericalTensor_CL",
        parserPatch::PatchValueExpressionParser::token::START_FACE_HTENSOR_CLOSE
    );
    insert(
        "point_sphericalTensor_SC",
        parserPatch::PatchValueExpressionParser::token::START_POINT_HTENSOR_COMMA
    );
    insert(
        "point_sphericalTensor_CL",
        parserPatch::PatchValueExpressionParser::token::START_POINT_HTENSOR_CLOSE
    );
    insert(
        "logical_SC",
        parserPatch::PatchValueExpressionParser::token::START_FACE_LOGICAL_COMMA
    );
    insert(
        "logical_CL",
        parserPatch::PatchValueExpressionParser::token::START_FACE_LOGICAL_CLOSE
    );
    insert(
        "point_logical_SC",
        parserPatch::PatchValueExpressionParser::token::START_POINT_LOGICAL_COMMA
    );
    insert(
        "point_logical_CL",
        parserPatch::PatchValueExpressionParser::token::START_POINT_LOGICAL_CLOSE
    );

    insert(
        "CL",
        parserPatch::PatchValueExpressionParser::token::START_CLOSE_ONLY
    );
    insert(
        "SC",
        parserPatch::PatchValueExpressionParser::token::START_COMMA_ONLY
    );
}

const PatchValueExpressionDriver::SymbolTable<PatchValueExpressionDriver> &PatchValueExpressionDriver::symbolTable()
{
    static SymbolTable<PatchValueExpressionDriver> actualTable;

    return actualTable;
}

int PatchValueExpressionDriver::startupSymbol(const word &name) {
    return symbolTable()[name];
}


autoPtr<CommonPluginFunction> PatchValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        PatchValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool PatchValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return PatchValuePluginFunction::exists(
        *this,
        name
    );
}

// ************************************************************************* //

} // namespace
