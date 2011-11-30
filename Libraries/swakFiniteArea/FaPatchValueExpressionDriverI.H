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

#include "edgeMesh.H"
#include "faePatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include <cassert>

namespace Foam {
 
    template<class Type>
    Field<Type> *FaPatchValueExpressionDriver::getField(const string &name)
    {
        typedef GeometricField<Type,faPatchField,areaMesh> localAreaField;
        typedef GeometricField<Type,faePatchField,edgeMesh> localEdgeField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        {
            Field<Type> *vals=NULL;
            
            if(variables_.found(name)) {
                vals=variables_[name].getResult<Type>(true).ptr();
            } else if(isGlobal<Type>(name,false)) {
                vals=const_cast<ExpressionResult &>(
                    lookupGlobal(name)
                ).getResult<Type>(true).ptr();
            }
            if(vals) {
                if(vals->size()>0) {
                    if(vals->size()==this->size()) {
                        return vals;
                    } else {
                        Field<Type> *result=new Field<Type>(this->size(),gAverage(*vals));
                        delete vals;
                        return result;
                    }
                }
            }
        }
    
        autoPtr<localAreaField> tmpVol;
        autoPtr<localEdgeField> tmpSurf;
        autoPtr<localPointField> tmpPoint;

        if(
            searchOnDisc()
            &&
            !mesh().foundObject<localAreaField>(name)
            &&
            !mesh().foundObject<localEdgeField>(name)
            &&
            !mesh().foundObject<localPointField>(name)
        ) {
            word fType=this->getTypeOfField(name);
            if(fType==localAreaField::typeName) {
                tmpVol.set(this->readAndRegister<localAreaField>(name).ptr());
            } else if(fType==localEdgeField::typeName) {
                tmpSurf.set(this->readAndRegister<localEdgeField>(name).ptr());
            } 
            // TODO: this constructor doesn't work
            // else if(fType==localPointField::typeName) {
            //     tmpPoint.set(this->readAndRegister<localPointField>(name).ptr());
            // }
        }

        if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localAreaField>(name)
        ) {
            const typename localAreaField::PatchFieldType &pField=
                patch_.lookupPatchField<localAreaField,Type>(name);
            return new Field<Type>(pField);
        } else if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localEdgeField>(name)
        ) {
            const typename localEdgeField::PatchFieldType &pField=
                patch_.lookupPatchField<localEdgeField,Type>(name);
            return new Field<Type>(pField);
        } else if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else{
            FatalErrorIn("Field<Type> *FaPatchValueExpressionDriver::getField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *FaPatchValueExpressionDriver::getPatchInternalField(const string &name)
    {
        typedef GeometricField<Type,faPatchField,areaMesh> localAreaField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
        }

        if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localAreaField>(name)
        ) {
            const typename localAreaField::PatchFieldType &pField=
                patch_.lookupPatchField<localAreaField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else{
            FatalErrorIn("Field<Type> *FaPatchValueExpressionDriver::getPatchInternalField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for patchInternalField\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *FaPatchValueExpressionDriver::getPatchNeighbourField(const string &name)
    {
        typedef GeometricField<Type,faPatchField,areaMesh> localAreaField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
        }

        if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localAreaField>(name)
        ) {
            const typename localAreaField::PatchFieldType &pField=
                patch_.lookupPatchField<localAreaField,Type>(name);
            return new Field<Type>(pField.patchNeighbourField());
        } else{
            FatalErrorIn("Field<Type> *FaPatchValueExpressionDriver::getPatchNeighbourField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for patchNeighbourField\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *FaPatchValueExpressionDriver::getSurfaceNormalField(const string &name)
    {
        typedef GeometricField<Type,faPatchField,areaMesh> localAreaField;

        if(
            patch_.boundaryMesh().mesh().thisDb().
            objectRegistry::foundObject<localAreaField>(name)
        ) {
            const typename localAreaField::PatchFieldType &pField=
                patch_.lookupPatchField<localAreaField,Type>(name);
            return new Field<Type>(pField.snGrad());
        } else{
            FatalErrorIn("Field<Type> *FaPatchValueExpressionDriver::getSurfaceNormalField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for lnGrad\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *FaPatchValueExpressionDriver::toFace(const Field<Type> &pField)
    {
        notImplemented("FaPatchValueExpressionDriver::toFace(const Field<Type> &pField)");

        // primitivePatchInterpolation inter(patch_.patch());        
        // tmp<Field<Type> > result=inter.pointToFaceInterpolate(pField);

        tmp<Field<Type> > result(new Field<Type>(0));

        return result.ptr();
    }

    template<class Type>
    Field<Type> *FaPatchValueExpressionDriver::toPoint(const Field<Type> &fField)
    {
        notImplemented("FaPatchValueExpressionDriver::toPoint(const Field<Type> &fField)");

        // primitivePatchInterpolation inter(patch_.patch());        
        // tmp<Field<Type> > result=inter.faceToPointInterpolate(fField);

        tmp<Field<Type> > result(new Field<Type>(0));

        return result.ptr();
    }

}


// ************************************************************************* //
