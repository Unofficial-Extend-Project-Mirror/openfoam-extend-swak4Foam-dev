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
    2009-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
    2010 Marianne Mataln <mmataln@ice-sf>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "PatchValueExpressionDriver.H"
#include "PatchValuePluginFunction.H"

#include "FieldValueExpressionDriver.H"

#include "SubsetValueExpressionDriver.H"

#include "Random.H"

#include "addToRunTimeSelectionTable.H"

#include <nearWallDist.H>

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
#include "directMappedFvPatch.H"

#include "mapDistribute.H"

namespace Foam {
    // these typedefs should keep the difference between the 1.7 and the 2.1 code minimal
    typedef directMappedFvPatch mappedFvPatch;
    typedef directMappedPolyPatch mappedPolyPatch;
}

#else
#include "mappedFvPatch.H"
#endif

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
    patch_(orig.patch_),
    mappingInterpolationSchemes_(orig.mappingInterpolationSchemes_)
{
    if(debug) {
        Info << "PatchValueExpressionDriver - copy constructor" << endl;
    }
}

PatchValueExpressionDriver::PatchValueExpressionDriver(const fvPatch& patch)
:
    CommonValueExpressionDriver(),
    patch_(patch),
    mappingInterpolationSchemes_()
{
    if(debug) {
        Info << "PatchValueExpressionDriver - patch constructor" << endl;
    }
}

PatchValueExpressionDriver::PatchValueExpressionDriver(
    const dictionary& dict,
    const fvPatch& patch
)
:
    CommonValueExpressionDriver(dict),
    patch_(patch),
    mappingInterpolationSchemes_(dict.subOrEmptyDict("mappingInterpolation"))
{
    if(debug) {
        Info << "PatchValueExpressionDriver - patch+dict constructor" << endl;
    }
    if(!dict.isDict("mappingInterpolation")) {
        mappingInterpolationSchemes_.name()=
            dict.name()+"::mappingInterpolation";
    }
}

label getPatchID(const fvMesh &mesh,const word &name)
{
    label result=mesh.boundaryMesh().findPatchID(name);
    if(result<0) {
        FatalErrorIn("getPatchID(const fvMesh &mesh,const word &name)")
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
    ),
    mappingInterpolationSchemes_(dict.subOrEmptyDict("mappingInterpolation"))
{
    if(debug) {
        Info << "PatchValueExpressionDriver - dict+mesh constructor" << endl;
    }
    if(!dict.isDict("mappingInterpolation")) {
        mappingInterpolationSchemes_.name()=
            dict.name()+"::mappingInterpolation";
    }
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
    ),
    mappingInterpolationSchemes_()
{
    if(debug) {
        Info << "PatchValueExpressionDriver - id+mesh constructor" << endl;
    }
}

PatchValueExpressionDriver::PatchValueExpressionDriver(
    const fvPatch& patch,
    const PatchValueExpressionDriver& old
)
:
    CommonValueExpressionDriver(old),
    patch_(patch),
    mappingInterpolationSchemes_(old.mappingInterpolationSchemes_)
{
    if(debug) {
        Info << "PatchValueExpressionDriver - patch+driver constructor" << endl;
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

PatchValueExpressionDriver::~PatchValueExpressionDriver()
{
    if(debug) {
        Info << "~PatchValueExpressionDriver()" << endl;
    }
}


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

tmp<vectorField> PatchValueExpressionDriver::makePositionField()
{
    return tmp<vectorField>(
        new vectorField(patch_.Cf())
    );
}

tmp<vectorField> PatchValueExpressionDriver::makePointField()
{
    return tmp<vectorField>(
        new vectorField(patch_.patch().localPoints())
    );
}

tmp<vectorField> PatchValueExpressionDriver::makeFaceNormalField()
{
    return tmp<vectorField>(
        new vectorField(patch_.nf())
    );
}

tmp<vectorField> PatchValueExpressionDriver::makeFaceAreaField()
{
    return tmp<vectorField>(
        new vectorField(patch_.Sf())
    );
}

tmp<vectorField> PatchValueExpressionDriver::makeCellNeighbourField()
{
    return tmp<vectorField>(
        new vectorField(patch_.Cn())
    );
}

tmp<vectorField> PatchValueExpressionDriver::makeDeltaField()
{
    return tmp<vectorField>(
        new vectorField(patch_.delta())
    );
}

tmp<scalarField> PatchValueExpressionDriver::makeWeightsField()
{
    return tmp<scalarField>(
        new scalarField(patch_.weights())
    );
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

tmp<scalarField> PatchValueExpressionDriver::makeFaceIdField()
{
    tmp<scalarField> result(
        new scalarField(patch_.size())
    );
    forAll(result(),i) {
        result()[i]=i;
    }
    return result;
}

tmp<scalarField> PatchValueExpressionDriver::makeNearDistField()
{
    tmp<scalarField> result(
        new scalarField(patch_.size())
    );

    nearWallDist dist(this->mesh());
    result()=dist[patch_.index()];
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

template<>
HashPtrTable<interpolation<scalar> > &PatchValueExpressionDriver::interpolations<scalar>()
{
    return interpolationScalar_;
}

template<>
HashPtrTable<interpolation<vector> > &PatchValueExpressionDriver::interpolations<vector>()
{
    return interpolationVector_;
}

template<>
HashPtrTable<interpolation<tensor> > &PatchValueExpressionDriver::interpolations<tensor>()
{
    return interpolationTensor_;
}

template<>
HashPtrTable<interpolation<symmTensor> > &PatchValueExpressionDriver::interpolations<symmTensor>()
{
    return interpolationSymmTensor_;
}

template<>
HashPtrTable<interpolation<sphericalTensor> > &PatchValueExpressionDriver::interpolations<sphericalTensor>()
{
    return interpolationSphericalTensor_;
}

const word PatchValueExpressionDriver::getInterpolationScheme(const word &name)
{
    if(mappingInterpolationSchemes_.found(name)) {
        return word(mappingInterpolationSchemes_.lookup(name));
    } else if(mappingInterpolationSchemes_.found("default")) {
        WarningIn("PatchValueExpressionDriver::getInterpolationScheme(const word &name)")
            << "No entry for " << name << " in "
                << mappingInterpolationSchemes_.name()
                << ". Using 'default'"
                << endl;

        word scheme(word(mappingInterpolationSchemes_.lookup("default")));
        mappingInterpolationSchemes_.add(name,scheme);

        return scheme;
    } else {
        FatalErrorIn("PatchValueExpressionDriver::getInterpolationScheme(const word &name)")
            << "No entry for " << name << " or 'default' in "
                << mappingInterpolationSchemes_.name()
                << endl
                << exit(FatalError);
    }

    return word("nixDaGefunden");
}

autoPtr<ExpressionResult> PatchValueExpressionDriver::getRemoteResult(
    CommonValueExpressionDriver &otherDriver
)
{
    if(debug) {
        Info << "PatchValueExpressionDriver::getRemoteResult" << endl;
    }

    if(
        !isA<PatchValueExpressionDriver>(otherDriver)
        ||
        !isA<mappedFvPatch>(patch_)
    ) {
        if(debug) {
            Info << "Not mapped or not remote-patch -> uniform" << endl;
        }
        return CommonValueExpressionDriver::getRemoteResult(otherDriver);
    }

    //    const mappedPolyPatch &patch=dynamicCast<const mappedPolyPatch&>(
    const mappedPolyPatch &patch=dynamic_cast<const mappedPolyPatch&>(
        patch_.patch()
    );
    PatchValueExpressionDriver &driver=
        //        dynamicCast<PatchValueExpressionDriver&>(otherDriver);
        dynamic_cast<PatchValueExpressionDriver&>(otherDriver);

    if(
        patch.mode()!=mappedPatchBase::NEARESTPATCHFACE
        ||
        driver.patch().name()!=patch.samplePatch()
        ||
        driver.patch().boundaryMesh().mesh().name()!=patch.sampleRegion()
    ) {
        if(debug) {
            Info << "Not correct circumstances for mapping -> uniform" << endl;
        }
        return CommonValueExpressionDriver::getRemoteResult(otherDriver);
    }

    if(driver.result().isPoint()) {
        WarningIn("PatchValueExpressionDriver::getRemoteResult")
            << "Can not map point fields (though everyting else "
                << "is OK for mapping"
                << endl;
        return CommonValueExpressionDriver::getRemoteResult(otherDriver);
    }

    if(debug) {
        Info << "Mapping a result:" << driver.result() << endl;
    }

    if(driver.result().valueType()==pTraits<scalar>::typeName) {
        return autoPtr<ExpressionResult>(
            new ExpressionResult(
                mapField(
                    driver.result().getResult<scalar>(false)
                )()
            )
        );
    } else if(driver.result().valueType()==pTraits<vector>::typeName) {
        return autoPtr<ExpressionResult>(
            new ExpressionResult(
                mapField(
                    driver.result().getResult<vector>(false)
                )()
            )
        );
    } else if(driver.result().valueType()==pTraits<tensor>::typeName) {
        return autoPtr<ExpressionResult>(
            new ExpressionResult(
                mapField(
                    driver.result().getResult<tensor>(false)
                )()
            )
        );
    } else if(driver.result().valueType()==pTraits<symmTensor>::typeName) {
        return autoPtr<ExpressionResult>(
            new ExpressionResult(
                mapField(
                    driver.result().getResult<symmTensor>(false)
                )()
            )
        );
    } else if(driver.result().valueType()==pTraits<sphericalTensor>::typeName) {
        return autoPtr<ExpressionResult>(
            new ExpressionResult(
                mapField(
                    driver.result().getResult<sphericalTensor>(false)
                )()
            )
        );
    } else {
        FatalErrorIn("")
            << "Mapping for result type " << driver.result().valueType()
                << " undefined"
                << endl
                << exit(FatalError);
        return autoPtr<ExpressionResult>(); // this should never be reached
    }
}

tmp<scalarField> PatchValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!isFace) {
        Pout << "Expected size: " << size
            << " Face size: " << faceSize << endl;

        FatalErrorIn("PatchValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(new scalarField(patch().magSf()));
}

// ************************************************************************* //

} // namespace
