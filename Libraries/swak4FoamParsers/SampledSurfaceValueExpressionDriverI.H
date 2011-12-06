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
    label SampledSurfaceValueExpressionDriver::size() const
    {
        return theSurface_.faces().size();
    }

    label SampledSurfaceValueExpressionDriver::pointSize() const
    {
        return theSurface_.points().size();
    }

    const fvMesh &SampledSurfaceValueExpressionDriver::mesh() const
    {
        //        return dynamicCast<const fvMesh&>(faceZone_.zoneMesh().mesh()); // doesn't work with gcc 4.2
        return dynamic_cast<const fvMesh&>(theSurface_.mesh());
    }

    template<class Type,class volField,class surfField>
    Field<Type> *SampledSurfaceValueExpressionDriver::sampleOrInterpolateInternal(const word &name)
    {
        if(this->variables_.found(name)) {
            return this->variables_[name].getResult<Type>(true).ptr();
        }
        Field<Type> *result=NULL;

        if(
            this->searchInMemory() 
            && 
            this->mesh().foundObject<volField>(name)
        ) {
            if(interpolate_) {
                autoPtr<interpolation<Type> > inter(
                    interpolation<Type>::New(
                        interpolationType_,
                        this->mesh().lookupObject<volField>(name)
                    )
                );
                
                result=new Field<Type>(
                    this->theSurface_.interpolate(
                        inter()
                    )
                );
            } else {
                result=new Field<Type>(
                    this->theSurface_.sample(
                        this->mesh().lookupObject<volField>(name)
                    )
                );
            }
        } else {
            autoPtr<volField> f=this->getOrReadField<volField>(name);

            if(interpolate_) {
                autoPtr<interpolation<Type> > inter(
                    interpolation<Type>::New(
                        interpolationType_,
                        f()
                    )
                );
                
                result=new Field<Type>(
                    this->theSurface_.interpolate(
                        inter()
                    )
                );
            } else {
                result=new Field<Type>(
                    this->theSurface_.sample(
                        f()
                    )
                );
            }
        }

        if(result==NULL) {
            FatalErrorIn("SampledSurfaceValueExpressionDriver::sampleOrInterpolateInternal(const word &name)")
                << "No valid result" 
                    << endl
                    << exit(FatalError);
        }

        return result;
    }

}


// ************************************************************************* //
