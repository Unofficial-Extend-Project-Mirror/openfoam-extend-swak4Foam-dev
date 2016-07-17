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

Contributors/Copyright:
    2010, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "edgeMesh.H"
#include "faePatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include <cassert>

namespace Foam {
 
    template<class T>
    inline tmp<T> FaCommonValueExpressionDriver::readAndRegister(const word &name)
    {
        tmp<T>f(
            new T(
                IOobject
                (
                    name,
                    this->time(),
                    this->mesh(),
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE
                ),
                aMesh()
            )
        );
        if(cacheReadFields()) {
            if(debug) {
                Info << "Registering a copy of " << name << " with mesh" << endl;
            }
            T* toCache=new T(f());
            toCache->store();
        }
        return f;
    }

    template<class Type>
    bool FaCommonValueExpressionDriver::is(const string &name,bool isPoint) const
    {
        if(isVariable<Type>(name,isPoint)) {
            return true;
        } else {
            return isField<Type>(name,isPoint);
        }
    }

    template<class Type>
    bool FaCommonValueExpressionDriver::isField(const string &name,bool isPoint) const
    {
        typedef GeometricField<Type,faPatchField,areaMesh> localAreaField;
        typedef GeometricField<Type,faePatchField,edgeMesh> localEdgeField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(isPoint) {
            if(
                isThere<localPointField>(name)
            ) {
                return true;
            } else {
                return false;
            }
        } else {
            if(
                isThere<localAreaField>(name)
            ) {
                return true;
            } else if(
                isThere<localEdgeField>(name)
            ) {
                return true;
            } else{
                return false;
            }
        }
    }

}


// ************************************************************************* //
