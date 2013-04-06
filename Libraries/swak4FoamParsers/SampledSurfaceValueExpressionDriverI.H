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
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
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
    tmp<Field<Type> >
    SampledSurfaceValueExpressionDriver::sampleOrInterpolateInternal(
        const word &name,
        bool oldTime
    )
    {
        if(
            this->hasVariable(name)
            &&
            this->variable(name).valueType()==pTraits<Type>::typeName
        ) {
            return this->variable(name).getResult<Type>(true);
        }
        autoPtr<Field<Type> > result;

        if(
            this->searchInMemory()
            &&
            this->mesh().foundObject<volField>(name)
        ) {
            if(interpolate_) {
                autoPtr<interpolation<Type> > inter(
                    interpolation<Type>::New(
                        interpolationType_,
                        (
                            !oldTime ?
                            this->mesh().lookupObject<volField>(name) :
                            this->mesh().lookupObject<volField>(name).oldTime()
                        )
                    )
                );

                result.set(
                    new Field<Type>(
                        this->theSurface_.interpolate(
                            inter()
                        )
                    )
                );
            } else {
                result.set(
                    new Field<Type>(
                        this->theSurface_.sample(
                            (
                                !oldTime ?
                                this->mesh().lookupObject<volField>(name) :
                                this->mesh().lookupObject<volField>(name).oldTime()
                            )
                        )
                    )
                );
            }
        } else {
            autoPtr<volField> f=this->getOrReadField<volField>(
                name,
                true, // fail if not found
                oldTime
            );

            if(interpolate_) {
                autoPtr<interpolation<Type> > inter(
                    interpolation<Type>::New(
                        interpolationType_,
                        (
                            !oldTime ?
                            f() :
                            f->oldTime()
                        )
                    )
                );

                result.set(
                    new Field<Type>(
                        this->theSurface_.interpolate(
                            inter()
                        )
                    )
                );
            } else {
                result.set(
                    new Field<Type>(
                        this->theSurface_.sample(
                            (
                                !oldTime ?
                                f() :
                                f->oldTime()
                            )
                        )
                    )
                );
            }
        }

        if(!result.valid()) {
            FatalErrorIn("SampledSurfaceValueExpressionDriver::sampleOrInterpolateInternal(const word &name)")
                << "No valid result"
                    << endl
                    << exit(FatalError);
        }

        if(debug) {
            Pout << "SampledSurfaceValueExpressionDriver::sampleOrInterpolateInternal()"
                << "Type: " << pTraits<Type>::typeName
                << " name: " << name
                << " size: " << this->size() << " result: "
                << result().size()
                << endl;
        }

        return tmp<Field<Type> >(result.ptr());
    }

}


// ************************************************************************* //
