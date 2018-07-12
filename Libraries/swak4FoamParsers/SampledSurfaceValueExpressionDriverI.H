/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2010-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "surfaceMesh.H"
#include "fvsPatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include <cassert>

namespace Foam {
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
        } else if(
            this->isGlobal<Type>(name,false)
        ) {
            const ExpressionResult &globalVar=this->lookupGlobal(name);
            if(!globalVar.isSingleValue()) {
                return const_cast<ExpressionResult&>(
                    globalVar
                ).getResult<Type>(true);
            }
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
#ifdef FOAM_SAMPLEDSURFACE_SAMPLE_WANTS_INTERPOLATION
                                interpolation<Type>::New(
                                    interpolationType_,
                                    (
#endif
                                        !oldTime ?
                                        this->mesh().lookupObject<volField>(name) :
                                        this->mesh().lookupObject<volField>(name).oldTime()
#ifdef FOAM_SAMPLEDSURFACE_SAMPLE_WANTS_INTERPOLATION
                                    )
                                )
#endif
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
#ifdef FOAM_SAMPLEDSURFACE_SAMPLE_WANTS_INTERPOLATION
                                interpolation<Type>::New(
                                    interpolationType_,
                                    (
#endif
                                        !oldTime ?
                                        f() :
                                        f->oldTime()
#ifdef FOAM_SAMPLEDSURFACE_SAMPLE_WANTS_INTERPOLATION
                                    )
                                )
#endif
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
