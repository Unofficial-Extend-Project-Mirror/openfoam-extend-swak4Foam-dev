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
    Field<Type> *SubsetValueExpressionDriver::getField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
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
            FatalErrorIn("Field<Type> *SubsetValueExpressionDriver::getField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << abort(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *SubsetValueExpressionDriver::getSubsetInternalField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
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
            FatalErrorIn("Field<Type> *SubsetValueExpressionDriver::getSubsetInternalField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for subsetInternalField\n"
                    << endl
                    << abort(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *SubsetValueExpressionDriver::getSubsetNeighbourField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField.patchNeighbourField());
        } else{
            FatalErrorIn("Field<Type> *SubsetValueExpressionDriver::getSubsetNeighbourField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for subsetNeighbourField\n"
                    << endl
                    << abort(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *SubsetValueExpressionDriver::getSurfaceNormalField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField.snGrad());
        } else{
            FatalErrorIn("Field<Type> *SubsetValueExpressionDriver::getSurfaceNormalField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for snGrad\n"
                    << endl
                    << abort(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *SubsetValueExpressionDriver::toFace(const Field<Type> &pField)
    {
        primitivePatchInterpolation inter(patch_.patch());
        
        tmp<Field<Type> > result=inter.pointToFaceInterpolate(pField);

        return result.ptr();
    }

    template<class Type>
    Field<Type> *SubsetValueExpressionDriver::toPoint(const Field<Type> &fField)
    {
        primitivePatchInterpolation inter(patch_.patch());
        
        tmp<Field<Type> > result=inter.faceToPointInterpolate(fField);

        return result.ptr();
    }

}


// ************************************************************************* //
