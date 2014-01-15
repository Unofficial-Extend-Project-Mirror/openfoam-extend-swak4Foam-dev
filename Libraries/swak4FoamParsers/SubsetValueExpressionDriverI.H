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
                if(!oldTime) {
                    result()[cnt]=f[getIndexFromIterator(iter)];
                } else {
                    result()[cnt]=f.oldTime()[getIndexFromIterator(iter)];
                }
                cnt++;
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
                    if(!oldTime) {
                        result()[cnt]=f()[getIndexFromIterator(iter)];
                    } else {
                        result()[cnt]=f->oldTime()[getIndexFromIterator(iter)];
                    }
                    cnt++;
                }
                done=true;
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
                    } else {
                        // OK. We could also try to get the value from the original face
                        if(!oldTime) {
                            result()[cnt]=f[own[faceI]];
                        } else {
                            result()[cnt]=f.oldTime()[own[faceI]];
                        }
                    }
                    cnt++;
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
                        } else {
                            // OK. We could also try to get the value from the original face
                            if(!oldTime) {
                                result()[cnt]=f()[own[faceI]];
                            } else {
                                result()[cnt]=f->oldTime()[own[faceI]];
                            }
                        }
                        cnt++;
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

}


// ************************************************************************* //
