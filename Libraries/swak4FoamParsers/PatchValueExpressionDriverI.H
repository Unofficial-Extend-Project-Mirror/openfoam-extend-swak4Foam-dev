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
    2009-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
    2014 Hrvoje Jasak <h.jasak@wikki.co.uk>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "surfaceMesh.H"
#include "fvsPatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include "swak.H"

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
#include "directMappedPatchBase.H"
#include "interpolationCell.H"

#include "mapDistribute.H"

namespace Foam {
    // these typedefs should keep the difference between the 1.7 and the 2.1 code minimal
    typedef directMappedPatchBase mappedPatchBase;
}

#else
#include "mappedPatchBase.H"
#include "interpolationCell.H"
#endif

#include <cassert>

namespace Foam {

   template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::getField(const word &inName)
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::getField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        {
            autoPtr<Field<Type> >vals;
            bool isPoint=false;
            bool isSingleValue=false;

            if(
                hasVariable(name)
                &&
                variable(name).valueType()==pTraits<Type>::typeName
            ) {
                isPoint=variable(name).isPoint();
                isSingleValue=variable(name).isSingleValue();
                vals.set(
                    variable(name).getResult<Type>(true).ptr()
                );
            } else if(isGlobal<Type>(name,false)) {
                ExpressionResult &var=const_cast<ExpressionResult &>(
                    lookupGlobal(name)
                );
                isSingleValue=var.isSingleValue();
                vals.set(
                    var.getResult<Type>(true).ptr()
                );
            }
            if(vals.valid()) {
                bool qSize=vals->size()==this->size();
                bool qPSize=vals->size()==this->pointSize();
                reduce(qSize,andOp<bool>());
                reduce(qPSize,andOp<bool>());

                if(
                    (
                        ! isPoint
                        &&
                        qSize
                    )
                    ||
                    (
                        isPoint
                        &&
                        qPSize
                    )
                ) {
                    return tmp<Field<Type> >(
                        vals.ptr()
                    );
                } else {
                    if(!isSingleValue) {
                        WarningIn("PatchValueExpressionDriver::getField(const word &inName)")
                            << "Variable " << name
                                << " is not a single value but does not "
                                << "fit the size. Using average and "
                                << "hoping for the best"
                                << endl;
                    }
                    return tmp<Field<Type> >(
                        new Field<Type>(
                            this->size(),gAverage(vals())
                        )
                    );
                }
            }
        }

        autoPtr<localVolField> tmpVol;
        autoPtr<localSurfaceField> tmpSurf;
        autoPtr<localPointField> tmpPoint;

        if(
            searchOnDisc()
            &&
            !mesh().foundObject<localVolField>(name)
            &&
            !mesh().foundObject<localSurfaceField>(name)
            &&
            !mesh().foundObject<localPointField>(name)
        ) {
            word fType=this->getTypeOfField(name);
            if(fType==localVolField::typeName) {
                tmpVol.set(
                    this->readAndRegister<localVolField>(
                        name,
                        mesh()
                    ).ptr()
                );
            } else if(fType==localSurfaceField::typeName) {
                tmpSurf.set(
                    this->readAndRegister<localSurfaceField>(
                        name,
                        mesh()
                    ).ptr()
                );
            }
            // TODO: this constructor doesn't work
            // else if(fType==localPointField::typeName) {
            //     tmpPoint.set(this->readAndRegister<localPointField>(name,mesh()).ptr());
            // }
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);

            return tmp<Field<Type> >(
                new Field<Type>(pField)
            );
        } else if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localSurfaceField>(name)
        ) {
            const typename localSurfaceField::PatchFieldType &pField=
                patch_.lookupPatchField<localSurfaceField,Type>(name);

            return tmp<Field<Type> >(
                new Field<Type>(pField)
            );
        } else if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);

            return pField.patchInternalField();
        } else{
            FatalErrorIn("tmp<Field<Type> > PatchValueExpressionDriver::getField(const word &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << "Fields of type: " << localVolField::typeName
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localVolField>()
                    << endl
                    << "Fields of type: " << localSurfaceField::typeName
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localSurfaceField>()
                    << endl
                    << "Fields of type: " << localPointField::typeName
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localPointField>()
                    << endl
                    // << "All registered objects:"
                    // << patch_.boundaryMesh().mesh().objectRegistry::names()
                    // << endl
                    << exit(FatalError);

            return tmp<Field<Type> >(
                new Field<Type>(0)
            );
        }
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::getOldTimeField(
        const word &inName
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::getOldTimeField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh>
            localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        autoPtr<localVolField> tmpVol;
        autoPtr<localSurfaceField> tmpSurf;
        autoPtr<localPointField> tmpPoint;

        if(
            searchOnDisc()
            &&
            !mesh().foundObject<localVolField>(name)
            &&
            !mesh().foundObject<localSurfaceField>(name)
            &&
            !mesh().foundObject<localPointField>(name)
        ) {
            word fType=this->getTypeOfField(name);
            if(fType==localVolField::typeName) {
                tmpVol.set(
                    this->readAndRegister<localVolField>(name,mesh()).ptr()
                );
            } else if(fType==localSurfaceField::typeName) {
                tmpSurf.set(
                    this->readAndRegister<localSurfaceField>(name,mesh()).ptr()
                );
            }
            // TODO: this constructor doesn't work
            // else if(fType==localPointField::typeName) {
            //     tmpPoint.set(this->readAndRegister<localPointField>(name,mesh()).ptr());
            // }
        }

        label patchI=this->patch_.index();
        const fvMesh &mesh=patch_.boundaryMesh().mesh();

        if(
            mesh.objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                mesh.lookupObject<localVolField>(name).
                oldTime().boundaryField()[patchI];
            return tmp<Field<Type> >(
                new Field<Type>(pField)
            );
        } else if(
            mesh.objectRegistry::foundObject<localSurfaceField>(name)
        ) {
            const typename localSurfaceField::PatchFieldType &pField=
                mesh.lookupObject<localSurfaceField>(name).
                oldTime().boundaryField()[patchI];
            return tmp<Field<Type> >(
                new Field<Type>(pField)
            );
        } else if(
            mesh.objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                mesh.lookupObject<localPointField>(name).
                oldTime().boundaryField()[patchI];
            return tmp<Field<Type> >(
                new Field<Type>(pField.patchInternalField())
            );
        } else{
            FatalErrorIn("tmp<Field<Type> > PatchValueExpressionDriver::getOldTimeField(const word &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << exit(FatalError);

            return tmp<Field<Type> >(new Field<Type>(0));;
        }
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::getPatchInternalField(
        const word &inName
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::getPatchInternalField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(
            hasVariable(name)
            &&
            variable(name).valueType()==pTraits<Type>::typeName
        ) {
            return variable(name).getResult<Type>(true);
        }

        autoPtr<localVolField> dummy; // hold on to the field until the calculation is finished
        if(
            !patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
            &&
            searchOnDisc()
        ) {
            dummy=this->getOrReadField<localVolField>(name,false);
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return pField.patchInternalField();
        } else if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);
            return pField.patchInternalField();
        } else{
            FatalErrorIn("tmp<Field<Type> > PatchValueExpressionDriver::getPatchInternalField(const word &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name << " found or unsupported type for patchInternalField\n"
                    << endl
                    << exit(FatalError);

            return tmp<Field<Type> >(
                new Field<Type>(0)
            );
        }
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::getPatchNeighbourField(
        const word &inName
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::getPatchNeighbourField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;

        if(
            hasVariable(name)
            &&
            variable(name).valueType()==pTraits<Type>::typeName
        ) {
            return variable(name).getResult<Type>(true);
        }

        autoPtr<localVolField> dummy; // hold on to the field until the calculation is finished
        if(
            !patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
            &&
            searchOnDisc()
        ) {
            dummy=this->getOrReadField<localVolField>(name,false);
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return pField.patchNeighbourField();
        } else{
            FatalErrorIn("tmp<Field<Type> > PatchValueExpressionDriver::getPatchNeighbourField(const word &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << " with the name " << name
                    << " found or unsupported type for patchNeighbourField\n"
                    << endl
                    << exit(FatalError);

            return tmp<Field<Type> >(
                new Field<Type>(0)
            );
        }
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::getSurfaceNormalField(
        const word &inName
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::getSurfaceNormalField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;

        autoPtr<localVolField> dummy; // hold on to the field until the calculation is finished
        if(
            !patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
            &&
            searchOnDisc()
        ) {
            dummy=this->getOrReadField<localVolField>(name,false);
        }

        if(
            patch_.boundaryMesh().mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return pField.snGrad();
        } else{
            FatalErrorIn("tmp<Field<Type> > PatchValueExpressionDriver::getSurfaceNormalField(const word &name)")
                << "No field of type " << pTraits<Type>::typeName
                    << "( " << pTraits<localVolField>::typeName << ")"
                    << " with the name " << name
                    << " found or unsupported type for snGrad\n"
                    << "Fields of type: "
                    << patch_.boundaryMesh().mesh().objectRegistry::names<localVolField>()
                    << "\nAll registered objects:"
                    << patch_.boundaryMesh().mesh().objectRegistry::names()

                    << endl
                    << exit(FatalError);

            return tmp<Field<Type> >(
                new Field<Type>(0)
            );
        }
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::toFace(
        const Field<Type> &pField
    )
    {
        primitivePatchInterpolation inter(patch_.patch());

        return inter.pointToFaceInterpolate(pField);
    }

    template<class Type>
    tmp<Field<Type> > PatchValueExpressionDriver::toPoint(
        const Field<Type> &fField
    )
    {
        primitivePatchInterpolation inter(patch_.patch());

        return inter.faceToPointInterpolate(fField);
    }

    template<class Type>
    const interpolation<Type> &PatchValueExpressionDriver::getInterpolation(
        const GeometricField< Type, fvPatchField, volMesh > &psi
    )
    {
        const word &name=psi.name();

        HashPtrTable<interpolation<Type> > &table=
            this->interpolations<Type>();

        if(!table.found(name)) {
            if(debug) {
                Info << "Need to create a new interpolation for "
                    << name << endl;
            }
            word schemeName(this->getInterpolationScheme(name));

            table.set(
                name,
                interpolation<Type>::New(
                    schemeName,
                    psi
                ).ptr()
            );
        }

        return *table[name];
    }

    template<class T>
    tmp<Field<T> > PatchValueExpressionDriver::mapField(
        const word &inName,
        bool internalField
    ) {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::mapField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        if(debug) {
            Info << "Mapping " << name << " with result of type "
                << pTraits<T>::typeName << endl;
        }

        typedef GeometricField<T,fvPatchField,volMesh> localVolField;
        typedef GeometricField<T,fvsPatchField,surfaceMesh> localSurfaceField;

        if(!isA<mappedPatchBase>(patch_.patch())) {
            FatalErrorIn("tmp<Field<T> > PatchValueExpressionDriver::mapField")
                << "Current patch " << patch_.name()
                    << "is not a mapped patch"
                    << endl
                    << exit(FatalError);

        }

        const mappedPatchBase& mpp = dynamicCast<const mappedPatchBase>
            (
                patch_.patch()
            );

        const fvMesh& thisMesh = patch_.boundaryMesh().mesh();
        const fvMesh& nbrMesh = refCast<const fvMesh>(mpp.sampleMesh());
        const fvMesh& lookupMesh= mpp.sameRegion() ? thisMesh : nbrMesh;

        bool isSurfaceField=false;
        if(lookupMesh.foundObject<localSurfaceField>(name)) {
            if(debug) {
                Info << name << " is a " << localSurfaceField::typeName << endl;
            }
            isSurfaceField=true;
        } else if(!lookupMesh.foundObject<localVolField>(name)) {
            FatalErrorIn("tmp<Field<T> > PatchValueExpressionDriver::mapField")
                << "No field " << name << " of type "
                    << localSurfaceField::typeName << " or "
                    << localVolField::typeName << " on mesh "
                    << lookupMesh.name()
                    << endl
                    << exit(FatalError);
        } else {
            if(debug) {
                Info << name << " is a " << localVolField::typeName << endl;
            }
        }

        tmp<Field<T> > result(new Field<T>(this->size(),pTraits<T>::zero));
        Field<T> &newValues=result();

        if(
            isSurfaceField
            &&
            internalField
        ) {
            FatalErrorIn(
                "tmp<Field<T> > PatchValueExpressionDriver::mapField"
            )
                << "Field " << name << " is a surfaceField. "
                    << "Function 'mappedInternal'"
                    << " on patch " << patch_.name()
                    << " does not work for that."
                    << endl
                    << exit(FatalError);
        }

        switch (mpp.mode())
        {
            case mappedPatchBase::NEARESTCELL:
            {
                if(isSurfaceField) {
                    FatalErrorIn(
                        "tmp<Field<T> > PatchValueExpressionDriver::mapField"
                    )
                        << "Field " << name << " is a surfaceField. "
                            << "Map mode 'nearestCell'"
                            << " on patch " << patch_.name()
                            << " does not work for that."
                            << endl
                            << exit(FatalError);
                }

                if(internalField) {
                    FatalErrorIn(
                        "tmp<Field<T> > PatchValueExpressionDriver::mapField"
                    )
                        << "Function 'mappedInternal' does not work with "
                            << "Map mode 'nearestCell'"
                            << " on patch " << patch_.name()
                            << endl
                            << exit(FatalError);
                }

                const mapDistribute& distMap = mpp.map();
                const word interpolationScheme=this->getInterpolationScheme(
                    name
                );

                if (interpolationScheme != interpolationCell<T>::typeName)
                {
#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
                    notImplemented(
                        "Interpolation scheme "+interpolationScheme
                        +" in version 1.x of OpenFOAM"
                    );
#else
                    // Send back sample points to the processor that holds the cell
                    vectorField samples(mpp.samplePoints());
                    distMap.reverseDistribute
                        (
                            (
                                mpp.sameRegion()
                                ? thisMesh.nCells()
                                : nbrMesh.nCells()
                            ),
                            point::max,
                            samples
                        );

                    const interpolation<T>& interp =
                        this->getInterpolation(
                            lookupMesh.lookupObject<localVolField>(name)
                        );

                    newValues.setSize(samples.size(), pTraits<T>::max);
                    forAll(samples, cellI)
                    {
                        if (samples[cellI] != point::max)
                        {
                            newValues[cellI] = interp.interpolate
                                (
                                    samples[cellI],
                                    cellI
                                );
                        }
                    }
#endif
                }
                else
                {
                    newValues = lookupMesh.lookupObject<localVolField>(name);
                }

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
                mapDistribute::distribute
                            (
                                Pstream::defaultCommsType,
                                distMap.schedule(),
                                distMap.constructSize(),
                                distMap.subMap(),
                                distMap.constructMap(),
                                newValues
                            );
#else
                distMap.distribute(newValues)    ;
#endif

                break;
            }

            case mappedPatchBase::NEARESTPATCHFACE:
#ifdef FOAM_HAS_ABI_PATCHES
            case mappedPatchBase::NEARESTPATCHFACEAMI:
#endif
            {
                const label nbrPatchID =
                    nbrMesh.boundaryMesh().findPatchID(mpp.samplePatch());

                if (nbrPatchID < 0)
                {
                    FatalErrorIn
                        (
                            "tmp<Field<T> > PatchValueExpressionDriver::mapField"
                        )<< "Unable to find sample patch " << mpp.samplePatch()
                            << " in region " << mpp.sampleRegion()
                            << " for patch " << this->patch().name() << nl
                            << abort(FatalError);
                }


                if(isSurfaceField) {
                    const localSurfaceField& nbrField
                        = lookupMesh.lookupObject<localSurfaceField>(name);

                    newValues = nbrField.boundaryField()[nbrPatchID];
                } else {
                    const localVolField& nbrField
                        = lookupMesh.lookupObject<localVolField>(name);

                    if(internalField) {
                        newValues = nbrField.boundaryField()[nbrPatchID]
                            .patchInternalField();
                    } else {
                        newValues = nbrField.boundaryField()[nbrPatchID];
                    }
                }

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
                const mapDistribute& distMap = mpp.map();
                mapDistribute::distribute
                            (
                                Pstream::defaultCommsType,
                                distMap.schedule(),
                                distMap.constructSize(),
                                distMap.subMap(),
                                distMap.constructMap(),
                                newValues
                            );
#else
                mpp.distribute(newValues);
#endif

                break;
            }
            case mappedPatchBase::NEARESTFACE:
            {
                if(isSurfaceField) {
                    FatalErrorIn(
                        "tmp<Field<T> > PatchValueExpressionDriver::mapField"
                    )
                        << "Field " << name << " is a surfaceField. "
                            << "Map mode 'nearestFace'"
                            << " on patch " << patch_.name()
                            << " does not work for that."
                            << endl
                            << exit(FatalError);
                }

                const mapDistribute& distMap = mpp.map();

                Field<T> allValues(nbrMesh.nFaces(), pTraits<T>::zero);

                const localVolField& nbrField = lookupMesh.lookupObject<localVolField>(name);

                forAll(nbrField.boundaryField(), patchI)
                {
                    if(internalField) {
                        const fvPatchField<T>& pf =
                            nbrField.boundaryField()[patchI];
                        const Field<T> pif(pf.patchInternalField());
                        label faceStart = pf.patch().patch().start();

                        forAll(pf, faceI)
                        {
                            allValues[faceStart++] = pif[faceI];
                        }
                    } else {
                        const fvPatchField<T>& pf =
                            nbrField.boundaryField()[patchI];
                        label faceStart = pf.patch().patch().start();

                        forAll(pf, faceI)
                        {
                            allValues[faceStart++] = pf[faceI];
                        }
                    }
                }

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
                mapDistribute::distribute
                            (
                                Pstream::defaultCommsType,
                                distMap.schedule(),
                                distMap.constructSize(),
                                distMap.subMap(),
                                distMap.constructMap(),
                                allValues
                            );
#else
                distMap.distribute(allValues);
#endif
                newValues.transfer(allValues);

                break;
            }
            default:
                {
                    FatalErrorIn
                        (
                            "tmp<Field<T> > PatchValueExpressionDriver::mapField"
                        )<< "Unknown sampling mode: " << label(mpp.mode())
                            << nl << abort(FatalError);
                }
        }

        return result;
    }

    template<class T>
    tmp<Field<T> > PatchValueExpressionDriver::mapField(tmp<Field<T> > orig)
    {
        if(debug) {
            Info << "PatchValueExpressionDriver::mapField(tmp<Field<T> > orig)"
                << endl << "orig: " << orig() << endl;
        }

        if(!isA<mappedPatchBase>(patch_.patch())) {
            FatalErrorIn("tmp<Field<T> > PatchValueExpressionDriver::mapField")
                << "Current patch " << patch_.name()
                    << "is not a mapped patch"
                    << endl
                    << exit(FatalError);

        }

        const mappedPatchBase& mpp = dynamicCast<const mappedPatchBase>
            (
                patch_.patch()
            );


        if(
            mpp.mode()!=mappedPatchBase::NEARESTPATCHFACE
#ifdef FOAM_HAS_ABI_PATCHES
            &&
            mpp.mode()!=mappedPatchBase::NEARESTPATCHFACEAMI
#endif
        ) {
            FatalErrorIn("PatchValueExpressionDriver::mapField(tmp<Field<T> > orig)")
            << "This method only works for patch-to-patch mapping"
            << endl
            << exit(FatalError);

        }
        //        const fvMesh& thisMesh = patch_.boundaryMesh().mesh();
        const fvMesh& nbrMesh = refCast<const fvMesh>(mpp.sampleMesh());

        tmp<Field<T> > pResult(
            new Field<T>(orig)
        );

        Field<T> &result=pResult();

        const label nbrPatchID =
            nbrMesh.boundaryMesh().findPatchID(mpp.samplePatch());

        if (nbrPatchID < 0)
        {
            FatalErrorIn
                (
                    "tmp<Field<T> > PatchValueExpressionDriver::mapField"
                )<< "Unable to find sample patch " << mpp.samplePatch()
                    << " in region " << mpp.sampleRegion()
                    << " for patch " << this->patch().name() << nl
                    << abort(FatalError);
        }

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
        const mapDistribute& distMap = mpp.map();
        mapDistribute::distribute
        (
            Pstream::defaultCommsType,
            distMap.schedule(),
            distMap.constructSize(),
            distMap.subMap(),
            distMap.constructMap(),
            result
        );
#else
        mpp.distribute(result);
#endif
        if(debug) {
            Info << "Mapped to " << result << endl;
        }
        return pResult;
    }

    template<class T>
    tmp<Field<T> > PatchValueExpressionDriver::mapPointField(
        const word &inName
    ) {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "PatchValueExpressionDriver::mapPointField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<T,pointPatchField,pointMesh> localPointField;

        // dummy implemntation
        tmp<Field<T> > result(new Field<T>(this->pointSize(),pTraits<T>::zero));

        notImplemented("tmp<Field<T> > PatchValueExpressionDriver::mapPointField");

        return result;
    }

}


// ************************************************************************* //
