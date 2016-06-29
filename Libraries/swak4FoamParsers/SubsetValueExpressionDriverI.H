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
    2010-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "surfaceMesh.H"
#include "fvsPatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include "swak.H"

#ifdef FOAM_COUPLEDPOLYPATCH_HAS_OWNER
#include "coupledPolyPatch.H"
#else
#include "processorPolyPatch.H"
#endif

#include <cassert>

namespace Foam {

    template<class FieldType,class Subset,class T>
    tmp<Field<T> > SubsetValueExpressionDriver::getFieldInternal(
        const word &inName,
        const Subset &sub,
        bool oldTime
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "SubsetValueExpressionDriver::getFieldInternal. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        if(debug) {
            Pout << "SubsetValueExpressionDriver::getFieldInternal"
                << " searching for " << name << " Type: " << Subset::typeName
                << endl;
        }

        if(
            this->hasVariable(name)
            &&
            this->variable(name).valueType()==pTraits<T>::typeName
        ) {
            if(debug) {
                Pout << name << " is a variable" << endl;
            }
            return this->variable(name).getResult<T>(true);
        }

        tmp<Field<T> > result(new Field<T>(this->size()));
        label cnt=0;

        if(debug) {
            Pout << "searchInMemory: " << this->searchInMemory()
                << " found: " << this->mesh().foundObject<FieldType>(name)
                << endl;
        }

        if(this->searchInMemory() && this->mesh().foundObject<FieldType>(name)) {
            if(debug) {
                Pout << name << " was found in memory" << endl;
            }
            // Keep this separate to avoid copying if found in memory
            if(!oldTime) {
                const FieldType &f=this->mesh().lookupObject<FieldType>(name);

                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f[getIndexFromIterator(iter)];
                    cnt++;
                }
            } else {
                const FieldType &f=this->mesh().lookupObject<FieldType>(name).oldTime();

                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f[getIndexFromIterator(iter)];
                    cnt++;
                }
            }
        } else {
            if(debug) {
                Pout << name << " was found on disc" << endl;
            }
            // otherwise try disc and memory
            autoPtr<FieldType> f=this->getOrReadField<FieldType>(
                name,
                true, // fail if not found
                oldTime
            );

            if(!oldTime) {
                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f()[getIndexFromIterator(iter)];
                    cnt++;
                }
            } else {
                forAllConstIter(typename Subset,sub,iter) {
                    result()[cnt]=f->oldTime()[getIndexFromIterator(iter)];
                    cnt++;
                }
            }
        }
        assert(cnt==this->size());

        return result;
    }

    template<class FieldType,class IFieldType,class Subset,class T>
    tmp<Field<T> > SubsetValueExpressionDriver::getFieldInternalAndInterpolate(
        const word &inName,
        const Subset &sub,
        bool oldTime
    )
    {
        // Yeah. I know. Code duplication without end

        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "SubsetValueExpressionDriver::getFieldInternalAndInterpolate. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }
        if(
            this->hasVariable(name)
            &&
            this->variable(name).valueType()==pTraits<T>::typeName
        ) {
            return this->variable(name).getResult<T>(true);
        }

        tmp<Field<T> > result(new Field<T>(this->size()));

        bool done=false;

        if(
            this->searchInMemory()
            &&
            this->mesh().foundObject<FieldType>(name)
        ) {
            const FieldType &f=this->mesh().lookupObject<FieldType>(name);
            label cnt=0;

            forAllConstIter(typename Subset,sub,iter) {
                const label index=getIndexFromIterator(iter);
                if(index<this->mesh().nInternalFaces()) {
                    if(!oldTime) {
                        result()[cnt]=f[index];
                    } else {
                        result()[cnt]=f.oldTime()[index];
                    }
                    cnt++;
                } else {
                    T value;
                    bool use;
                    if(!oldTime) {
                        use=getBoundaryFaceValue(f,index,value);
                    } else {
                        use=getBoundaryFaceValue(f.oldTime(),index,value);
                    }
                    if(use) {
                        result()[cnt]=value;
                        cnt++;
                    }
                }
            }

            assert(cnt==this->size());

            done=true;
        } else {
            autoPtr<FieldType> f=this->getOrReadField<FieldType>(
                name,
                false,
                oldTime
            );
            if(f.valid()) {
                label cnt=0;
                forAllConstIter(typename Subset,sub,iter) {
                    const label index=getIndexFromIterator(iter);
                    if(index<this->mesh().nInternalFaces()) {
                        if(!oldTime) {
                            result()[cnt]=f()[index];
                        } else {
                            result()[cnt]=f->oldTime()[index];
                        }
                        cnt++;
                    } else {
                        T value;
                        bool use;
                        if(!oldTime) {
                            use=getBoundaryFaceValue(f(),index,value);
                        } else {
                            use=getBoundaryFaceValue(f->oldTime(),index,value);
                        }
                        if(use) {
                            result()[cnt]=value;
                            cnt++;
                        }
                    }
                }
                done=true;

                assert(cnt==this->size());
            }
        }

        if(
            !done
            &&
            this->autoInterpolate_
        ) {
            if(
                this->warnAutoInterpolate_
            ) {
                WarningIn("Field<T> *SubsetValueExpressionDriver::getFieldInternalAndInterpolate(const word &name,const Subset &sub)")
                    << "Going to interpolate " << name
                        << endl;
            }

            const labelList &own=this->mesh().faceOwner();
            const labelList &nei=this->mesh().faceNeighbour();
            const surfaceScalarField &weights=this->mesh().weights();

            if(
                this->searchInMemory()
                &&
                this->mesh().foundObject<IFieldType>(name)
            ) {
                const IFieldType &f=this->mesh().lookupObject<IFieldType>(name);
                label cnt=0;

                forAllConstIter(typename Subset,sub,iter) {
                    label faceI=getIndexFromIterator(iter);
                    if(faceI<this->mesh().nInternalFaces()) {
                        if(!oldTime) {
                            result()[cnt]=
                                weights[faceI]*f[own[faceI]]
                                +
                                (1-weights[faceI])*f[nei[faceI]];
                        } else {
                            result()[cnt]=
                                weights[faceI]*f.oldTime()[own[faceI]]
                                +
                                (1-weights[faceI])*f.oldTime()[nei[faceI]];
                        }
                        cnt++;
                    } else {
                        T value;
                        bool use;
                        if(!oldTime) {
                            use=getBoundaryFaceValue(f,faceI,value);
                        } else {
                            use=getBoundaryFaceValue(f.oldTime(),faceI,value);
                        }
                        if(use) {
                            result()[cnt]=value;
                            cnt++;
                        }
                    }
                }

                assert(cnt==this->size());

                done=true;
            } else {
                autoPtr<IFieldType> f=this->getOrReadField<IFieldType>(
                    name,
                    false,
                    oldTime
                );

                if(f.valid()) {
                    label cnt=0;

                    forAllConstIter(typename Subset,sub,iter) {
                        label faceI=getIndexFromIterator(iter);
                        if(faceI<this->mesh().nInternalFaces()) {
                            if(!oldTime) {
                                result()[cnt]=
                                    weights[faceI]*f()[own[faceI]]
                                    +
                                    (1-weights[faceI])*f()[nei[faceI]];
                            } else {
                                result()[cnt]=
                                    weights[faceI]*f->oldTime()[own[faceI]]
                                    +
                                    (1-weights[faceI])*f->oldTime()[nei[faceI]];
                            }
                            cnt++;
                        } else {
                            T value;
                            bool use;
                            if(!oldTime) {
                                use=getBoundaryFaceValue(f(),faceI,value);
                            } else {
                                use=getBoundaryFaceValue(f->oldTime(),faceI,value);
                            }
                            if(use) {
                                result()[cnt]=value;
                                cnt++;
                            }
                        }
                    }

                    assert(cnt==this->size());

                    done=true;
                }
            }
        }

        if(!done) {
            FatalErrorIn("SubsetValueExpressionDriver::getFieldInternalAndInterpolate(const word &name,const Subset &sub)")
                << "Could not find a field name " << name
                    << " of type " << pTraits<T>::typeName
                    << " (neither " << FieldType::typeName
                    << " nor " << IFieldType::typeName << ")"
                    << " autoInterpolate: " << this->autoInterpolate_
                    << (this->autoInterpolate_ ? "" : " (try setting 'autoInterpolate' to 'true')")
                    << endl
                    << exit(FatalError);
        }

        return result;
    }

    template<class Subset,class T>
    tmp<Field<T> > SubsetValueExpressionDriver::getFromFieldInternal(
        const Field<T> &original,
        const Subset &sub
    ) const
    {
        tmp<Field<T> > result(new Field<T>(this->size()));
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            result()[cnt]=original[getIndexFromIterator(iter)];
            cnt++;
        }

        assert(cnt==this->size());

        return result;
    }

    template<class Subset,class T>
    tmp<Field<T> > SubsetValueExpressionDriver::getFromSurfaceFieldInternal(
        const GeometricField<T,fvsPatchField,surfaceMesh> &original,
        const Subset &sub
    ) const
    {
        tmp<Field<T> > result(new Field<T>(this->size()));
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            label faceI=getIndexFromIterator(iter);
            if(faceI<this->mesh().nInternalFaces()) {
                result()[cnt]=original[faceI];
                cnt++;
            } else {
                T value;
                bool use;
                use=getBoundaryFaceValue(original,faceI,value);
                if(use) {
                    result()[cnt]=value;
                    cnt++;
                }
            }
        }

        assert(cnt==this->size());

        return result;
    }

    bool SubsetValueExpressionDriver::useFaceValue(
        const label faceI
    ) const
    {
        if(faceI<this->mesh().nInternalFaces()) {
            return true;
        } else {
            label patchI=this->mesh().boundaryMesh().whichPatch(faceI);
            if(patchI<0) {
                FatalErrorIn("SubsetValueExpressionDriver::useFaceValue")
                    << "Can't find the correct patch for face " << faceI
                        << endl
                        << exit(FatalError);
            }
            const polyPatch &patch=this->mesh().boundaryMesh()[patchI];
            if(!patch.coupled()) {
                return true;
            } else {
#ifdef FOAM_COUPLEDPOLYPATCH_HAS_OWNER
                if(isA<coupledPolyPatch>(patch)) {
                    const coupledPolyPatch &pp=dynamicCast<const coupledPolyPatch&>(
                        patch
                    );
#else
                if(isA<processorPolyPatch>(patch)) {
                    const processorPolyPatch &pp=dynamicCast<const processorPolyPatch&>(
                        patch
                    );
#endif
                    return pp.owner();
                } else {
                    return false;
                }
            }
        }
    }

    template<class FieldType,class T>
    bool SubsetValueExpressionDriver::getBoundaryFaceValue(
        const FieldType &field,
        const label index,
        T &value
    ) const
    {
        value=pTraits<T>::zero;

        if(index<this->mesh().nInternalFaces()) {
            FatalErrorIn("SubsetValueExpressionDriver::getBoundaryFaceValue")
                << "Face with index " << index << " is an internal face (<"
                    << this->mesh().nInternalFaces() << ")"
                    << endl
                    << exit(FatalError);
        }
        label patchI=this->mesh().boundaryMesh().whichPatch(index);

        if(useFaceValue(index)) {
            const polyPatch &patch=this->mesh().boundaryMesh()[patchI];
            const typename FieldType::PatchFieldType &bf=
                field.boundaryField()[patchI];
            value=bf[patch.whichFace(index)];
            return true;
        } else {
            return false;
        }
    }

    template<class Subset>
    label SubsetValueExpressionDriver::nrValidFaces(const Subset &sub) const
    {
        label cnt=0;

        forAllConstIter(typename Subset,sub,iter) {
            const label faceI=getIndexFromIterator(iter);
            if(useFaceValue(faceI)) {
                cnt++;
            }
        }

        return cnt;
    }
}


// ************************************************************************* //
