//  ICE Revision: $Id$ 

#include "FieldValueExpressionDriver.H"
#include <Random.H>
#include <wallDist.H>
#include <dimensionedVector.H>
#include "zeroGradientFvPatchFields.H"
#include "fixedValueFvPatchFields.H"
#include "wallFvPatch.H"
#include "cellSet.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(FieldValueExpressionDriver, 0);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FieldValueExpressionDriver, dictionary, internalField);

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
      result_(NULL),
      vresult_(NULL),
      typ_(NO_TYPE)
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
      time_(mesh.time().timeName()),
      mesh_(mesh),
      runTime_(mesh.time()),
      result_(NULL),
      vresult_(NULL),
      typ_(NO_TYPE)
{
}

FieldValueExpressionDriver::FieldValueExpressionDriver (
    const dictionary &dict,
    const fvMesh &mesh
)
    : CommonValueExpressionDriver(dict),
      time_(mesh.time().timeName()),
      mesh_(mesh),
      runTime_(mesh.time()),
      result_(NULL),
      vresult_(NULL),
      typ_(NO_TYPE)
{
}

FieldValueExpressionDriver::~FieldValueExpressionDriver ()
{
    if(result_) {
        delete result_;
    }
    if(vresult_) {
        delete vresult_;
    }
}

void FieldValueExpressionDriver::setScalarResult(volScalarField *r) {
    result_=r;
    typ_=SCALAR_TYPE;
}

void FieldValueExpressionDriver::setLogicalResult(volScalarField *r) {
    result_=r;
    typ_=LOGICAL_TYPE;
}

void FieldValueExpressionDriver::setVectorResult(volVectorField *r) {
    vresult_=r;
    typ_=VECTOR_TYPE;
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

string FieldValueExpressionDriver::getTypeOfField(const string &name)
{
    IOobject f 
        (
            name,
            time_,
            mesh_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
    f.headerOk();

    return f.headerClassName();
}

string FieldValueExpressionDriver::getTypeOfSet(const string &name)
{
    IOobject f 
        (
            name,
            time_,
            polyMesh::meshSubDir/"sets",
	    mesh_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
    
    if(f.headerOk()) {;
        return f.headerClassName();
    } else {
        Info << "No set " << name << " at t=" << time_ 
            << " falling back to 'constant'" << endl;
        f=IOobject 
        (
            name,
            "constant",
            polyMesh::meshSubDir/"sets",
	    mesh_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
        f.headerOk();
        return f.headerClassName();
    }
}

volScalarField *FieldValueExpressionDriver::makeModuloField(
    const volScalarField &a,
    const volScalarField &b)
{
    volScalarField *result_=makeScalarField(0.);

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

volScalarField *FieldValueExpressionDriver::makeRandomField()
{
    volScalarField *f=makeScalarField(0.);
    Random rand(65);

    forAll(*f,cellI) {
        (*f)[cellI]=rand.scalar01();
    }

    return f;
}

volScalarField *FieldValueExpressionDriver::makeCellIdField()
{
    volScalarField *f=makeScalarField(0.);

    forAll(*f,cellI) {
        (*f)[cellI]=scalar(cellI);
    }

    return f;
}

volScalarField *FieldValueExpressionDriver::makeGaussRandomField()
{
    volScalarField *f=makeScalarField(0.);
    Random rand(65);

    forAll(*f,cellI) {
        (*f)[cellI]=rand.GaussNormal();
    }

    return f;
}

volVectorField *FieldValueExpressionDriver::makePositionField()
{
    dimensionSet nullDim(0,0,0,0,0);
    volVectorField *f=new volVectorField(
        IOobject
        (
            "pos",
            time_,
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
    return f;
}

surfaceVectorField *FieldValueExpressionDriver::makeFacePositionField()
{
    dimensionSet nullDim(0,0,0,0,0);
    surfaceVectorField *f=new surfaceVectorField(
        IOobject
        (
            "fpos",
            time_,
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
            time_,
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
            time_,
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
            time_,
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
            time_,
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

    return f;
}

volScalarField *FieldValueExpressionDriver::makeDistanceField()
{
    dimensionSet nullDim(0,0,0,0,0);
    volScalarField *f=new volScalarField(
        IOobject
        (
            "dist",
            time_,
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
    return f;

}

volScalarField *FieldValueExpressionDriver::makeRDistanceField(const volVectorField& r)
{
    dimensionSet nullDim(0,0,0,0,0);
    volScalarField *f=new volScalarField(
        IOobject
        (
            "rdist",
            time_,
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

    return f;
}

volScalarField *FieldValueExpressionDriver::makeScalarField(const scalar &val)
{
    std::ostringstream buff;
    buff << "constantScalar" << val;

    volScalarField *f=new volScalarField(
        IOobject
        (
            buff.str(),
            time_,
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        val,
        "zeroGradient"
    );

    return f;
}

surfaceScalarField *FieldValueExpressionDriver::makeSurfaceScalarField
(
    const scalar &val
){
    std::ostringstream buff;
    buff << "constantScalar" << val;

    surfaceScalarField *f=new surfaceScalarField(
        IOobject
        (
            buff.str(),
            time_,
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        val,
        "fixedValue"
    );

    return f;
}

volScalarField *FieldValueExpressionDriver::makeCellSetField(const string &name)
{
  volScalarField *f=makeScalarField(0);

  IOobject head 
      (
          name,
          time_,
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

  return f;
}

volScalarField *FieldValueExpressionDriver::makeCellZoneField(const string &name)
{
  volScalarField *f=makeScalarField(0);
  label zoneID=mesh_.cellZones().findZoneID(name);

  const cellZone &zone=mesh_.cellZones()[zoneID];

  forAll(zone,ind) {
      label cellI=zone[ind];
      (*f)[cellI]=1.;
  }

  return f;
}

volVectorField *FieldValueExpressionDriver::makeVectorField(const vector &vec)
{
    std::ostringstream buff;
    buff << "constantVector" << vec.x() << "_" << vec.y() << "_" << vec.z() ;

    volVectorField *f=new volVectorField(
        IOobject
        (
            buff.str(),
            time_,
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        vec,
        "zeroGradient"
    );

    return f;
}

volVectorField *FieldValueExpressionDriver::makeVectorField
(
    volScalarField *x,
    volScalarField *y,
    volScalarField *z
) {
    volVectorField *f=makeVectorField(vector(0,0,0));

    forAll(*f,cellI) {
        (*f)[cellI]=vector((*x)[cellI],(*y)[cellI],(*z)[cellI]);
    }

    return f;
}

surfaceVectorField *FieldValueExpressionDriver::makeSurfaceVectorField
(
    const vector &vec
){
    std::ostringstream buff;
    buff << "constantVector" << vec.x() << "_" << vec.y() << "_" << vec.z() ;

    surfaceVectorField *f=new surfaceVectorField(
        IOobject
        (
            buff.str(),
            time_,
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        vec,
        "fixedValue"
    );

    return f;
}

surfaceVectorField *FieldValueExpressionDriver::makeSurfaceVectorField
(
    surfaceScalarField *x,
    surfaceScalarField *y,
    surfaceScalarField *z
)
{
    surfaceVectorField *f=makeSurfaceVectorField(vector(0,0,0));

    forAll(*f,faceI) {
        (*f)[faceI]=vector((*x)[faceI],(*y)[faceI],(*z)[faceI]);
    }

    return f;
}

template<class T>
void FieldValueExpressionDriver::makePatches
(
    GeometricField<T,fvPatchField,volMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,fvPatchField,volMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<fvPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const fvPatch &patch=bf[patchI].patch();

    bool isValuePatch=false;
    forAll(fixedPatches,i) {
      if(fixedPatches[i]==patch.name()) {
	isValuePatch=true;
      }
    }

    if(
        (
            !keepPatches 
            || 
            isValuePatch
        ) 
        && 
        (
            typeid(patch)==typeid(wallFvPatch)
            || 
            typeid(patch)==typeid(fvPatch
            )
        )
    ) {
        if(isValuePatch){
            bfNew[patchI]=new fixedValueFvPatchField<T>(patch,field);  
        } else {
            bfNew[patchI]=new zeroGradientFvPatchField<T>(patch,field);      
        }
    } else {
        bfNew[patchI]=bf[patchI].clone().ptr();
    }
  }

  bf.clear();
  bf.setSize(bfNew.size());
  forAll(bf,i) {
      bf.set(i,bfNew[i]);
  }
}

template<class T>
void FieldValueExpressionDriver::setValuePatches
(
    GeometricField<T,fvPatchField,volMesh> &field,
    bool keepPatches,
    const wordList &fixedPatches
) {
  typename GeometricField<T,fvPatchField,volMesh>::GeometricBoundaryField &bf=field.boundaryField();
  List<fvPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const fvPatch &patch=bf[patchI].patch();

    bool isValuePatch=false;
    forAll(fixedPatches,i) {
      if(fixedPatches[i]==patch.name()) {
	isValuePatch=true;
      }
    }

    if(
        (
            !keepPatches 
            ||
            isValuePatch
        ) 
        && 
        (
            typeid(patch)==typeid(wallFvPatch)
            ||
            typeid(patch)==typeid(fvPatch
            )
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(fixedValueFvPatchField<T>)) {
            fvPatchField<T> &pf=field.boundaryField()[patchI];
            
            pf==pf.patchInternalField();
        }
    }
  }
}

// Force the compiler to generate the code, there'S a better way but I'm too stupid
void dummyS(GeometricField<scalar,fvPatchField,volMesh>  &f,bool keepPatches,const wordList &fixedPatches) {
    FieldValueExpressionDriver::makePatches(f,keepPatches,fixedPatches);
    FieldValueExpressionDriver::setValuePatches(f,keepPatches,fixedPatches);
}

void dummyV(GeometricField<vector,fvPatchField,volMesh>  &f,bool keepPatches,const wordList &fixedPatches) {
    FieldValueExpressionDriver::makePatches(f,keepPatches,fixedPatches);
    FieldValueExpressionDriver::setValuePatches(f,keepPatches,fixedPatches);
}

} // end namespace
