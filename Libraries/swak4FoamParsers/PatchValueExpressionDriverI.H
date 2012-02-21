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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "surfaceMesh.H"
#include "fvsPatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include <cassert>

namespace Foam {
 
    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        {
            Field<Type> *vals=NULL;
            bool isPoint=false;

            if(variables_.found(name)) {
                isPoint=variables_[name].isPoint();
                vals=variables_[name].getResult<Type>(true).ptr();
            } else if(isGlobal<Type>(name,false)) {
                vals=const_cast<ExpressionResult &>(
                    lookupGlobal(name)
                ).getResult<Type>(true).ptr();
            }
            if(vals) {
                if(
                    (
                        ! isPoint
                        &&
                        vals->size()==this->size()
                    )
                    ||
                    (
                        isPoint
                        &&
                        vals->size()==this->pointSize()
                    )
                ) {
                    return vals;
                } else {
                    Field<Type> *result=new Field<Type>(this->size(),gAverage(*vals));
                    delete vals;
                    return result;
                }
            }
        }

        autoPtr<localVolField> tmpVol;
        autoPtr<localSurfaceField> tmpSurf;
        autoPtr<localPointField> tmpPoint;

        if(
            searchOnDisc()
            &&
            !mesh().foundObject<localVolField>(name)
            &&
            !mesh().foundObject<localSurfaceField>(name)
            &&
            !mesh().foundObject<localPointField>(name)
        ) {
            word fType=this->getTypeOfField(name);
            if(fType==localVolField::typeName) {
                tmpVol.set(this->readAndRegister<localVolField>(name,mesh()).ptr());
            } else if(fType==localSurfaceField::typeName) {
                tmpSurf.set(this->readAndRegister<localSurfaceField>(name,mesh()).ptr());
            } 
            // TODO: this constructor doesn't work
            // else if(fType==localPointField::typeName) {
            //     tmpPoint.set(this->readAndRegister<localPointField>(name,mesh()).ptr());
            // }
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField);
        } else if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localSurfaceField>(name)
        ) {
            const typename localSurfaceField::PatchFieldType &pField=
                patch_.lookupPatchField<localSurfaceField,Type>(name);
            return new Field<Type>(pField);
        } else if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else{
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getField(const string &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << "Fields of type: " << localVolField::typeName
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localVolField>()
                    << endl
                    << "Fields of type: " << localSurfaceField::typeName
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localSurfaceField>()
                    << endl
                    << "Fields of type: " << localPointField::typeName
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localPointField>()
                    << endl
                    // << "All registered objects:"
                    // << patch_.boundaryMesh().mesh().objectRegistry::names()
                    // << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getOldTimeField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        autoPtr<localVolField> tmpVol;
        autoPtr<localSurfaceField> tmpSurf;
        autoPtr<localPointField> tmpPoint;

        if(
            searchOnDisc()
            &&
            !mesh().foundObject<localVolField>(name)
            &&
            !mesh().foundObject<localSurfaceField>(name)
            &&
            !mesh().foundObject<localPointField>(name)
        ) {
            word fType=this->getTypeOfField(name);
            if(fType==localVolField::typeName) {
                tmpVol.set(this->readAndRegister<localVolField>(name,mesh()).ptr());
            } else if(fType==localSurfaceField::typeName) {
                tmpSurf.set(this->readAndRegister<localSurfaceField>(name,mesh()).ptr());
            } 
            // TODO: this constructor doesn't work
            // else if(fType==localPointField::typeName) {
            //     tmpPoint.set(this->readAndRegister<localPointField>(name,mesh()).ptr());
            // }
        }

        label patchI=this->patch_.index();
        const fvMesh &mesh=patch_.boundaryMesh().mesh();

        if(
            mesh.objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                mesh.lookupObject<localVolField>(name).
                oldTime().boundaryField()[patchI];
            return new Field<Type>(pField);
        } else if(
            mesh.objectRegistry::foundObject<localSurfaceField>(name)
        ) {
            const typename localSurfaceField::PatchFieldType &pField=
                mesh.lookupObject<localSurfaceField>(name).
                oldTime().boundaryField()[patchI];
            return new Field<Type>(pField);
        } else if(
            mesh.objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                mesh.lookupObject<localPointField>(name).
                oldTime().boundaryField()[patchI];
            return new Field<Type>(pField.patchInternalField());
        } else{
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getOldTimeField(const string &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getPatchInternalField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
        }

        autoPtr<localVolField> dummy; // hold on to the field until the calculation is finished
        if(
            !patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
            &&
            searchOnDisc()
        ) {
            dummy=this->getOrReadField<localVolField>(name,false);
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else{
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getPatchInternalField(const string &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found or unsupported type for patchInternalField\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getPatchNeighbourField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
        }

        autoPtr<localVolField> dummy; // hold on to the field until the calculation is finished
        if(
            !patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
            &&
            searchOnDisc()
        ) {
            dummy=this->getOrReadField<localVolField>(name,false);
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField.patchNeighbourField());
        } else{
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getPatchNeighbourField(const string &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found or unsupported type for patchNeighbourField\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getSurfaceNormalField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;

        autoPtr<localVolField> dummy; // hold on to the field until the calculation is finished
        if(
            !patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
            &&
            searchOnDisc()
        ) {
            dummy=this->getOrReadField<localVolField>(name,false);
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField.snGrad());
        } else{
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getSurfaceNormalField(const string &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << "( " << pTraits<localVolField>::typeName << ")"
                    << " with the name " << name << " found or unsupported type for snGrad\n"
                    << "Fields of type: " 
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localVolField>()
                    << "\nAll registered objects:"
                    << patch_.boundaryMesh().mesh().objectRegistry::names()

                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::toFace(const Field<Type> &pField)
    {
        primitivePatchInterpolation inter(patch_.patch());
        
        tmp<Field<Type> > result=inter.pointToFaceInterpolate(pField);

        return result.ptr();
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::toPoint(const Field<Type> &fField)
    {
        primitivePatchInterpolation inter(patch_.patch());
        
        tmp<Field<Type> > result=inter.faceToPointInterpolate(fField);

        return result.ptr();
    }

}


// ************************************************************************* //
