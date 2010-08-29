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

        const FieldType &f=this->mesh().lookupObject<FieldType>(name);
        Field<T> *result=new Field<T>(this->size());
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            (*result)[cnt]=f[*iter];
            cnt++;
        }

        assert(cnt==this->size());

        return result;
    }

    template<class Subset,class T>
    Field<T> *SubsetValueExpressionDriver::getFromFieldInternal(const Field<T> &original,const Subset &sub)
    {
        Field<T> *result=new Field<T>(this->size());
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            (*result)[cnt]=original[*iter];
            cnt++;
        }

        assert(cnt==this->size());

        return result;
    }

}


// ************************************************************************* //
