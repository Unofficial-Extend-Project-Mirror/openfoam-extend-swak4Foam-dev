/*----------------------- -*- C++ -*- ---------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description


Contributors/Copyright:
    2011-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FaFieldValueExpressionDriver.H"
#include "FaFieldValuePluginFunction.H"

#include <Random.H>
#include <dimensionedVector.H>
#include "zeroGradientFaPatchFields.H"
#include "fixedValueFaPatchFields.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

word FaFieldValueExpressionDriver::driverName_="internalFaField";

defineTypeNameAndDebug(FaFieldValueExpressionDriver, 0);

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaFieldValueExpressionDriver, dictionary, internalFaField);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaFieldValueExpressionDriver, idName, internalFaField);

FaFieldValueExpressionDriver::FaFieldValueExpressionDriver (
    const word &id,
    const fvMesh &mesh
)
    : FaCommonValueExpressionDriver(
        false,
        true,
        false
    ),
      mesh_(faRegionMesh(mesh)),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
}

FaFieldValueExpressionDriver::FaFieldValueExpressionDriver (
    const fvMesh &mesh,
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
    : FaCommonValueExpressionDriver(
        cacheReadFields,
        searchInMemory,
        searchOnDisc
    ),
      mesh_(faRegionMesh(mesh)),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
}

FaFieldValueExpressionDriver::FaFieldValueExpressionDriver (
    const FaFieldValueExpressionDriver &orig
)
    : FaCommonValueExpressionDriver(
	 orig
    ),
      mesh_(orig.mesh_),
      typ_(orig.typ_),
      isLogical_(orig.isLogical_),
      isSurfaceField_(orig.isSurfaceField_),
      resultDimension_(orig.resultDimension_)
{
}

FaFieldValueExpressionDriver::FaFieldValueExpressionDriver (
    const dictionary &dict,
    const fvMesh &mesh
)
    : FaCommonValueExpressionDriver(dict),
      mesh_(
          faRegionMesh(
              regionMesh(
                  dict,
                  mesh,
                  searchOnDisc()
              ))),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
    if(dict.found("dimensions")) {
        resultDimension_.reset(dimensionSet(dict.lookup("dimensions")));
    }
}

FaFieldValueExpressionDriver::~FaFieldValueExpressionDriver ()
{
}

void FaFieldValueExpressionDriver::parseInternal (int startToken)
{
    parserFaField::FaFieldValueExpressionParser parser (
        scanner_,
        *this,
        startToken,
        0
    );
    parser.set_debug_level (traceParsing());
    parser.parse ();
}

tmp<areaScalarField> FaFieldValueExpressionDriver::makeModuloField(
    const areaScalarField &a,
    const areaScalarField &b
) const
{
    tmp<areaScalarField> result(
        makeConstantField<areaScalarField>(0.)
    );

    forAll(result(),cellI) {
        scalar val=fmod(a[cellI],b[cellI]);

        if(fabs(val)>(b[cellI]/2)) {
            if(val>0) {
                val-=b[cellI];
            } else {
                val=b[cellI];
            }
        }

        result()[cellI]=val;
    }

    return result;
}

tmp<areaScalarField>
FaFieldValueExpressionDriver::makeRandomField(label seed) const
{
    tmp<areaScalarField> f(
        makeConstantField<areaScalarField>(0.)
    );

    f->internalField()=CommonValueExpressionDriver::makeRandomField(seed);

    return f;
}

tmp<areaScalarField> FaFieldValueExpressionDriver::makeCellIdField() const
{
    tmp<areaScalarField> f(
        makeConstantField<areaScalarField>(0.)
    );

    forAll(f(),cellI) {
        f()[cellI]=scalar(cellI);
    }

    return f;
}

tmp<areaScalarField> FaFieldValueExpressionDriver::makeGaussRandomField(
    label seed
) const
{
    tmp<areaScalarField> f(
        makeConstantField<areaScalarField>(0.)
    );

    f->internalField()=CommonValueExpressionDriver::makeGaussRandomField(seed);

    return f;
}

tmp<areaVectorField> FaFieldValueExpressionDriver::makePositionField() const
{
    dimensionSet nullDim(0,0,0,0,0);
    tmp<areaVectorField> f(
        new areaVectorField(
            IOobject
            (
                "pos",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            vector(0,0,0)
        )
    );
    f->dimensions().reset(mesh_.areaCentres().dimensions());
    f()=mesh_.areaCentres();
    f->dimensions().reset(nullDim);
    return f;
}

tmp<edgeVectorField>
FaFieldValueExpressionDriver::makeEdgePositionField() const
{
    dimensionSet nullDim(0,0,0,0,0);
    tmp<edgeVectorField> f(
        new edgeVectorField(
            IOobject
            (
                "fpos",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            vector(0,0,0)
        )
    );
    f->dimensions().reset(mesh_.edgeCentres().dimensions());
    f()=mesh_.edgeCentres();
    f->dimensions().reset(nullDim);
    return f;
}

tmp<edgeVectorField>
FaFieldValueExpressionDriver::makeEdgeProjectionField() const
{

    dimensionSet nullDim(0,0,0,0,0);
    tmp<edgeVectorField> f(
        new edgeVectorField(
            IOobject
            (
                "fproj",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            vector(0,0,0)
        )
    );
    f->dimensions().reset(mesh_.edgeCentres().dimensions());

    vector fmin(0,0,0);
    vector fmax(0,0,0);

    forAll(f(),faceI)
    {
        const edge &fProp = mesh_.edges()[faceI];
        fmin = mesh_.points()[fProp[0]];
        fmax = fmin;
        forAll(fProp,pointI)
        {
            forAll(mesh_.points()[0],compI)
            {
                if(
                    mesh_.points()[fProp[pointI]].component(compI)
                    <
                    fmin.component(compI)
                ) {
                    fmin.component(compI) =
                        mesh_.points()[fProp[pointI]].component(compI);
                }
                if(
                    mesh_.points()[fProp[pointI]].component(compI)
                    >
                    fmax.component(compI)
                ) {
                    fmax.component(compI) =
                        mesh_.points()[fProp[pointI]].component(compI);
                }
            }
        }
        f()[faceI] = fmax - fmin;
    }
    forAll(mesh_.boundary(),patchI)
    {
        labelList cNumbers = mesh_.boundary()[patchI].edgeFaces();
        faePatchVectorField & fFace = f->boundaryField()[patchI];

        forAll(fFace,faceI)
        {
            const face & cProp(mesh_.faces()[cNumbers[faceI]]);

            forAll(cProp,cJ)
            {
                const label patchID = mesh_.boundary().whichPatch(cProp[cJ]);

                if (patchID == patchI)
                {
                    const edge & fProp = mesh_.edges()[cProp[cJ]];

                    fmin = mesh_.points()[fProp[0]];
                    fmax = fmin;

                    forAll(fProp,pointI)
                    {
                        forAll(mesh_.points()[0],compI)
                        {
                            if(
                                mesh_.points()[fProp[pointI]].component(compI)
                                <
                                fmin.component(compI)
                            ) {
                                fmin.component(compI) =
                                    mesh_.points()[fProp[pointI]].component(compI);
                            }
                            if(
                                mesh_.points()[fProp[pointI]].component(compI)
                                >
                                fmax.component(compI)
                            ) {
                                fmax.component(compI) =
                                    mesh_.points()[fProp[pointI]].component(compI);
                            }
                        }
                    }
                    fFace[faceI] = fmax - fmin;
                }
            }
        }
    }

    f->dimensions().reset(nullDim);
    return f;
}

tmp<edgeVectorField> FaFieldValueExpressionDriver::makeEdgeField() const
{
    dimensionSet nullDim(0,0,0,0,0);
    tmp<edgeVectorField> f(
        new edgeVectorField(
            IOobject
            (
                "face",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            vector(0,0,0)
        )
    );
    f->dimensions().reset(mesh_.Le().dimensions());
    f()=mesh_.Le();
    f->dimensions().reset(nullDim);
    return f;
}

tmp<edgeScalarField> FaFieldValueExpressionDriver::makeLengthField() const
{
    dimensionSet nullDim(0,0,0,0,0);
    tmp<edgeScalarField> f(
        new edgeScalarField(
            IOobject
            (
                "face",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            0.
        )
    );
    f->dimensions().reset(mesh_.magLe().dimensions());
    f()=mesh_.magLe();
    f->dimensions().reset(nullDim);
    return f;
}

tmp<areaScalarField> FaFieldValueExpressionDriver::makeAreaField() const
{
    tmp<areaScalarField> f(
        new areaScalarField(
            IOobject
            (
                "area",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            0.
        )
    );
    const scalarField &V=mesh_.S();

    forAll(f(),cellI) {
        f()[cellI]=V[cellI];
    }

    return f;
}

tmp<areaScalarField> FaFieldValueExpressionDriver::makeRDistanceField(
    const areaVectorField& r
) const
{
    dimensionSet nullDim(0,0,0,0,0);
    tmp<areaScalarField> f(
        new areaScalarField(
            IOobject
            (
                "rdist",
                time(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            aMesh(),
            0.
        )
    );

    forAll(f(),cellI) {
        f()[cellI]=mag(mesh_.areaCentres()[cellI] - r[cellI]);
    }

    return f;
}

tmp<areaVectorField> FaFieldValueExpressionDriver::makeVectorField
(
    const areaScalarField &x,
    const areaScalarField &y,
    const areaScalarField &z
) {
    tmp<areaVectorField> f(
        makeConstantField<areaVectorField>(vector(0,0,0))
    );

    forAll(f(),cellI) {
        f()[cellI]=vector(x[cellI],y[cellI],z[cellI]);
    }

    return f;
}

tmp<edgeVectorField> FaFieldValueExpressionDriver::makeEdgeVectorField
(
    const edgeScalarField &x,
    const edgeScalarField &y,
    const edgeScalarField &z
)
{
    tmp<edgeVectorField> f(
        makeConstantField<edgeVectorField>(vector(0,0,0))
    );

    forAll(f(),faceI) {
        f()[faceI]=vector(x[faceI],y[faceI],z[faceI]);
    }

    return f;
}

tmp<areaTensorField> FaFieldValueExpressionDriver::makeTensorField
(
    const areaScalarField &xx,const areaScalarField &xy,const areaScalarField &xz,
    const areaScalarField &yx,const areaScalarField &yy,const areaScalarField &yz,
    const areaScalarField &zx,const areaScalarField &zy,const areaScalarField &zz
) {
    tmp<areaTensorField> f(
        makeConstantField<areaTensorField>(tensor(0,0,0,0,0,0,0,0,0))
    );

    forAll(f(),cellI) {
        f()[cellI]=tensor(
            xx[cellI],xy[cellI],xz[cellI],
            yx[cellI],yy[cellI],yz[cellI],
            zx[cellI],zy[cellI],zz[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<areaSymmTensorField> FaFieldValueExpressionDriver::makeSymmTensorField
(
    const areaScalarField &xx,const areaScalarField &xy,const areaScalarField &xz,
    const areaScalarField &yy,const areaScalarField &yz,
    const areaScalarField &zz
) {
    tmp<areaSymmTensorField> f(
        makeConstantField<areaSymmTensorField>(symmTensor(0,0,0,0,0,0))
    );

    forAll(f(),cellI) {
        f()[cellI]=symmTensor(
            xx[cellI],xy[cellI],xz[cellI],
            yy[cellI],yz[cellI],
            zz[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<areaSphericalTensorField>
FaFieldValueExpressionDriver::makeSphericalTensorField
(
    const areaScalarField &xx
) {
    tmp<areaSphericalTensorField> f(
        makeConstantField<areaSphericalTensorField>(sphericalTensor(0))
    );

    forAll(f(),cellI) {
        f()[cellI]=sphericalTensor(
            xx[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<edgeTensorField> FaFieldValueExpressionDriver::makeEdgeTensorField
(
    const edgeScalarField &xx,const edgeScalarField &xy,const edgeScalarField &xz,
    const edgeScalarField &yx,const edgeScalarField &yy,const edgeScalarField &yz,
    const edgeScalarField &zx,const edgeScalarField &zy,const edgeScalarField &zz
) {
    tmp<edgeTensorField> f(
        makeConstantField<edgeTensorField>(tensor(0,0,0,0,0,0,0,0,0))
    );

    forAll(f(),faceI) {
        f()[faceI]=tensor(
            xx[faceI],xy[faceI],xz[faceI],
            yx[faceI],yy[faceI],yz[faceI],
            zx[faceI],zy[faceI],zz[faceI]
        );
    }

    return f;
}

tmp<edgeSymmTensorField> FaFieldValueExpressionDriver::makeEdgeSymmTensorField
(
    const edgeScalarField &xx,const edgeScalarField &xy,const edgeScalarField &xz,
    const edgeScalarField &yy,const edgeScalarField &yz,
    const edgeScalarField &zz
) {
    tmp<edgeSymmTensorField> f(
        makeConstantField<edgeSymmTensorField>(symmTensor(0,0,0,0,0,0))
    );

    forAll(f(),faceI) {
        f()[faceI]=symmTensor(
            xx[faceI],xy[faceI],xz[faceI],
            yy[faceI],yz[faceI],
            zz[faceI]
        );
    }

    return f;
}

tmp<edgeSphericalTensorField>
FaFieldValueExpressionDriver::makeEdgeSphericalTensorField
(
    const edgeScalarField &xx
) {
    tmp<edgeSphericalTensorField> f(
        makeConstantField<edgeSphericalTensorField>(sphericalTensor(0))
    );

    forAll(f(),faceI) {
        f()[faceI]=sphericalTensor(
            xx[faceI]
        );
    }

    return f;
}

template<>
FaFieldValueExpressionDriver::SymbolTable<FaFieldValueExpressionDriver>::SymbolTable()
:
StartupSymbols()
{
    // default value
    insert("",parserFaField::FaFieldValueExpressionParser::token::START_DEFAULT);

    insert(
        "areaScalarField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_SCALAR_COMMA
    );
    insert(
        "areaScalarField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_SCALAR_CLOSE
    );
    insert(
        "areaVectorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_VECTOR_COMMA
    );
    insert(
        "areaVectorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_VECTOR_CLOSE
    );
    insert(
        "areaTensorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_TENSOR_COMMA
    );
    insert(
        "areaTensorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_TENSOR_CLOSE
    );
    insert(
        "areaSymmTensorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_YTENSOR_COMMA
    );
    insert(
        "areaSymmTensorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_YTENSOR_CLOSE
    );
    insert(
        "areaSphericalTensorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_HTENSOR_COMMA
    );
    insert(
        "areaSphericalTensorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_HTENSOR_CLOSE
    );
    insert(
        "areaLogicalField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_LOGICAL_COMMA
    );
    insert(
        "areaLogicalField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_AREA_LOGICAL_CLOSE
    );

    insert(
        "edgeScalarField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_SCALAR_COMMA
    );
    insert(
        "edgeScalarField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_SCALAR_CLOSE
    );
    insert(
        "edgeVectorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_VECTOR_COMMA
    );
    insert(
        "edgeVectorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_VECTOR_CLOSE
    );
    insert(
        "edgeTensorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_TENSOR_COMMA
    );
    insert(
        "edgeTensorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_TENSOR_CLOSE
    );
    insert(
        "edgeSymmTensorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_YTENSOR_COMMA
    );
    insert(
        "edgeSymmTensorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_YTENSOR_CLOSE
    );
    insert(
        "edgeSphericalTensorField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_HTENSOR_COMMA
    );
    insert(
        "edgeSphericalTensorField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_HTENSOR_CLOSE
    );
    insert(
        "edgeLogicalField_SC",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_LOGICAL_COMMA
    );
    insert(
        "edgeLogicalField_CL",
        parserFaField::FaFieldValueExpressionParser::token::START_EDGE_LOGICAL_CLOSE
    );

    insert(
        "CL",
        parserFaField::FaFieldValueExpressionParser::token::START_CLOSE_ONLY
    );
    insert(
        "SC",
        parserFaField::FaFieldValueExpressionParser::token::START_COMMA_ONLY
    );
}

const FaFieldValueExpressionDriver::SymbolTable<FaFieldValueExpressionDriver> &FaFieldValueExpressionDriver::symbolTable()
{
    static SymbolTable<FaFieldValueExpressionDriver> actualTable;

    return actualTable;
}

int FaFieldValueExpressionDriver::startupSymbol(const word &name) {
    return symbolTable()[name];
}


autoPtr<CommonPluginFunction> FaFieldValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        FaFieldValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool FaFieldValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return FaFieldValuePluginFunction::exists(
        *this,
        name
    );
}

label FaFieldValueExpressionDriver::size() const
{
    return mesh_.nFaces();
}

label FaFieldValueExpressionDriver::pointSize() const
{
    return mesh_.nPoints();
}

const faMesh &FaFieldValueExpressionDriver::aMesh() const
{
    return mesh_;
}

const fvMesh &FaFieldValueExpressionDriver::mesh() const
{
    return dynamic_cast<const fvMesh&>(mesh_.thisDb());
}

tmp<scalarField> FaFieldValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!isFace) {
        Pout << "Expected size: " << size
            << " Face size: " << faceSize << endl;

        FatalErrorIn("FaFieldValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(
        new scalarField(
            mesh_.S()
        )
    );
}

} // end namespace
