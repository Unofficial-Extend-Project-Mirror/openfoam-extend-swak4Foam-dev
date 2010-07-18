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
    tmp<Field<Type> > PatchValueExpressionDriver::evaluate(const string &expr,bool qPoint)
    {
        parse(expr);

        return getResult<Type>(qPoint);
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::makeField(Type val,bool isPoint)
    {
        label size=patch_.size();
        if(isPoint) {
            size=patch_.patch().nPoints();
        }
        return new Field<Type>(size,val);
    }

    template<class Type>
    void PatchValueExpressionDriver::setResult(Field<Type> *val,bool isPoint)
    {
        result_.setResult<Type>(val,isPoint);
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::getResult(bool qPoint)
    {
        if(qPoint!=result_.isPoint()) {
            FatalErrorIn("PatchValueExpressionDriver::getResult(bool qPoint)")
                << "A " << (qPoint ? "point" : "face" ) << "-field was wanted"
                    << " but we got a " << (!qPoint ? "point" : "face" ) 
                    << endl
                    << abort(FatalError);
        }

        return result_.getResult<Type>();
    }

    template<class Type>
    bool PatchValueExpressionDriver::is(const string &name,bool isPoint) const
    {
        if(variables_.found(name)) {
            if(
                variables_[name].type()==pTraits<Type>::typeName 
                && 
                variables_[name].isPoint()==isPoint
            ) {
                return true;
            } else {
                return false;
            }
        } else {
            return isField<Type>(name,isPoint);
        }
    }

    template<class Type>
    bool PatchValueExpressionDriver::isField(const string &name,bool isPoint) const
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(isPoint) {
            if(
                patch_.boundaryMesh().mesh().
                objectRegistry::foundObject<localPointField>(name)
            ) {
                return true;
            } else {
                return false;
            }
        } else {
            if(
                patch_.boundaryMesh().mesh().
                objectRegistry::foundObject<localVolField>(name)
            ) {
                return true;
            } else if(
                patch_.boundaryMesh().mesh().
                objectRegistry::foundObject<localSurfaceField>(name)
            ) {
                return true;
            } else{
                return false;
            }
        }
    }

    inline bool PatchValueExpressionDriver::isLine(const string &name) const 
    {
        return lines_.found(name);
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getField(const string &name)
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
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << abort(FatalError);

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
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for patchInternalField\n"
                    << endl
                    << abort(FatalError);

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

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField.patchNeighbourField());
        } else{
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getPatchNeighbourField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for patchNeighbourField\n"
                    << endl
                    << abort(FatalError);

            return NULL;
        }
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::getSurfaceNormalField(const string &name)
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
            FatalErrorIn("Field<Type> *PatchValueExpressionDriver::getSurfaceNormalField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found or unsupported type for snGrad\n"
                    << endl
                    << abort(FatalError);

            return NULL;
        }
    }

    template <class Op,class Type>
    Field<bool> *PatchValueExpressionDriver::doCompare(Field<Type> *a,Op op,Field<Type> *b)
    {
        assert(a->size()==b->size());

        Field<bool> *res=new Field<bool>(a->size(),false);

        forAll(*res,i) {
            (*res)[i]=op((*a)[i],(*b)[i]);
        }
        
        return res;
    }

    template <class Op>
    Field<bool> *PatchValueExpressionDriver::doLogicalOp(Field<bool> *a,Op op,Field<bool> *b)
    {
        assert(a->size()==b->size());

        Field<bool> *res=new Field<bool>(a->size(),false);

        forAll(*res,i) {
            (*res)[i]=op((*a)[i],(*b)[i]);
        }
        
        return res;
    }

    template<class Type>
    Field<Type> *PatchValueExpressionDriver::doConditional(Field<bool> *d,Field<Type> *yes,Field<Type> *no)
    {
        assert(yes->size()==no->size() && d->size()==yes->size());

        Field<Type> *res=new Field<Type>(yes->size());

        forAll(*res,i) {
            (*res)[i] = (*d)[i] ? (*yes)[i] : (*no)[i];
        }

        return res;
    }

    Field<bool> *PatchValueExpressionDriver::doLogicalNot(Field<bool> *a)
    {
        Field<bool> *res=new Field<bool>(a->size());

        forAll(*res,i) {
            (*res)[i] = (*a)[i];
        }

        return res;
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
