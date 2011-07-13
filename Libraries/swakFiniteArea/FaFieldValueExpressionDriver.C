//  ICE Revision: $Id$ 

#include "FaFieldValueExpressionDriver.H"
#include <Random.H>
#include <dimensionedVector.H>
#include "zeroGradientFaPatchFields.H"
#include "fixedValueFaPatchFields.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

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
      typ_(NO_TYPE),
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
      typ_(NO_TYPE),
      resultDimension_(0,0,0,0,0,0,0)
{
}

FaFieldValueExpressionDriver::FaFieldValueExpressionDriver (
    const dictionary &dict,
    const fvMesh &mesh
)
    : FaCommonValueExpressionDriver(dict),
      mesh_(faRegionMesh(regionMesh(dict,mesh))),
      typ_(NO_TYPE),
      resultDimension_(0,0,0,0,0,0,0)
{
    if(dict.found("dimensions")) {
        resultDimension_.reset(dimensionSet(dict.lookup("dimensions")));
    }
}

FaFieldValueExpressionDriver::~FaFieldValueExpressionDriver ()
{
}

void FaFieldValueExpressionDriver::setScalarResult(areaScalarField *r) {
    if(debug) {
        Info << "FaFieldValueExpressionDriver::setScalarResult(areaScalarField *r)" << endl;
    }

    sresult_.reset(r);

    if(!resultDimension_.dimensionless()) {
        sresult_->dimensions().reset(resultDimension_);
    }
    typ_=SCALAR_TYPE;
    result_.setResult(sresult_->internalField());
}

void FaFieldValueExpressionDriver::setLogicalResult(areaScalarField *r) {
    if(debug) {
        Info << "FaFieldValueExpressionDriver::setLogicalResult(areaScalarField *r)" << endl;
    }

    sresult_.reset(r);

    typ_=LOGICAL_TYPE;
    result_.setResult(sresult_->internalField());
}

void FaFieldValueExpressionDriver::setVectorResult(areaVectorField *r) {
    if(debug) {
        Info << "FaFieldValueExpressionDriver::setVectorResult(areaVectorField *r)" << endl;
    }

    vresult_.reset(r);

    if(!resultDimension_.dimensionless()) {
        vresult_->dimensions().reset(resultDimension_);
    }
    typ_=VECTOR_TYPE;
    result_.setResult(vresult_->internalField());
}

void FaFieldValueExpressionDriver::parse (const std::string &f)
{
    content_ = f;
    if(debug) {
        Info << "FaField-parsing: " << content_ << endl;
    }
    scan_begin ();
    parserFaField::FaFieldValueExpressionParser parser (*this);
    if(debug) {
        Info << "FaFieldTrace: " << trace_parsing_ << endl;
        trace_parsing_=1;
    }
    parser.set_debug_level (trace_parsing_);
    parser.parse ();
    scan_end ();
}

areaScalarField *FaFieldValueExpressionDriver::makeModuloField(
    const areaScalarField &a,
    const areaScalarField &b)
{
    areaScalarField *result_=makeConstantField<areaScalarField>(0.);

    forAll(*result_,cellI) {
        scalar val=fmod(a[cellI],b[cellI]);

        if(fabs(val)>(b[cellI]/2)) {
            if(val>0) {
                val-=b[cellI];
            } else {
                val+=b[cellI];
            }
        }

        (*result_)[cellI]=val;
    }

    return result_;
}

areaScalarField *FaFieldValueExpressionDriver::makeRandomField(label seed)
{
    areaScalarField *f=makeConstantField<areaScalarField>(0.);

    autoPtr<scalarField> rField(CommonValueExpressionDriver::makeRandomField(seed));
    f->internalField()=rField();

    return f;
}

areaScalarField *FaFieldValueExpressionDriver::makeCellIdField()
{
    areaScalarField *f=makeConstantField<areaScalarField>(0.);

    forAll(*f,cellI) {
        (*f)[cellI]=scalar(cellI);
    }

    return f;
}

areaScalarField *FaFieldValueExpressionDriver::makeGaussRandomField(label seed)
{
    areaScalarField *f=makeConstantField<areaScalarField>(0.);

    autoPtr<scalarField> rField(CommonValueExpressionDriver::makeGaussRandomField(seed));
    f->internalField()=rField();

    return f;
}

areaVectorField *FaFieldValueExpressionDriver::makePositionField()
{
    dimensionSet nullDim(0,0,0,0,0);
    areaVectorField *f=new areaVectorField(
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
    );
    f->dimensions().reset(mesh_.areaCentres().dimensions());
    *f=mesh_.areaCentres();
    f->dimensions().reset(nullDim);
    return f;
}

edgeVectorField *FaFieldValueExpressionDriver::makeEdgePositionField()
{
    dimensionSet nullDim(0,0,0,0,0);
    edgeVectorField *f=new edgeVectorField(
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
    );
    f->dimensions().reset(mesh_.edgeCentres().dimensions());
    *f=mesh_.edgeCentres();
    f->dimensions().reset(nullDim);
    return f;
}

edgeVectorField *FaFieldValueExpressionDriver::makeEdgeProjectionField()
{

    dimensionSet nullDim(0,0,0,0,0);
    edgeVectorField *f=new edgeVectorField(
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
    );
    f->dimensions().reset(mesh_.edgeCentres().dimensions());

    vector fmin(0,0,0);
    vector fmax(0,0,0);
    
    forAll(*f,faceI)
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
                    fmin.component(compI) = mesh_.points()[fProp[pointI]].component(compI);
                }
                if(
                    mesh_.points()[fProp[pointI]].component(compI) 
                    > 
                    fmax.component(compI)
                ) {
                    fmax.component(compI) = mesh_.points()[fProp[pointI]].component(compI);
                }
            }
        }
        (*f)[faceI] = fmax - fmin;
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
                                fmin.component(compI) = mesh_.points()[fProp[pointI]].component(compI);
                            }
                            if(
                                mesh_.points()[fProp[pointI]].component(compI) 
                                >
                                fmax.component(compI)
                            ) {
                                fmax.component(compI) = mesh_.points()[fProp[pointI]].component(compI);
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

edgeVectorField *FaFieldValueExpressionDriver::makeEdgeField()
{
    dimensionSet nullDim(0,0,0,0,0);
    edgeVectorField *f=new edgeVectorField(
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
    );
    f->dimensions().reset(mesh_.Le().dimensions());
    *f=mesh_.Le();
    f->dimensions().reset(nullDim);
    return f;
}

edgeScalarField *FaFieldValueExpressionDriver::makeLengthField()
{
    dimensionSet nullDim(0,0,0,0,0);
    edgeScalarField *f=new edgeScalarField(
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
    );
    f->dimensions().reset(mesh_.magLe().dimensions());
    *f=mesh_.magLe();
    f->dimensions().reset(nullDim);
    return f;
}

areaScalarField *FaFieldValueExpressionDriver::makeAreaField()
{
    areaScalarField *f=new areaScalarField(
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
    );
    const scalarField &V=mesh_.S();

    forAll(*f,cellI) {
        (*f)[cellI]=V[cellI];
    }

    return f;
}

areaScalarField *FaFieldValueExpressionDriver::makeRDistanceField(const areaVectorField& r)
{
    dimensionSet nullDim(0,0,0,0,0);
    areaScalarField *f=new areaScalarField(
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
    );

    forAll(*f,cellI) {
        (*f)[cellI]=mag(mesh_.areaCentres()[cellI] - r[cellI]);
    }

    return f;
}

areaVectorField *FaFieldValueExpressionDriver::makeVectorField
(
    areaScalarField *x,
    areaScalarField *y,
    areaScalarField *z
) {
    areaVectorField *f=makeConstantField<areaVectorField>(vector(0,0,0));

    forAll(*f,cellI) {
        (*f)[cellI]=vector((*x)[cellI],(*y)[cellI],(*z)[cellI]);
    }

    return f;
}

edgeVectorField *FaFieldValueExpressionDriver::makeEdgeVectorField
(
    edgeScalarField *x,
    edgeScalarField *y,
    edgeScalarField *z
)
{
    edgeVectorField *f=makeConstantField<edgeVectorField>(vector(0,0,0));

    forAll(*f,faceI) {
        (*f)[faceI]=vector((*x)[faceI],(*y)[faceI],(*z)[faceI]);
    }

    return f;
}

} // end namespace
