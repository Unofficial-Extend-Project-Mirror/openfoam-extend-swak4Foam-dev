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
//#include "primitivePatchInterpolation.H"
#include "interpolation.H"

#include <cassert>

namespace Foam {

    label SampledSetValueExpressionDriver::pointSize() const
    {
        FatalErrorIn("SampledSetValueExpressionDriver::pointSize()")
        << "sampledSets are a collection of points. So the 'cell' functionality should be used"
            << endl
            << abort(FatalError);
        
        return 0;
    }

    const fvMesh &SampledSetValueExpressionDriver::mesh() const
    {
        //        return dynamicCast<const fvMesh&>(faceZone_.zoneMesh().mesh()); // doesn't work with gcc 4.2
        return dynamic_cast<const fvMesh&>(theSet_.mesh());
    }

    template<class Type,class volField,class surfField>
    Field<Type> *SampledSetValueExpressionDriver::sampleOrInterpolateInternal(const word &name)
    {
        if(debug) {
            Info << "Sampling set: " << name << " on set " 
                << this->theSet_.name() << " with " << this->size() 
                << " points" << endl; 
            //            this->theSet_.write(Info);
            Info << "Search memory: " << this->searchInMemory()
                << " Field there: " << this->mesh().foundObject<volField>(name)
                << " Is variable: " << this->variables_.found(name)
                << endl;
        }
        if(this->variables_.found(name)) {
            return this->variables_[name].getResult<Type>(true).ptr();
        }

        Field<Type> *result=NULL;
        const sampledSet& samples = this->theSet_;

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
                
                result=new Field<Type>(this->theSet_.size());
                Field<Type> &values=*result;

                forAll(samples, samplei)
                {
                    const point& samplePt = samples[samplei];
                    label celli = samples.cells()[samplei];
                    label facei = samples.faces()[samplei];
                    
                    values[samplei] = inter().interpolate
                        (
                            samplePt,
                            celli,
                            facei
                        );
                }                
                if(debug) {
                    Info << "Interpolated values: " << values << endl;
                }
            } else {
                result=new Field<Type>(this->theSet_.size());
                Field<Type> &values=*result;
                const volField &field=this->mesh().lookupObject<volField>(name);

                forAll(samples, samplei)
                {
                    values[samplei] = field[samples.cells()[samplei]];
                }                
                if(debug) {
                    Info << "Values: " << values << endl;
                }
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
                
                result=new Field<Type>(this->theSet_.size());
                Field<Type> &values=*result;

                forAll(samples, samplei)
                {
                    const point& samplePt = samples[samplei];
                    label celli = samples.cells()[samplei];
                    label facei = samples.faces()[samplei];
                    
                    values[samplei] = inter().interpolate
                        (
                            samplePt,
                            celli,
                            facei
                        );
                }                
            } else {
                result=new Field<Type>(this->theSet_.size());
                Field<Type> &values=*result;
                const volField &field=f();

                forAll(samples, samplei)
                {
                    values[samplei] = field[samples.cells()[samplei]];
                }                
            }
        }

        if(result==NULL) {
            FatalErrorIn("SampledSetValueExpressionDriver::sampleOrInterpolateInternal(const word &name)")
                << "No valid result" 
                    << endl
                    << abort(FatalError);
        }

        return result;
    }

}


// ************************************************************************* //
