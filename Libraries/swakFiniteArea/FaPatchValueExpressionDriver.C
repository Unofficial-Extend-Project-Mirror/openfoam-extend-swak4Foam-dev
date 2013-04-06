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
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FaPatchValueExpressionDriver.H"
#include "FaPatchValuePluginFunction.H"

#include "Random.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

word FaPatchValueExpressionDriver::driverName_="faPatch";

defineTypeNameAndDebug(FaPatchValueExpressionDriver, 0);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaPatchValueExpressionDriver, dictionary, faPatch);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaPatchValueExpressionDriver, idName, faPatch);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(
    const FaPatchValueExpressionDriver& orig
)
:
    FaCommonValueExpressionDriver(orig),
    patch_(orig.patch_)
{}

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(
    const faPatch& patch
)
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

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(
    const dictionary& dict,
    const fvMesh&mesh
)
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

FaPatchValueExpressionDriver::FaPatchValueExpressionDriver(
    const word& id,
    const fvMesh&mesh
)
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


void FaPatchValueExpressionDriver::parseInternal (int startToken)
{
    parserFaPatch::FaPatchValueExpressionParser parser (
        scanner_,
        *this,
        startToken,
        0
    );
    parser.set_debug_level (traceParsing());
    parser.parse ();
}

tmp<vectorField> FaPatchValueExpressionDriver::makePositionField() const
{
    return tmp<vectorField>(new vectorField(patch_.edgeFaceCentres()));
}

tmp<vectorField> FaPatchValueExpressionDriver::makePointField() const
{
    // if implemented go to the call in the grammar and reuse there
    notImplemented("FaPatchValueExpressionDriver::makePointField()");

    return tmp<vectorField>(new vectorField(0));
}

tmp<vectorField> FaPatchValueExpressionDriver::makeEdgeNormalField() const
{
    return tmp<vectorField>(new vectorField(patch_.edgeNormals()));
}

tmp<vectorField> FaPatchValueExpressionDriver::makeEdgeLengthField() const
{
    return tmp<vectorField>(new vectorField(patch_.edgeLengths()));
}

tmp<vectorField> FaPatchValueExpressionDriver::makeFaceNeighbourField() const
{
    return tmp<vectorField>(new vectorField(patch_.edgeFaceCentres()));
}

tmp<vectorField> FaPatchValueExpressionDriver::makeDeltaField() const
{
    return tmp<vectorField>(new vectorField(patch_.delta()));
}

tmp<scalarField> FaPatchValueExpressionDriver::makeWeightsField() const
{
    return tmp<scalarField>(new scalarField(patch_.weights()));
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

tmp<scalarField> FaPatchValueExpressionDriver::makeEdgeIdField() const
{
    tmp<scalarField> result(new scalarField(patch_.size()));
    forAll(result(),i) {
        result()[i]=i;
    }
    return result;
}

template<>
FaPatchValueExpressionDriver::SymbolTable<FaPatchValueExpressionDriver>::SymbolTable()
:
StartupSymbols()
{
    // default value
    insert("",parserFaPatch::FaPatchValueExpressionParser::token::START_DEFAULT);

    insert(
        "scalar_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_SCALAR_COMMA
    );
    insert(
        "scalar_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_SCALAR_CLOSE
    );
    insert(
        "point_scalar_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_SCALAR_COMMA
    );
    insert(
        "point_scalar_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_SCALAR_CLOSE
    );
    insert(
        "vector_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_VECTOR_COMMA
    );
    insert(
        "vector_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_VECTOR_CLOSE
    );
    insert(
        "point_vector_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_VECTOR_COMMA
    );
    insert(
        "point_vector_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_VECTOR_CLOSE
    );
    insert(
        "tensor_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_TENSOR_COMMA
    );
    insert(
        "tensor_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_TENSOR_CLOSE
    );
    insert(
        "point_tensor_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_TENSOR_COMMA
    );
    insert(
        "point_tensor_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_TENSOR_CLOSE
    );
    insert(
        "symmTensor_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_YTENSOR_COMMA
    );
    insert(
        "symmTensor_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_YTENSOR_CLOSE
    );
    insert(
        "point_symmTensor_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_YTENSOR_COMMA
    );
    insert(
        "point_symmTensor_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_YTENSOR_CLOSE
    );
    insert(
        "sphericalTensor_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_HTENSOR_COMMA
    );
    insert(
        "sphericalTensor_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_HTENSOR_CLOSE
    );
    insert(
        "point_sphericalTensor_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_HTENSOR_COMMA
    );
    insert(
        "point_sphericalTensor_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_HTENSOR_CLOSE
    );
    insert(
        "logical_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_LOGICAL_COMMA
    );
    insert(
        "logical_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_FACE_LOGICAL_CLOSE
    );
    insert(
        "point_logical_SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_LOGICAL_COMMA
    );
    insert(
        "point_logical_CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_POINT_LOGICAL_CLOSE
    );

    insert(
        "CL",
        parserFaPatch::FaPatchValueExpressionParser::token::START_CLOSE_ONLY
    );
    insert(
        "SC",
        parserFaPatch::FaPatchValueExpressionParser::token::START_COMMA_ONLY
    );
}


const FaPatchValueExpressionDriver::SymbolTable<FaPatchValueExpressionDriver> &FaPatchValueExpressionDriver::symbolTable()
{
    static SymbolTable<FaPatchValueExpressionDriver> actualTable;

    return actualTable;
}

int FaPatchValueExpressionDriver::startupSymbol(const word &name) {
    return symbolTable()[name];
}


autoPtr<CommonPluginFunction> FaPatchValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        FaPatchValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool FaPatchValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return FaPatchValuePluginFunction::exists(
        *this,
        name
    );
}

tmp<scalarField> FaPatchValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!faceSize) {
        Pout << "Expected size: " << size
            << " Face size: " << faceSize << endl;

        FatalErrorIn("FaPatchValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(mag(makeEdgeLengthField()));
}

// ************************************************************************* //

} // namespace
