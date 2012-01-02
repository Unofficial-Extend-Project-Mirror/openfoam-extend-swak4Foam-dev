//  ICE Revision: $Id$ 

#include "FieldValueExpressionDriver.H"
#include <Random.H>
#include <wallDist.H>
#include <nearWallDist.H>
#include <dimensionedVector.H>
#include "cellSet.H"
#include "faceSet.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(FieldValueExpressionDriver, 0);

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FieldValueExpressionDriver, dictionary, internalField);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FieldValueExpressionDriver, idName, internalField);

FieldValueExpressionDriver::FieldValueExpressionDriver (
    const string& time,
    const Time& runTime,
    const fvMesh &mesh,
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
    : CommonValueExpressionDriver(
        cacheReadFields,
        searchInMemory,
        searchOnDisc        
    ),
      time_(time),
      mesh_(mesh),
      runTime_(runTime),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
}

FieldValueExpressionDriver::FieldValueExpressionDriver (
    const word &id,
    const fvMesh &mesh
)
    : CommonValueExpressionDriver(
        false,
        true,
        false        
    ),
      time_(""),
      mesh_(mesh),
      runTime_(mesh.time()),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
}
FieldValueExpressionDriver::FieldValueExpressionDriver (
    const FieldValueExpressionDriver &orig
)
    : CommonValueExpressionDriver(orig),
      time_(orig.time_),
      mesh_(orig.mesh_),
      runTime_(orig.runTime_),
      typ_(orig.typ_),
      isLogical_(orig.isLogical_),
      isSurfaceField_(orig.isSurfaceField_),
      resultDimension_(orig.resultDimension_)
{
}

FieldValueExpressionDriver::FieldValueExpressionDriver (
    const fvMesh &mesh,
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
    : CommonValueExpressionDriver(
        cacheReadFields,
        searchInMemory,
        searchOnDisc        
    ),
      time_(""),
      mesh_(mesh),
      runTime_(mesh.time()),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
}

FieldValueExpressionDriver::FieldValueExpressionDriver (
    const dictionary &dict,
    const fvMesh &mesh
)
    : CommonValueExpressionDriver(dict),
      time_(""),
      mesh_(mesh),
      runTime_(mesh.time()),
      typ_("nothing"),
      isLogical_(false),
      isSurfaceField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
    if(dict.found("dimensions")) {
        resultDimension_.reset(dimensionSet(dict.lookup("dimensions")));
    }
}

FieldValueExpressionDriver::~FieldValueExpressionDriver ()
{
}

void FieldValueExpressionDriver::parse (const std::string &f)
{
    content_ = f;
    scan_begin ();
    parserField::FieldValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing_);
    parser.parse ();
    scan_end ();
}


bool FieldValueExpressionDriver::isCellSet(const string &name)
{
    if(getTypeOfSet(name)=="cellSet") {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isCellZone(const string &name)
{
    if(mesh_.cellZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isFaceSet(const string &name)
{
    if(getTypeOfSet(name)=="faceSet") {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isFaceZone(const string &name)
{
    if(mesh_.faceZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

volScalarField *FieldValueExpressionDriver::makeModuloField(
    const volScalarField &a,
    const volScalarField &b)
{
    volScalarField *result_=makeConstantField<volScalarField>(0.);

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

    result_->correctBoundaryConditions();

    return result_;
}

volScalarField *FieldValueExpressionDriver::makeRandomField(label seed)
{
    volScalarField *f=makeConstantField<volScalarField>(0.);

    autoPtr<scalarField> rField(CommonValueExpressionDriver::makeRandomField(seed));
    f->internalField()=rField();

    f->correctBoundaryConditions();

    return f;
}

volScalarField *FieldValueExpressionDriver::makeCellIdField()
{
    volScalarField *f=makeConstantField<volScalarField>(0.);

    forAll(*f,cellI) {
        (*f)[cellI]=scalar(cellI);
    }

    f->correctBoundaryConditions();

    return f;
}

volScalarField *FieldValueExpressionDriver::makeGaussRandomField(label seed)
{
    volScalarField *f=makeConstantField<volScalarField>(0.);

    autoPtr<scalarField> rField(CommonValueExpressionDriver::makeGaussRandomField(seed));
    f->internalField()=rField();

    f->correctBoundaryConditions();

    return f;
}

volVectorField *FieldValueExpressionDriver::makePositionField()
{
    dimensionSet nullDim(0,0,0,0,0);
    volVectorField *f=new volVectorField(
        IOobject
        (
            "pos",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        vector(0,0,0)
    );
    f->dimensions().reset(mesh_.C().dimensions());
    *f=mesh_.C();
    f->dimensions().reset(nullDim);

    f->correctBoundaryConditions();

    return f;
}

surfaceVectorField *FieldValueExpressionDriver::makeFacePositionField()
{
    dimensionSet nullDim(0,0,0,0,0);
    surfaceVectorField *f=new surfaceVectorField(
        IOobject
        (
            "fpos",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        vector(0,0,0)
    );
    f->dimensions().reset(mesh_.Cf().dimensions());
    *f=mesh_.Cf();
    f->dimensions().reset(nullDim);

    return f;
}

surfaceVectorField *FieldValueExpressionDriver::makeFaceProjectionField()
{

    dimensionSet nullDim(0,0,0,0,0);
    surfaceVectorField *f=new surfaceVectorField(
        IOobject
        (
            "fproj",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        vector(0,0,0)
    );
    f->dimensions().reset(mesh_.Cf().dimensions());

    vector fmin(0,0,0);
    vector fmax(0,0,0);
    
    forAll(*f,faceI)
    {
        const face &fProp = mesh_.faces()[faceI];
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
    forAll(mesh_.boundaryMesh(),patchI)
    {
        labelList cNumbers = mesh_.boundaryMesh()[patchI].faceCells();
        fvsPatchVectorField & fFace = f->boundaryField()[patchI];
        
        forAll(fFace,faceI)
        {
            const cell & cProp(mesh_.cells()[cNumbers[faceI]]);

            forAll(cProp,cJ)
            {
                const label patchID = mesh_.boundaryMesh().whichPatch(cProp[cJ]);

                if (patchID == patchI)
                {
                    const face & fProp = mesh_.faces()[cProp[cJ]];

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

surfaceVectorField *FieldValueExpressionDriver::makeFaceField()
{
    dimensionSet nullDim(0,0,0,0,0);
    surfaceVectorField *f=new surfaceVectorField(
        IOobject
        (
            "face",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        vector(0,0,0)
    );
    f->dimensions().reset(mesh_.Sf().dimensions());
    *f=mesh_.Sf();
    f->dimensions().reset(nullDim);

    return f;
}

surfaceScalarField *FieldValueExpressionDriver::makeAreaField()
{
    dimensionSet nullDim(0,0,0,0,0);
    surfaceScalarField *f=new surfaceScalarField(
        IOobject
        (
            "face",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );
    f->dimensions().reset(mesh_.magSf().dimensions());
    *f=mesh_.magSf();
    f->dimensions().reset(nullDim);

    return f;
}

volScalarField *FieldValueExpressionDriver::makeVolumeField()
{
    volScalarField *f=new volScalarField(
        IOobject
        (
            "vol",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );
    const scalarField &V=mesh_.V();

    forAll(*f,cellI) {
        (*f)[cellI]=V[cellI];
    }

    f->correctBoundaryConditions();

    return f;
}

volScalarField *FieldValueExpressionDriver::makeDistanceField()
{
    dimensionSet nullDim(0,0,0,0,0);
    volScalarField *f=new volScalarField(
        IOobject
        (
            "dist",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );
    f->dimensions().reset(mesh_.C().dimensions());
    wallDist dist(mesh_);
    *f=dist;
    f->dimensions().reset(nullDim);

    f->correctBoundaryConditions();

    return f;
}

volScalarField *FieldValueExpressionDriver::makeNearDistanceField()
{
    dimensionSet nullDim(0,0,0,0,0);
    volScalarField *f=new volScalarField(
        IOobject
        (
            "dist",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        0.,
        "fixedValue"
    );
    f->dimensions().reset(mesh_.C().dimensions());
    nearWallDist dist(mesh_);
    f->boundaryField()==dist;
    f->dimensions().reset(nullDim);

    f->correctBoundaryConditions();

    return f;
}

volScalarField *FieldValueExpressionDriver::makeRDistanceField(const volVectorField& r)
{
    dimensionSet nullDim(0,0,0,0,0);
    volScalarField *f=new volScalarField(
        IOobject
        (
            "rdist",
            time(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );

    forAll(*f,cellI) {
        (*f)[cellI]=mag(mesh_.C()[cellI] - r[cellI]);
    }

    f->correctBoundaryConditions();

    return f;
}

volScalarField *FieldValueExpressionDriver::makeCellSetField(const string &name)
{
  volScalarField *f=makeConstantField<volScalarField>(0);

  IOobject head 
      (
          name,
          time(),
          polyMesh::meshSubDir/"sets",
          mesh_,
          IOobject::MUST_READ,
          IOobject::NO_WRITE
      );
  
  if(!head.headerOk()) {;
      head=IOobject 
          (
              name,
              "constant",
              polyMesh::meshSubDir/"sets",
              mesh_,
              IOobject::MUST_READ,
              IOobject::NO_WRITE
          );
      head.headerOk();
  }

  cellSet cs(head);
  labelList cells(cs.toc());

  forAll(cells,cellI) {
    (*f)[cells[cellI]]=1.;
  }

  f->correctBoundaryConditions();

  return f;
}

surfaceScalarField *FieldValueExpressionDriver::makeFaceSetField(const string &name)
{
  surfaceScalarField *f=makeConstantField<surfaceScalarField>(0);

  IOobject head 
      (
          name,
          time(),
          polyMesh::meshSubDir/"sets",
          mesh_,
          IOobject::MUST_READ,
          IOobject::NO_WRITE
      );
  
  if(!head.headerOk()) {;
      head=IOobject 
          (
              name,
              "constant",
              polyMesh::meshSubDir/"sets",
              mesh_,
              IOobject::MUST_READ,
              IOobject::NO_WRITE
          );
      head.headerOk();
  }

  faceSet cs(head);
  labelList faces(cs.toc());

  forAll(faces,faceI) {
    (*f)[faces[faceI]]=1.;
  }

  return f;
}

volScalarField *FieldValueExpressionDriver::makeCellZoneField(const string &name)
{
  volScalarField *f=makeConstantField<volScalarField>(0);
  label zoneID=mesh_.cellZones().findZoneID(name);

  const cellZone &zone=mesh_.cellZones()[zoneID];

  forAll(zone,ind) {
      label cellI=zone[ind];
      (*f)[cellI]=1.;
  }

  f->correctBoundaryConditions();

  return f;
}

surfaceScalarField *FieldValueExpressionDriver::makeFaceZoneField(const string &name)
{
  surfaceScalarField *f=makeConstantField<surfaceScalarField>(0);
  label zoneID=mesh_.faceZones().findZoneID(name);

  const faceZone &zone=mesh_.faceZones()[zoneID];

  forAll(zone,ind) {
      label faceI=zone[ind];
      (*f)[faceI]=1.;
  }

  return f;
}

volVectorField *FieldValueExpressionDriver::makeVectorField
(
    volScalarField *x,
    volScalarField *y,
    volScalarField *z
) {
    volVectorField *f=makeConstantField<volVectorField>(vector(0,0,0));

    forAll(*f,cellI) {
        (*f)[cellI]=vector((*x)[cellI],(*y)[cellI],(*z)[cellI]);
    }

    f->correctBoundaryConditions();

    return f;
}

volTensorField *FieldValueExpressionDriver::makeTensorField
(
    volScalarField *xx,volScalarField *xy,volScalarField *xz,
    volScalarField *yx,volScalarField *yy,volScalarField *yz,
    volScalarField *zx,volScalarField *zy,volScalarField *zz
) {
    volTensorField *f=makeConstantField<volTensorField>(tensor(0,0,0,0,0,0,0,0,0));

    forAll(*f,cellI) {
        (*f)[cellI]=tensor(
            (*xx)[cellI],(*xy)[cellI],(*xz)[cellI],
            (*yx)[cellI],(*yy)[cellI],(*yz)[cellI],
            (*zx)[cellI],(*zy)[cellI],(*zz)[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

volSymmTensorField *FieldValueExpressionDriver::makeSymmTensorField
(
    volScalarField *xx,volScalarField *xy,volScalarField *xz,
    volScalarField *yy,volScalarField *yz,
    volScalarField *zz
) {
    volSymmTensorField *f=makeConstantField<volSymmTensorField>(symmTensor(0,0,0,0,0,0));

    forAll(*f,cellI) {
        (*f)[cellI]=symmTensor(
            (*xx)[cellI],(*xy)[cellI],(*xz)[cellI],
            (*yy)[cellI],(*yz)[cellI],
            (*zz)[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

volSphericalTensorField *FieldValueExpressionDriver::makeSphericalTensorField
(
    volScalarField *xx
) {
    volSphericalTensorField *f=makeConstantField<volSphericalTensorField>(sphericalTensor(0));

    forAll(*f,cellI) {
        (*f)[cellI]=sphericalTensor(
            (*xx)[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

surfaceVectorField *FieldValueExpressionDriver::makeSurfaceVectorField
(
    surfaceScalarField *x,
    surfaceScalarField *y,
    surfaceScalarField *z
)
{
    surfaceVectorField *f=makeConstantField<surfaceVectorField>(vector(0,0,0));

    forAll(*f,faceI) {
        (*f)[faceI]=vector((*x)[faceI],(*y)[faceI],(*z)[faceI]);
    }

    return f;
}

surfaceTensorField *FieldValueExpressionDriver::makeSurfaceTensorField
(
    surfaceScalarField *xx,surfaceScalarField *xy,surfaceScalarField *xz,
    surfaceScalarField *yx,surfaceScalarField *yy,surfaceScalarField *yz,
    surfaceScalarField *zx,surfaceScalarField *zy,surfaceScalarField *zz
) {
    surfaceTensorField *f=makeConstantField<surfaceTensorField>(tensor(0,0,0,0,0,0,0,0,0));

    forAll(*f,faceI) {
        (*f)[faceI]=tensor(
            (*xx)[faceI],(*xy)[faceI],(*xz)[faceI],
            (*yx)[faceI],(*yy)[faceI],(*yz)[faceI],
            (*zx)[faceI],(*zy)[faceI],(*zz)[faceI]
        );
    }

    return f;
}

surfaceSymmTensorField *FieldValueExpressionDriver::makeSurfaceSymmTensorField
(
    surfaceScalarField *xx,surfaceScalarField *xy,surfaceScalarField *xz,
    surfaceScalarField *yy,surfaceScalarField *yz,
    surfaceScalarField *zz
) {
    surfaceSymmTensorField *f=makeConstantField<surfaceSymmTensorField>(symmTensor(0,0,0,0,0,0));

    forAll(*f,faceI) {
        (*f)[faceI]=symmTensor(
            (*xx)[faceI],(*xy)[faceI],(*xz)[faceI],
            (*yy)[faceI],(*yz)[faceI],
            (*zz)[faceI]
        );
    }

    return f;
}

surfaceSphericalTensorField *FieldValueExpressionDriver::makeSurfaceSphericalTensorField
(
    surfaceScalarField *xx
) {
    surfaceSphericalTensorField *f=makeConstantField<surfaceSphericalTensorField>(sphericalTensor(0));

    forAll(*f,faceI) {
        (*f)[faceI]=sphericalTensor(
            (*xx)[faceI]
        );
    }

    return f;
}

const word FieldValueExpressionDriver::time() const
{
    if(time_!="") {
        return time_;
    } else {
        return CommonValueExpressionDriver::time();
    }
}

} // end namespace
