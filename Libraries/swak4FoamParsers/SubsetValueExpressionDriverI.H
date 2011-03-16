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

    template<class FieldType,class Subset,class T>
    Field<T> *SubsetValueExpressionDriver::getFieldInternal(const string &name,const Subset &sub)
    {
        if(this->variables_.found(name)) {
            return this->variables_[name].getResult<T>(true).ptr();
        }

        Field<T> *result=new Field<T>(this->size());
        label cnt=0;
        
        if(this->searchInMemory() && this->mesh().foundObject<FieldType>(name)) {
            // Keep this separate to avoid copying if found in memory
            const FieldType &f=this->mesh().lookupObject<FieldType>(name);
            
            forAllConstIter(typename Subset,sub,iter) {
                (*result)[cnt]=f[getIndexFromIterator(iter)];
                cnt++;
            }
        } else {
            // otherwise try disc and memory
            autoPtr<FieldType> f=this->getOrReadField<FieldType>(name);

            forAllConstIter(typename Subset,sub,iter) {
                (*result)[cnt]=f()[getIndexFromIterator(iter)];
                cnt++;
            }            
        }
        assert(cnt==this->size());

        return result;
    }

    template<class FieldType,class IFieldType,class Subset,class T>
    Field<T> *SubsetValueExpressionDriver::getFieldInternalAndInterpolate(const string &name,const Subset &sub)
    {
        if(this->variables_.found(name)) {
            return this->variables_[name].getResult<T>(true).ptr();
        }

        Field<T> *result=new Field<T>(this->size());
        if(
            this->mesh().foundObject<FieldType>(name)
            ||
            !this->autoInterpolate_
        ) {
            const FieldType &f=this->mesh().lookupObject<FieldType>(name);
            label cnt=0;
            
            forAllConstIter(typename Subset,sub,iter) {
                (*result)[cnt]=f[getIndexFromIterator(iter)];
                cnt++;
            }
            
            assert(cnt==this->size());
        } else {
            if(this->warnAutoInterpolate_) {
                WarningIn("Field<T> *SubsetValueExpressionDriver::getFieldInternalAndInterpolate(const string &name,const Subset &sub)")
                    << "Going to interpolate " << name
                        << endl;

            }
            const IFieldType &f=this->mesh().lookupObject<IFieldType>(name);
            label cnt=0;

            const labelList &own=this->mesh().faceOwner();
            const labelList &nei=this->mesh().faceNeighbour();
            const surfaceScalarField &weights=this->mesh().weights();

            forAllConstIter(typename Subset,sub,iter) {
                label faceI=getIndexFromIterator(iter);
                if(faceI<this->mesh().nInternalFaces()) {
                    (*result)[cnt]=weights[faceI]*f[own[faceI]]+(1-weights[faceI])*f[nei[faceI]];
                } else {
                    // OK. We could also try to get the value from the original face
                    (*result)[cnt]=f[own[faceI]];
                }
                cnt++;
            }
            
            assert(cnt==this->size());
        }

        return result;
    }

    template<class Subset,class T>
    Field<T> *SubsetValueExpressionDriver::getFromFieldInternal(const Field<T> &original,const Subset &sub)
    {
        Field<T> *result=new Field<T>(this->size());
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            (*result)[cnt]=original[getIndexFromIterator(iter)];
            cnt++;
        }

        assert(cnt==this->size());

        return result;
    }

}


// ************************************************************************* //
