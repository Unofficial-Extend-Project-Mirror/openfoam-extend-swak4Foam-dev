//  ICE Revision: $Id$ 

#include "FieldValueExpressionDriver.H"
#include <Random.H>
#include <wallDist.H>
#include <dimensionedVector.H>
#include "zeroGradientFvPatchFields.H"
#include "fixedValueFvPatchFields.H"
#include "wallFvPatch.H"
#include "cellSet.H"

FieldValueExpressionDriver::FieldValueExpressionDriver (
    const Foam::string& time,
    const Foam::Time& runTime,
    const Foam::fvMesh &mesh,
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
    : time_(time),
      mesh_(mesh),
      runTime_(runTime),
      result_(NULL),
      vresult_(NULL),
      typ_(NO_TYPE),
      cacheReadFields_(cacheReadFields),
      searchInMemory_(
          searchInMemory
          ||
          cacheReadFields_
      ),
      searchOnDisc_(searchOnDisc),
      trace_scanning (false),
      trace_parsing (false)
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

void FieldValueExpressionDriver::setScalarResult(Foam::volScalarField *r) {
    result_=r;
    typ_=SCALAR_TYPE;
}

void FieldValueExpressionDriver::setLogicalResult(Foam::volScalarField *r) {
    result_=r;
    typ_=LOGICAL_TYPE;
}

void FieldValueExpressionDriver::setVectorResult(Foam::volVectorField *r) {
    vresult_=r;
    typ_=VECTOR_TYPE;
}

void FieldValueExpressionDriver::parse (const std::string &f)
{
    content = f;
    scan_begin ();
    parserField::FieldValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing);
    parser.parse ();
    scan_end ();
}

void FieldValueExpressionDriver::error (const parserField::location& l, const std::string& m)
{
    std::ostringstream buff;
    buff << l;
    std::string place="";
    for(unsigned int i=0;i<l.begin.column;i++) {
        place+=" ";
    }
    for(unsigned int i=l.begin.column;i<l.end.column;i++) {
        place+="^";
    }
    for(unsigned int i=l.end.column;i<content.size();i++) {
        place+=" ";
    }

    Foam::FatalErrorIn("parsingValue")
        //        << Foam::args.executable()
        << " Parser Error at " << buff.str() << " :"  << m << Foam::endl
            << content << Foam::endl << place
            << Foam::exit(Foam::FatalError);
    //    Foam::Info << buff.str() << ": " << m << Foam::endl;
}

void FieldValueExpressionDriver::error (const std::string& m)
{
    Foam::FatalErrorIn("parsingValue")
        //        << Foam::args.executable()
            << " Parser Error: " << m
            << Foam::exit(Foam::FatalError);
}

bool FieldValueExpressionDriver::isCellSet(const Foam::string &name)
{
    if(getTypeOfSet(name)=="cellSet") {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isCellZone(const Foam::string &name)
{
    if(mesh_.cellZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

Foam::string FieldValueExpressionDriver::getTypeOfField(const Foam::string &name)
{
    Foam::IOobject f 
        (
            name,
            time_,
            mesh_,
            Foam::IOobject::MUST_READ,
            Foam::IOobject::NO_WRITE
        );
    f.headerOk();

    return f.headerClassName();
}

Foam::string FieldValueExpressionDriver::getTypeOfSet(const Foam::string &name)
{
    Foam::IOobject f 
        (
            name,
            time_,
            Foam::polyMesh::meshSubDir/"sets",
	    mesh_,
            Foam::IOobject::MUST_READ,
            Foam::IOobject::NO_WRITE
        );
    
    if(f.headerOk()) {;
        return f.headerClassName();
    } else {
        Foam::Info << "No set " << name << " at t=" << time_ 
            << " falling back to 'constant'" << Foam::endl;
        f=Foam::IOobject 
        (
            name,
            "constant",
            Foam::polyMesh::meshSubDir/"sets",
	    mesh_,
            Foam::IOobject::MUST_READ,
            Foam::IOobject::NO_WRITE
        );
        f.headerOk();
        return f.headerClassName();
    }
}

Foam::volScalarField *FieldValueExpressionDriver::makeModuloField(
    const Foam::volScalarField &a,
    const Foam::volScalarField &b)
{
    Foam::volScalarField *result_=makeScalarField(0.);

    forAll(*result_,cellI) {
        Foam::scalar val=fmod(a[cellI],b[cellI]);

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

Foam::volScalarField *FieldValueExpressionDriver::makeRandomField()
{
    Foam::volScalarField *f=makeScalarField(0.);
    Foam::Random rand(65);

    forAll(*f,cellI) {
        (*f)[cellI]=rand.scalar01();
    }

    return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeCellIdField()
{
    Foam::volScalarField *f=makeScalarField(0.);

    forAll(*f,cellI) {
        (*f)[cellI]=Foam::scalar(cellI);
    }

    return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeGaussRandomField()
{
    Foam::volScalarField *f=makeScalarField(0.);
    Foam::Random rand(65);

    forAll(*f,cellI) {
        (*f)[cellI]=rand.GaussNormal();
    }

    return f;
}

Foam::volVectorField *FieldValueExpressionDriver::makePositionField()
{
    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::volVectorField *f=new Foam::volVectorField(
        Foam::IOobject
        (
            "pos",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        Foam::vector(0,0,0)
    );
    f->dimensions().reset(mesh_.C().dimensions());
    *f=mesh_.C();
    f->dimensions().reset(nullDim);
    return f;
}

Foam::surfaceVectorField *FieldValueExpressionDriver::makeFacePositionField()
{
    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::surfaceVectorField *f=new Foam::surfaceVectorField(
        Foam::IOobject
        (
            "fpos",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        Foam::vector(0,0,0)
    );
    f->dimensions().reset(mesh_.Cf().dimensions());
    *f=mesh_.Cf();
    f->dimensions().reset(nullDim);
    return f;
}

Foam::surfaceVectorField *FieldValueExpressionDriver::makeFaceProjectionField()
{

    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::surfaceVectorField *f=new Foam::surfaceVectorField(
        Foam::IOobject
        (
            "fproj",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        Foam::vector(0,0,0)
    );
    f->dimensions().reset(mesh_.Cf().dimensions());

    Foam::vector fmin(0,0,0);
    Foam::vector fmax(0,0,0);
    
    forAll(*f,faceI)
    {
        const Foam::face &fProp = mesh_.faces()[faceI];
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
        Foam::labelList cNumbers = mesh_.boundaryMesh()[patchI].faceCells();
        Foam::fvsPatchVectorField & fFace = f->boundaryField()[patchI];
        
        forAll(fFace,faceI)
        {
            const Foam::cell & cProp(mesh_.cells()[cNumbers[faceI]]);

            forAll(cProp,cJ)
            {
                const Foam::label patchID = mesh_.boundaryMesh().whichPatch(cProp[cJ]);

                if (patchID == patchI)
                {
                    const Foam::face & fProp = mesh_.faces()[cProp[cJ]];

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

Foam::surfaceVectorField *FieldValueExpressionDriver::makeFaceField()
{
    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::surfaceVectorField *f=new Foam::surfaceVectorField(
        Foam::IOobject
        (
            "face",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        Foam::vector(0,0,0)
    );
    f->dimensions().reset(mesh_.Sf().dimensions());
    *f=mesh_.Sf();
    f->dimensions().reset(nullDim);
    return f;
}

Foam::surfaceScalarField *FieldValueExpressionDriver::makeAreaField()
{
    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::surfaceScalarField *f=new Foam::surfaceScalarField(
        Foam::IOobject
        (
            "face",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );
    f->dimensions().reset(mesh_.magSf().dimensions());
    *f=mesh_.magSf();
    f->dimensions().reset(nullDim);
    return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeVolumeField()
{
    Foam::volScalarField *f=new Foam::volScalarField(
        Foam::IOobject
        (
            "vol",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );
    const Foam::scalarField &V=mesh_.V();

    forAll(*f,cellI) {
        (*f)[cellI]=V[cellI];
    }

    return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeDistanceField()
{
    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::volScalarField *f=new Foam::volScalarField(
        Foam::IOobject
        (
            "dist",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );
    f->dimensions().reset(mesh_.C().dimensions());
    Foam::wallDist dist(mesh_);
    *f=dist;
    f->dimensions().reset(nullDim);
    return f;

}

Foam::volScalarField *FieldValueExpressionDriver::makeRDistanceField(const Foam::volVectorField& r)
{
    Foam::dimensionSet nullDim(0,0,0,0,0);
    Foam::volScalarField *f=new Foam::volScalarField(
        Foam::IOobject
        (
            "rdist",
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        0.
    );

    forAll(*f,cellI) {
        (*f)[cellI]=mag(mesh_.C()[cellI] - r[cellI]);
    }

    return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeScalarField(const Foam::scalar &val)
{
    std::ostringstream buff;
    buff << "constantScalar" << val;

    Foam::volScalarField *f=new Foam::volScalarField(
        Foam::IOobject
        (
            buff.str(),
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        val,
        "zeroGradient"
    );

    return f;
}

Foam::surfaceScalarField *FieldValueExpressionDriver::makeSurfaceScalarField
(
    const Foam::scalar &val
){
    std::ostringstream buff;
    buff << "constantScalar" << val;

    Foam::surfaceScalarField *f=new Foam::surfaceScalarField(
        Foam::IOobject
        (
            buff.str(),
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        val,
        "fixedValue"
    );

    return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeCellSetField(const Foam::string &name)
{
  Foam::volScalarField *f=makeScalarField(0);

  Foam::IOobject head 
      (
          name,
          time_,
          Foam::polyMesh::meshSubDir/"sets",
          mesh_,
          Foam::IOobject::MUST_READ,
          Foam::IOobject::NO_WRITE
      );
  
  if(!head.headerOk()) {;
      head=Foam::IOobject 
          (
              name,
              "constant",
              Foam::polyMesh::meshSubDir/"sets",
              mesh_,
              Foam::IOobject::MUST_READ,
              Foam::IOobject::NO_WRITE
          );
      head.headerOk();
  }

  Foam::cellSet cs(head);
  Foam::labelList cells(cs.toc());

  forAll(cells,cellI) {
    (*f)[cells[cellI]]=1.;
  }

  return f;
}

Foam::volScalarField *FieldValueExpressionDriver::makeCellZoneField(const Foam::string &name)
{
  Foam::volScalarField *f=makeScalarField(0);
  Foam::label zoneID=mesh_.cellZones().findZoneID(name);

  const Foam::cellZone &zone=mesh_.cellZones()[zoneID];

  forAll(zone,ind) {
      Foam::label cellI=zone[ind];
      (*f)[cellI]=1.;
  }

  return f;
}

Foam::volVectorField *FieldValueExpressionDriver::makeVectorField(const Foam::vector &vec)
{
    std::ostringstream buff;
    buff << "constantVector" << vec.x() << "_" << vec.y() << "_" << vec.z() ;

    Foam::volVectorField *f=new Foam::volVectorField(
        Foam::IOobject
        (
            buff.str(),
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        vec,
        "zeroGradient"
    );

    return f;
}

Foam::volVectorField *FieldValueExpressionDriver::makeVectorField
(
    Foam::volScalarField *x,
    Foam::volScalarField *y,
    Foam::volScalarField *z
) {
    Foam::volVectorField *f=makeVectorField(Foam::vector(0,0,0));

    forAll(*f,cellI) {
        (*f)[cellI]=Foam::vector((*x)[cellI],(*y)[cellI],(*z)[cellI]);
    }

    return f;
}

Foam::surfaceVectorField *FieldValueExpressionDriver::makeSurfaceVectorField
(
    const Foam::vector &vec
){
    std::ostringstream buff;
    buff << "constantVector" << vec.x() << "_" << vec.y() << "_" << vec.z() ;

    Foam::surfaceVectorField *f=new Foam::surfaceVectorField(
        Foam::IOobject
        (
            buff.str(),
            time_,
            mesh_,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE
        ),
        mesh_,
        vec,
        "fixedValue"
    );

    return f;
}

Foam::surfaceVectorField *FieldValueExpressionDriver::makeSurfaceVectorField
(
    Foam::surfaceScalarField *x,
    Foam::surfaceScalarField *y,
    Foam::surfaceScalarField *z
)
{
    Foam::surfaceVectorField *f=makeSurfaceVectorField(Foam::vector(0,0,0));

    forAll(*f,faceI) {
        (*f)[faceI]=Foam::vector((*x)[faceI],(*y)[faceI],(*z)[faceI]);
    }

    return f;
}

template<class T>
void FieldValueExpressionDriver::makePatches
(
    Foam::GeometricField<T,Foam::fvPatchField,Foam::volMesh> &field,
    bool keepPatches,
    const Foam::wordList &fixedPatches
) {
  typename Foam::GeometricField<T,Foam::fvPatchField,Foam::volMesh>::GeometricBoundaryField &bf=field.boundaryField();
  Foam::List<Foam::fvPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const Foam::fvPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(Foam::wallFvPatch)
            || 
            typeid(patch)==typeid(Foam::fvPatch
            )
        )
    ) {
        if(isValuePatch){
            bfNew[patchI]=new Foam::fixedValueFvPatchField<T>(patch,field);  
        } else {
            bfNew[patchI]=new Foam::zeroGradientFvPatchField<T>(patch,field);      
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
    Foam::GeometricField<T,Foam::fvPatchField,Foam::volMesh> &field,
    bool keepPatches,
    const Foam::wordList &fixedPatches
) {
  typename Foam::GeometricField<T,Foam::fvPatchField,Foam::volMesh>::GeometricBoundaryField &bf=field.boundaryField();
  Foam::List<Foam::fvPatchField<T> *>bfNew(bf.size());

  forAll(bf,patchI) {
    const Foam::fvPatch &patch=bf[patchI].patch();

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
            typeid(patch)==typeid(Foam::wallFvPatch)
            ||
            typeid(patch)==typeid(Foam::fvPatch
            )
        )
    ) {
        if(typeid(field.boundaryField()[patchI])==typeid(Foam::fixedValueFvPatchField<T>)) {
            Foam::fvPatchField<T> &pf=field.boundaryField()[patchI];
            
            pf==pf.patchInternalField();
        }
    }
  }
}

bool FieldValueExpressionDriver::debug=false;

// Force the compiler to generate the code, there'S a better way but I'm too stupid
void dummyS(Foam::GeometricField<Foam::scalar,Foam::fvPatchField,Foam::volMesh>  &f,bool keepPatches,const Foam::wordList &fixedPatches) {
    FieldValueExpressionDriver::makePatches(f,keepPatches,fixedPatches);
    FieldValueExpressionDriver::setValuePatches(f,keepPatches,fixedPatches);
}

void dummyV(Foam::GeometricField<Foam::vector,Foam::fvPatchField,Foam::volMesh>  &f,bool keepPatches,const Foam::wordList &fixedPatches) {
    FieldValueExpressionDriver::makePatches(f,keepPatches,fixedPatches);
    FieldValueExpressionDriver::setValuePatches(f,keepPatches,fixedPatches);
}
