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
    2006-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FieldValueExpressionDriver.H"
#include "FieldValuePluginFunction.H"

#include <Random.H>
#include <wallDist.H>
#include <nearWallDist.H>
#include <dimensionedVector.H>
#include "cellSet.H"
#include "faceSet.H"
#include "pointSet.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

word FieldValueExpressionDriver::driverName_="internalField";

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
      isPointField_(false),
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
      isPointField_(false),
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
      isPointField_(false),
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
      isPointField_(false),
      resultDimension_(0,0,0,0,0,0,0)
{
    if(dict.found("dimensions")) {
        resultDimension_.reset(dimensionSet(dict.lookup("dimensions")));
    }
}

FieldValueExpressionDriver::~FieldValueExpressionDriver ()
{
}

void FieldValueExpressionDriver::readVariablesAndTables(const dictionary &dict)
{
    CommonValueExpressionDriver::readVariablesAndTables(dict);

    if(dict.found("dimensions")) {
        resultDimension_.reset(dimensionSet(dict.lookup("dimensions")));
    }
}

void FieldValueExpressionDriver::parseInternal(int startToken)
{
    parserField::FieldValueExpressionParser parser (scanner_,*this,startToken,0);
    parser.set_debug_level (traceParsing());
    parser.parse ();
}

bool FieldValueExpressionDriver::isCellSet(const word &name)
{
    if(getTypeOfSet(name)=="cellSet") {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isCellZone(const word &name)
{
    if(mesh_.cellZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isFaceSet(const word &name)
{
    if(getTypeOfSet(name)=="faceSet") {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isFaceZone(const word &name)
{
    if(mesh_.faceZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isPointSet(const word &name)
{
    if(getTypeOfSet(name)=="pointSet") {
        return true;
    } else {
        return false;
    }
}

bool FieldValueExpressionDriver::isPointZone(const word &name)
{
    if(mesh_.pointZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

tmp<volScalarField> FieldValueExpressionDriver::makeModuloField(
    const volScalarField &a,
    const volScalarField &b)
{
    tmp<volScalarField> result=makeConstantField<volScalarField>(0.);

    forAll(result(),cellI) {
        scalar val=fmod(a[cellI],b[cellI]);

        if(fabs(val)>(b[cellI]/2)) {
            if(val>0) {
                val-=b[cellI];
            } else {
                val+=b[cellI];
            }
        }

        result()[cellI]=val;
    }

    result->correctBoundaryConditions();

    return result;
}

tmp<volScalarField> FieldValueExpressionDriver::makeRandomField(label seed)
{
    tmp<volScalarField> f=makeConstantField<volScalarField>(0.);

    f->internalField()=CommonValueExpressionDriver::makeRandomField(seed);

    f->correctBoundaryConditions();

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeCellIdField()
{
    tmp<volScalarField> f=makeConstantField<volScalarField>(0.);

    forAll(f(),cellI) {
        f()[cellI]=scalar(cellI);
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeGaussRandomField(label seed)
{
    tmp<volScalarField> f=makeConstantField<volScalarField>(0.);

    f->internalField()=CommonValueExpressionDriver::makeGaussRandomField(seed);

    f->correctBoundaryConditions();

    return f;
}

tmp<volVectorField> FieldValueExpressionDriver::makePositionField()
{
    tmp<volVectorField> f(
        new volVectorField(
            IOobject
            (
                "pos",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            vector(0,0,0),
            "zeroGradient"
        )
    );
    f->dimensions().reset(mesh_.C().dimensions());
    f()=mesh_.C();
    f->dimensions().reset(dimless);

    f->correctBoundaryConditions();

    return f;
}

tmp<pointVectorField> FieldValueExpressionDriver::makePointPositionField()
{
    tmp<pointVectorField> f(
        new pointVectorField(
            IOobject
            (
                "fpos",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            this->pMesh(),
            vector::zero,
            "zeroGradient"
        )
    );
    f->internalField()=mesh_.points();
    f->correctBoundaryConditions();

    return f;
}

tmp<surfaceVectorField> FieldValueExpressionDriver::makeFacePositionField()
{
    tmp<surfaceVectorField> f(
        new surfaceVectorField(
            IOobject
            (
                "fpos",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            vector::zero
        )
    );
    f->dimensions().reset(mesh_.Cf().dimensions());
    f()=mesh_.Cf();
    f->dimensions().reset(dimless);

    return f;
}

tmp<surfaceVectorField> FieldValueExpressionDriver::makeFaceProjectionField()
{
    tmp<surfaceVectorField> f(
        new surfaceVectorField(
            IOobject
            (
                "fproj",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            vector(0,0,0)
        )
    );
    f->dimensions().reset(mesh_.Cf().dimensions());

    vector fmin(0,0,0);
    vector fmax(0,0,0);

    forAll(f(),faceI)
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
        f()[faceI] = fmax - fmin;
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

    f->dimensions().reset(dimless);

    return f;
}

tmp<surfaceVectorField> FieldValueExpressionDriver::makeFaceField()
{
    tmp<surfaceVectorField> f(
        new surfaceVectorField(
            IOobject
            (
                "face",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            vector(0,0,0)
        )
    );
    f->dimensions().reset(mesh_.Sf().dimensions());
    f()=mesh_.Sf();
    f->dimensions().reset(dimless);

    return f;
}

tmp<surfaceScalarField> FieldValueExpressionDriver::makeAreaField()
{
    tmp<surfaceScalarField> f(
        new surfaceScalarField(
            IOobject
            (
                "face",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            0.
        )
    );
    f->dimensions().reset(mesh_.magSf().dimensions());
    f()=mesh_.magSf();
    f->dimensions().reset(dimless);

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeVolumeField()
{
    tmp<volScalarField> f(
        new volScalarField(
            IOobject
            (
                "vol",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            0.,
            "zeroGradient"
        )
    );
    const scalarField &V=mesh_.V();

    forAll(f(),cellI) {
        f()[cellI]=V[cellI];
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeDistanceField()
{
    tmp<volScalarField> f(
        new volScalarField(
            IOobject
            (
                "dist",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            0.,
            "fixedValue"
        )
    );
    f->dimensions().reset(mesh_.C().dimensions());
    wallDist dist(mesh_);
    f()==dist;
    f->dimensions().reset(dimless);

    f->correctBoundaryConditions();

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeNearDistanceField()
{
    tmp<volScalarField> f(
        new volScalarField(
            IOobject
            (
                "dist",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            0.,
            "fixedValue"
        )
    );

    f->dimensions().reset(mesh_.C().dimensions());
    nearWallDist dist(mesh_);
    f->boundaryField()==dist;
    f->dimensions().reset(dimless);

    f->correctBoundaryConditions();

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeRDistanceField(const volVectorField& r)
{
    tmp<volScalarField> f(
        new volScalarField(
            IOobject
            (
                "rdist",
                time(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false // don't register
            ),
            mesh_,
            0.,
            "zeroGradient"
        )
    );

    forAll(f(),cellI) {
        f()[cellI]=mag(mesh_.C()[cellI] - r[cellI]);
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeCellSetField(const word &name)
{
  tmp<volScalarField> f=makeConstantField<volScalarField>(0);

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
    f()[cells[cellI]]=1.;
  }

  f->correctBoundaryConditions();

  return f;
}

tmp<surfaceScalarField> FieldValueExpressionDriver::makeInternalFaceField()
{
    tmp<surfaceScalarField> f=makeConstantField<surfaceScalarField>(1,true);

    forAll(f->boundaryField(),patchI) {
        forAll(f().boundaryField()[patchI],faceI) {
            f->boundaryField()[patchI][faceI]=0;
        }
    }

    return f;
}

tmp<surfaceScalarField> FieldValueExpressionDriver::makeOnPatchField(const word &name)
{
    tmp<surfaceScalarField> f=makeConstantField<surfaceScalarField>(0,true);

    label patchI=mesh().boundaryMesh().findPatchID(name);
    if(patchI<0) {
        FatalErrorIn("makeFaceSetField(const word &name)")
            << "Patch name " << name << " not in valid names"
                << mesh().boundaryMesh().names()
                << endl
                << exit(FatalError);
    } else {
        forAll(f().boundaryField()[patchI],faceI) {
            f().boundaryField()[patchI][faceI]=1;
        }
    }

    return f;
}

tmp<surfaceScalarField> FieldValueExpressionDriver::makeFaceSetField(const word &name)
{
    tmp<surfaceScalarField> f=makeConstantField<surfaceScalarField>(0,true);

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
      if(faces[faceI] < mesh().nInternalFaces()) {
          f()[faces[faceI]]=1.;
      } else {
          label patchI=mesh().boundaryMesh().whichPatch(faces[faceI]);
          if(patchI<0) {
              FatalErrorIn("FieldValueExpressionDriver::makeFaceSetField(const word &name")
                  << "Face " << faces[faceI] << " of faceSet "
                      << name << " is not in the mesh"
                      << endl
                      << exit(FatalError);
          } else {
              f().boundaryField()[patchI][
                  faces[faceI]
                  -
                  mesh().boundaryMesh()[patchI].start()] = 1.;
          }
      }
  }

  return f;
}

tmp<pointScalarField> FieldValueExpressionDriver::makePointSetField(const word &name)
{
  tmp<pointScalarField> f=makePointConstantField<pointScalarField>(0);

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

  pointSet cs(head);
  labelList points(cs.toc());

  forAll(points,pointI) {
    f()[points[pointI]]=1.;
  }

  return f;
}

tmp<volScalarField> FieldValueExpressionDriver::makeCellZoneField(const word &name)
{
  tmp<volScalarField> f=makeConstantField<volScalarField>(0);
  label zoneID=mesh_.cellZones().findZoneID(name);

  const cellZone &zone=mesh_.cellZones()[zoneID];

  forAll(zone,ind) {
      label cellI=zone[ind];
      f()[cellI]=1.;
  }

  f->correctBoundaryConditions();

  return f;
}

tmp<surfaceScalarField> FieldValueExpressionDriver::makeFaceZoneField(const word &name)
{
  tmp<surfaceScalarField> f=makeConstantField<surfaceScalarField>(0);
  label zoneID=mesh_.faceZones().findZoneID(name);

  const faceZone &zone=mesh_.faceZones()[zoneID];

  forAll(zone,ind) {
      label faceI=zone[ind];

      if(faceI < mesh().nInternalFaces()) {
          f()[faceI]=1.;
      } else {
          label patchI=mesh().boundaryMesh().whichPatch(faceI);
          if(patchI<0) {
              FatalErrorIn("FieldValueExpressionDriver::makeFaceZoneField(const word &name")
                  << "Face " << faceI << " of faceZone "
                      << name << " is not in the mesh"
                      << endl
                      << exit(FatalError);
          } else {
              f().boundaryField()[patchI][
                  faceI
                  -
                  mesh().boundaryMesh()[patchI].start()] = 1.;
          }
      }
  }

  return f;
}

tmp<pointScalarField> FieldValueExpressionDriver::makePointZoneField(const word &name)
{
  tmp<pointScalarField> f=makePointConstantField<pointScalarField>(0);
  label zoneID=mesh_.pointZones().findZoneID(name);

  const pointZone &zone=mesh_.pointZones()[zoneID];

  forAll(zone,ind) {
      label pointI=zone[ind];
      f()[pointI]=1.;
  }

  return f;
}

tmp<volVectorField> FieldValueExpressionDriver::makeVectorField
(
    const volScalarField &x,
    const volScalarField &y,
    const volScalarField &z
) {
    tmp<volVectorField> f=makeConstantField<volVectorField>(vector(0,0,0));

    forAll(f(),cellI) {
        f()[cellI]=vector(x[cellI],y[cellI],z[cellI]);
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<volTensorField> FieldValueExpressionDriver::makeTensorField
(
    const volScalarField &xx,const volScalarField &xy,const volScalarField &xz,
    const volScalarField &yx,const volScalarField &yy,const volScalarField &yz,
    const volScalarField &zx,const volScalarField &zy,const volScalarField &zz
) {
    tmp<volTensorField> f=makeConstantField<volTensorField>(tensor(0,0,0,0,0,0,0,0,0));

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

tmp<volSymmTensorField> FieldValueExpressionDriver::makeSymmTensorField
(
    const volScalarField &xx,const volScalarField &xy,const volScalarField &xz,
    const volScalarField &yy,const volScalarField &yz,
    const volScalarField &zz
) {
    tmp<volSymmTensorField> f=makeConstantField<volSymmTensorField>(symmTensor(0,0,0,0,0,0));

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

tmp<volSphericalTensorField> FieldValueExpressionDriver::makeSphericalTensorField
(
    const volScalarField &xx
) {
    tmp<volSphericalTensorField> f=makeConstantField<volSphericalTensorField>(sphericalTensor(0));

    forAll(f(),cellI) {
        f()[cellI]=sphericalTensor(
            xx[cellI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<surfaceVectorField> FieldValueExpressionDriver::makeSurfaceVectorField
(
    const surfaceScalarField &x,
    const surfaceScalarField &y,
    const surfaceScalarField &z
)
{
    tmp<surfaceVectorField> f=makeConstantField<surfaceVectorField>(vector(0,0,0));

    forAll(f(),faceI) {
        f()[faceI]=vector(x[faceI],y[faceI],z[faceI]);
    }

    return f;
}

tmp<surfaceTensorField> FieldValueExpressionDriver::makeSurfaceTensorField
(
    const surfaceScalarField &xx,const surfaceScalarField &xy,const surfaceScalarField &xz,
    const surfaceScalarField &yx,const surfaceScalarField &yy,const surfaceScalarField &yz,
    const surfaceScalarField &zx,const surfaceScalarField &zy,const surfaceScalarField &zz
) {
    tmp<surfaceTensorField> f=makeConstantField<surfaceTensorField>(tensor(0,0,0,0,0,0,0,0,0));

    forAll(f(),faceI) {
        f()[faceI]=tensor(
            xx[faceI],xy[faceI],xz[faceI],
            yx[faceI],yy[faceI],yz[faceI],
            zx[faceI],zy[faceI],zz[faceI]
        );
    }

    return f;
}

tmp<surfaceSymmTensorField> FieldValueExpressionDriver::makeSurfaceSymmTensorField
(
    const surfaceScalarField &xx,const surfaceScalarField &xy,const surfaceScalarField &xz,
    const surfaceScalarField &yy,const surfaceScalarField &yz,
    const surfaceScalarField &zz
) {
    tmp<surfaceSymmTensorField> f=makeConstantField<surfaceSymmTensorField>(symmTensor(0,0,0,0,0,0));

    forAll(f(),faceI) {
        f()[faceI]=symmTensor(
            xx[faceI],xy[faceI],xz[faceI],
            yy[faceI],yz[faceI],
            zz[faceI]
        );
    }

    return f;
}

tmp<surfaceSphericalTensorField> FieldValueExpressionDriver::makeSurfaceSphericalTensorField
(
    const surfaceScalarField &xx
) {
    tmp<surfaceSphericalTensorField> f=makeConstantField<surfaceSphericalTensorField>(sphericalTensor(0));

    forAll(f(),faceI) {
        f()[faceI]=sphericalTensor(
            xx[faceI]
        );
    }

    return f;
}

tmp<pointVectorField> FieldValueExpressionDriver::makePointVectorField
(
    const pointScalarField &x,
    const pointScalarField &y,
    const pointScalarField &z
)
{
    tmp<pointVectorField> f=makePointConstantField<pointVectorField>(vector(0,0,0));

    forAll(f(),faceI) {
        f()[faceI]=vector(x[faceI],y[faceI],z[faceI]);
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<pointTensorField> FieldValueExpressionDriver::makePointTensorField
(
    const pointScalarField &xx,const pointScalarField &xy,const pointScalarField &xz,
    const pointScalarField &yx,const pointScalarField &yy,const pointScalarField &yz,
    const pointScalarField &zx,const pointScalarField &zy,const pointScalarField &zz
) {
    tmp<pointTensorField> f=makePointConstantField<pointTensorField>(tensor(0,0,0,0,0,0,0,0,0));

    forAll(f(),faceI) {
        f()[faceI]=tensor(
            xx[faceI],xy[faceI],xz[faceI],
            yx[faceI],yy[faceI],yz[faceI],
            zx[faceI],zy[faceI],zz[faceI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<pointSymmTensorField> FieldValueExpressionDriver::makePointSymmTensorField
(
    const pointScalarField &xx,const pointScalarField &xy,const pointScalarField &xz,
    const pointScalarField &yy,const pointScalarField &yz,
    const pointScalarField &zz
) {
    tmp<pointSymmTensorField> f=makePointConstantField<pointSymmTensorField>(symmTensor(0,0,0,0,0,0));

    forAll(f(),faceI) {
        f()[faceI]=symmTensor(
            xx[faceI],xy[faceI],xz[faceI],
            yy[faceI],yz[faceI],
            zz[faceI]
        );
    }

    f->correctBoundaryConditions();

    return f;
}

tmp<pointSphericalTensorField>
FieldValueExpressionDriver::makePointSphericalTensorField
(
    const pointScalarField &xx
) {
    tmp<pointSphericalTensorField> f=makePointConstantField
        <pointSphericalTensorField>(
            sphericalTensor(0)
        );

    forAll(f(),faceI) {
        f()[faceI]=sphericalTensor(
            xx[faceI]
        );
    }

    f->correctBoundaryConditions();

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

template<>
FieldValueExpressionDriver::SymbolTable<FieldValueExpressionDriver>::SymbolTable()
:
StartupSymbols()
{
    // default value
    insert("",parserField::FieldValueExpressionParser::token::START_DEFAULT);

    insert(
        "volScalarField_SC",
        parserField::FieldValueExpressionParser::token::START_VOL_SCALAR_COMMA
    );
    insert(
        "volScalarField_CL",
        parserField::FieldValueExpressionParser::token::START_VOL_SCALAR_CLOSE
    );
    insert(
        "volVectorField_SC",
        parserField::FieldValueExpressionParser::token::START_VOL_VECTOR_COMMA
    );
    insert(
        "volVectorField_CL",
        parserField::FieldValueExpressionParser::token::START_VOL_VECTOR_CLOSE
    );
    insert(
        "volTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_VOL_TENSOR_COMMA
    );
    insert(
        "volTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_VOL_TENSOR_CLOSE
    );
    insert(
        "volSymmTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_VOL_YTENSOR_COMMA
    );
    insert(
        "volSymmTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_VOL_YTENSOR_CLOSE
    );
    insert(
        "volSphericalTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_VOL_HTENSOR_COMMA
    );
    insert(
        "volSphericalTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_VOL_HTENSOR_CLOSE
    );
    insert(
        "volLogicalField_SC",
        parserField::FieldValueExpressionParser::token::START_VOL_LOGICAL_COMMA
    );
    insert(
        "volLogicalField_CL",
        parserField::FieldValueExpressionParser::token::START_VOL_LOGICAL_CLOSE
    );

    insert(
        "surfaceScalarField_SC",
        parserField::FieldValueExpressionParser::token::START_SURFACE_SCALAR_COMMA
    );
    insert(
        "surfaceScalarField_CL",
        parserField::FieldValueExpressionParser::token::START_SURFACE_SCALAR_CLOSE
    );
    insert(
        "surfaceVectorField_SC",
        parserField::FieldValueExpressionParser::token::START_SURFACE_VECTOR_COMMA
    );
    insert(
        "surfaceVectorField_CL",
        parserField::FieldValueExpressionParser::token::START_SURFACE_VECTOR_CLOSE
    );
    insert(
        "surfaceTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_SURFACE_TENSOR_COMMA
    );
    insert(
        "surfaceTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_SURFACE_TENSOR_CLOSE
    );
    insert(
        "surfaceSymmTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_SURFACE_YTENSOR_COMMA
    );
    insert(
        "surfaceSymmTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_SURFACE_YTENSOR_CLOSE
    );
    insert(
        "surfaceSphericalTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_SURFACE_HTENSOR_COMMA
    );
    insert(
        "surfaceSphericalTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_SURFACE_HTENSOR_CLOSE
    );
    insert(
        "surfaceLogicalField_SC",
        parserField::FieldValueExpressionParser::token::START_SURFACE_LOGICAL_COMMA
    );
    insert(
        "surfaceLogicalField_CL",
        parserField::FieldValueExpressionParser::token::START_SURFACE_LOGICAL_CLOSE
    );

    insert(
        "pointScalarField_SC",
        parserField::FieldValueExpressionParser::token::START_POINT_SCALAR_COMMA
    );
    insert(
        "pointScalarField_CL",
        parserField::FieldValueExpressionParser::token::START_POINT_SCALAR_CLOSE
    );
    insert(
        "pointVectorField_SC",
        parserField::FieldValueExpressionParser::token::START_POINT_VECTOR_COMMA
    );
    insert(
        "pointVectorField_CL",
        parserField::FieldValueExpressionParser::token::START_POINT_VECTOR_CLOSE
    );
    insert(
        "pointTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_POINT_TENSOR_COMMA
    );
    insert(
        "pointTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_POINT_TENSOR_CLOSE
    );
    insert(
        "pointSymmTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_POINT_YTENSOR_COMMA
    );
    insert(
        "pointSymmTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_POINT_YTENSOR_CLOSE
    );
    insert(
        "pointSphericalTensorField_SC",
        parserField::FieldValueExpressionParser::token::START_POINT_HTENSOR_COMMA
    );
    insert(
        "pointSphericalTensorField_CL",
        parserField::FieldValueExpressionParser::token::START_POINT_HTENSOR_CLOSE
    );
    insert(
        "pointLogicalField_SC",
        parserField::FieldValueExpressionParser::token::START_POINT_LOGICAL_COMMA
    );
    insert(
        "pointLogicalField_CL",
        parserField::FieldValueExpressionParser::token::START_POINT_LOGICAL_CLOSE
    );

    insert(
        "CL",
        parserField::FieldValueExpressionParser::token::START_CLOSE_ONLY
    );
    insert(
        "SC",
        parserField::FieldValueExpressionParser::token::START_COMMA_ONLY
    );
}

const FieldValueExpressionDriver::SymbolTable<FieldValueExpressionDriver> &FieldValueExpressionDriver::symbolTable()
{
    static SymbolTable<FieldValueExpressionDriver> actualTable;

    return actualTable;
}

int FieldValueExpressionDriver::startupSymbol(const word &name) {
    return symbolTable()[name];
}


autoPtr<CommonPluginFunction> FieldValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        FieldValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool FieldValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return FieldValuePluginFunction::exists(
        *this,
        name
    );
}

tmp<scalarField> FieldValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label cellSize=mesh().nCells();
    const label faceSize=mesh().nInternalFaces();
    bool isCell=(size==cellSize);
    bool isFace=(size==faceSize);
    reduce(isCell,andOp<bool>());
    reduce(isFace,andOp<bool>());

    if(isCell && isFace) {
        WarningIn("FieldValueExpressionDriver::weightsNonPoint")
            << "Can't determine whether this is a face or a cell field" << endl
                << "Going for CELL"
                << endl;
    } else if(isCell==isFace) {
        Pout << "Expected size: " << size
            << " Cell size: " << cellSize
            << " Face size: " << faceSize << endl;

        FatalErrorIn("FieldValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }
    if(isCell) {
        return tmp<scalarField>(new scalarField(mesh().V()));
    } else {
        return tmp<scalarField>(new scalarField(mesh().magSf()));
    }
}

} // end namespace
