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
    2010-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "surfaceMesh.H"
#include "fvsPatchField.H"
#include "pointPatchField.H"
#include "primitivePatchInterpolation.H"

#include <cassert>

namespace Foam {

    inline const pointMesh &CommonValueExpressionDriver::pMesh()
    {
        if(!pMesh_.valid()) {
            pMesh_.set(
                new pointMesh(mesh())
            );
        }

        return pMesh_();
    }

    template<class Type>
    tmp<Field<Type> > CommonValueExpressionDriver::evaluate(
        const exprString &expr,
        bool qPoint
    )
    {
        parse(expr);

        return getResult<Type>(qPoint);
    }

    template<class Type>
    Type CommonValueExpressionDriver::evaluateUniform(
        const exprString &expr,
        bool qPoint
    )
    {
        parse(expr);

        ExpressionResult result=this->getUniform(1,true);
        Type val=result.getResult<Type>()()[0];

        return val;
    }

    template<class Type>
    tmp<Field<Type> > CommonValueExpressionDriver::makeField(Type val,bool isPoint)
    {
        label size=this->size();
        if(isPoint) {
            size=this->pointSize();
        }
        return tmp<Field<Type> >(
            new Field<Type>(size,val)
        );
    }

    template<class Type>
    void CommonValueExpressionDriver::setResult(Field<Type> *val,bool isPoint)
    {
        result_.setResult<Type>(val,isPoint);
    }

    template<class Type>
    tmp<Field<Type> > CommonValueExpressionDriver::getResult(bool qPoint)
    {
        if(qPoint!=result_.isPoint()) {
            FatalErrorIn("CommonValueExpressionDriver::getResult(bool qPoint)")
                << "A " << (qPoint ? "point" : "face" ) << "-field was wanted"
                    << " but we got a " << (!qPoint ? "point" : "face" )
                    << endl
                    << exit(FatalError);
        }

        return result_.getResult<Type>();
    }

    template<template<class> class BinOp,class Type>
    inline Type CommonValueExpressionDriver::getReduced(
        BinOp<Type> op,
        Type initial
    )
    {
        return result_.getReduced(op,initial);
    }

    template<class Type>
    bool CommonValueExpressionDriver::isVariable(
        const word &name,
        bool isPoint,
        label expectedSize
    ) const
    {
        if(debug) {
            Pout << "CommonValueExpressionDriver::isVariable "
                << " looking for " << name << " isPoint: " << isPoint
                << " of type " << pTraits<Type>::typeName
                << " Expected size: " << expectedSize;
        }

        if(hasVariable(name)) {
            if(debug) {
                Pout << " - name found";
            }
            const ExpressionResult &var=variable(name);
            if(
                var.valueType()==pTraits<Type>::typeName
                &&
                var.isPoint()==isPoint
            ) {
                bool sizeOK=true;
                if(expectedSize>=0) {
                    sizeOK=(var.size()==expectedSize);
                    reduce(sizeOK,andOp<bool>());
                }

                if(sizeOK) {
                    if(debug) {
                        Pout << " - OK" << endl;
                    }

                    return true;
                }
            }
            if(debug) {
                Pout << "(" << var.valueType() << " "
                    << var.isPoint() << ")";
            }
        }

        if(debug) {
            Pout << " - not OK -> trying global" << endl;
        }

        return isGlobal<Type>(name,isPoint,expectedSize);
    }

    template<class Type>
    bool CommonValueExpressionDriver::isGlobal(
        const word &name,
        bool isPoint,
        label expectedSize
    ) const
    {
        if(debug) {
            Pout << "CommonValueExpressionDriver::isGlobal "
                << " looking for " << name << " isPoint: " << isPoint << flush;
        }

        const ExpressionResult &aGlobal=lookupGlobal(name);

        if (debug) {
            Pout << " - found type " << aGlobal.valueType()
                << " isPoint " << isPoint;
        }

        if(
            aGlobal.valueType()==pTraits<Type>::typeName
            &&
            aGlobal.isPoint()==isPoint
        ) {
            bool sizeOK=true;
            if(expectedSize>=0) {
                sizeOK=(aGlobal.size()==expectedSize);
                reduce(sizeOK,andOp<bool>());
            }

            if(sizeOK) {
                if(debug) {
                    Pout << " - OK" << endl;
                }
                return true;
            } else {
                if(debug) {
                    Pout << " - Found but wrong size" << endl;
                }
                return false;
            }
        } else {
            if(debug) {
                Pout << " - not OK" << endl;
            }
            return false;
        }
    }

    template<class Type>
    bool CommonValueExpressionDriver::is(const word &name,bool isPoint) const
    {
        if(isVariable<Type>(name,isPoint)) {
            return true;
        } else {
            return isField<Type>(name,isPoint);
        }
    }

    template<class Type>
    bool CommonValueExpressionDriver::isField(
        const word &inName,
        bool isPoint
    ) const
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::isField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
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
                isThere<localVolField>(name)
            ) {
                return true;
            } else if(
                isThere<localSurfaceField>(name)
            ) {
                return true;
            } else{
                return false;
            }
        }
    }

    template<class T>
    inline bool CommonValueExpressionDriver::updateSet(
        autoPtr<T> &theSet,
        const word &inName,
        SetOrigin origin
    ) const
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::updateSet. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        label oldSize=theSet->size();

        bool updated=false;
        const polyMesh &mesh=dynamic_cast<const polyMesh &>(theSet->db());

        if(debug) {
            Pout << "UpdateSet: " << theSet->name()
                << " Id: " << name << " Origin: " << int(origin) << endl;
        }

        switch(origin) {
            case FILE:
                { // Scope to make sure header is not present in the other cases
                    IOobject header
                        (
                            name,
                            mesh.time().timeName(),
                            polyMesh::meshSubDir/"sets",
                            mesh,
                            IOobject::MUST_READ,
                            IOobject::NO_WRITE
                        );
                    if(
                        header.headerOk()
                        &&
                        header.headerClassName()==T::typeName
                    ) {
                        if(debug) {
                            Pout << "Rereading from " << header.filePath()
                                << endl;
                        }
                        theSet.reset(
                            new T(
                                header
                            )
                        );
                        updated=true;
                    }
                }
                break;
            case NEW:
                WarningIn("CommonValueExpressionDriver::updateSet")
                    << "State NEW shouldn't exist"
                        << endl;
            case MEMORY:
            case CACHE:
                {
                    word sName=name;
                    if(mesh.thisDb().foundObject<T>(name)) {
                        if(debug) {
                            Pout << "Found " << name
                                << " and rereading it" << endl;
                        }
                        theSet.reset(
                            new T(
                                mesh,
                                name,
                                mesh.thisDb().lookupObject<T>(name)
                            )
                        );
                    } else {
                        FatalErrorIn("CommonValueExpressionDriver::updateSet")
                            << name << " Not found" << endl
                                << "In db: " << mesh.thisDb().names() << endl
                                << exit(FatalError);
                    }

                    updated=true;
                }
                break;
            case INVALID:
                FatalErrorIn("CommonValueExpressionDriver::updateSet")
                    << T::typeName << " " << name << " is invalid"
                        << endl
                        << exit(FatalError);
            default:
                if(debug) {
                    Pout << "Origin " << int(origin) << " not implemented"
                        << endl;
                }
        }

        if(debug) {
            Pout << name << " old size " << oldSize << " new: "
                << theSet->size()
                << endl;
        }

        return updated;
    }

    template<class T>
    inline autoPtr<T> CommonValueExpressionDriver::getSet(
        const fvMesh &mesh,
        const word &inName,
        SetOrigin &origin
    ) const
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::getSet. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }
        // avoid the possibility of name-clashes
        const word regName=name+"RegisteredNameFor"+T::typeName;

        if(debug) {
            Pout << "Looking for " << T::typeName << " named " << name
                << " or  registered as " << regName << " with mesh ";
            Pout << "Caching: " << cacheSets()
                << " Found: " << mesh.foundObject<T>(name)
                << " Found registered: " << mesh.foundObject<T>(regName)
                << endl;
        }
        if(
            !cacheSets()
            ||
            (
                !mesh.thisDb().foundObject<T>(regName)
                &&
                !mesh.thisDb().foundObject<T>(name)
            )
        ) {
            if(debug) {
                Pout << "Constructing a new " << T::typeName << " "
                    << name << endl;
                if(debug>1) {
                    Pout << mesh.thisDb().names();
                }
            }
            origin=FILE;

            autoPtr<T> s(
                new T(
                    mesh,
                    name,
                    IOobject::MUST_READ
                )
            );

            if(cacheSets()) {
                if(debug) {
                    Pout << "Registering a copy of " << name
                        << " with mesh" << endl;
                }
                autoPtr<T> toCache(new T(mesh,regName,s()));
                toCache->store(toCache);
            }
            return s;
        } else {
            if(mesh.thisDb().foundObject<T>(name)) {
                if(debug) {
                    Pout << "Getting existing " << name << endl;
                }

                origin=MEMORY;

                return autoPtr<T>(
                    new T(
                        mesh,
                        name,
                        mesh.lookupObject<T>(name)
                    )
                );
            } else {
                if(debug) {
                    Pout << "Getting existing " << regName << endl;
                }

                origin=CACHE;

                return autoPtr<T>(
                    new T(
                        mesh,
                        name,
                        mesh.lookupObject<T>(regName)
                    )
                );
            }
        }
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(
        volScalarField &f
    ) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(
        volVectorField &f
    ) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(
        volTensorField &f
    ) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(
        volSymmTensorField &f
    ) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(
        volSphericalTensorField &f
    ) const
    {
        f.correctBoundaryConditions();
    }

    template<class T>
    inline void CommonValueExpressionDriver::correctField(T &f) const
    {
    }

    template<class T>
    autoPtr<T> CommonValueExpressionDriver::getOrReadField(
        const word &name,
        bool failIfNoneFound,
        bool getOldTime
    )
    {
        return this->getOrReadFieldInternal<T>(
            name,
            this->mesh(),
            failIfNoneFound,
            getOldTime
        );
    }

    template<class T>
    autoPtr<T> CommonValueExpressionDriver::getOrReadPointField(
        const word &name,
        bool failIfNoneFound,
        bool getOldTime
    )
    {
        return this->getOrReadFieldInternal<T>(
            name,
            this->pMesh(),
            failIfNoneFound,
            getOldTime
        );
    }

    template<class T,class Mesh>
    autoPtr<T> CommonValueExpressionDriver::getOrReadFieldInternal(
        const word &inName,
        const Mesh &actualMesh,
        bool failIfNoneFound,
        bool getOldTime
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::getOrReadFieldInternal. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        if(debug) {
            Pout << "CommonValueExpressionDriver::getOrReadField. Name: "
                << name << " Type: " << T::typeName << endl;
        }

        dimensionSet nullDim(0,0,0,0,0);

        autoPtr<T> f;

        typedef typename T::value_type Type;

        if(
            (
                hasVariable(name)
                &&
                variable(name).valueType()==pTraits<Type>::typeName
            )
            ||
            isGlobal<Type>(name,false)
        ) {
            if(debug) {
                Pout << "Getting " << name << " from variables" << endl;
            }

            word patchType("calculated");
            if(T::PatchFieldType::typeName=="fvPatchField") {
                patchType="zeroGradient";
            }
            if(debug) {
                Info << "Creating field " << name << " of type "
                    << T::typeName << " with patch type " << patchType
                    << "(" << T::PatchFieldType::typeName << ")" << endl;
            }
            f.set(
                new T(
                    IOobject
                    (
                        name,
                        this->mesh().time().timeName(),
                        this->mesh(),
                        IOobject::NO_READ,
                        IOobject::NO_WRITE,
                        false  // don't register
                    ),
                    actualMesh,
                    dimensioned<Type>(name,nullDim,pTraits<Type>::zero),
                    patchType
                )
            );
            if(debug) {
                Pout <<"New field: " << name << " ownedByRegistry"
                    << f->ownedByRegistry() << endl;
            }

            Field<Type> vals;
            if(
                hasVariable(name)
                &&
                variable(name).valueType()==pTraits<Type>::typeName
            ) {
                vals=variable(name).getResult<Type>(true);
            } else {
                vals=const_cast<ExpressionResult &>(
                    lookupGlobal(name)
                ).getResult<Type>(true);
            }

            if(debug) {
                Pout << "sizes: " << vals.size() << " " << f->size() << endl;
            }

            bool sameSize=vals.size()==f->size();
            reduce(sameSize,andOp<bool>());
            if(sameSize) {
                f->internalField()=vals;
            } else {
                Type avg=gAverage(vals);

                bool noWarn=false;

                if(!noWarn) {
                    Type minVal=gMin(vals);
                    Type maxVal=gMax(vals);
                    if(mag(minVal-maxVal)>SMALL) {
                        WarningIn("CommonValueExpressionDriver::getOrReadFieldInternal(const word &name)")
                            << "The minimum value " << minVal
                                << " and the maximum " << maxVal
                                << " differ. I will use the average " << avg
                                << endl;
                    }
                }

                f->internalField()=avg;
            }

            correctField(f());

            return f;
        }

        if(
            searchInMemory()
            &&
            this->mesh().foundObject<T>(name)
        ) {
            if(debug) {
                Pout << "Getting " << name << " from memory" << endl;
            }

            f.set(
                new T(
                    name+"_copyBySwak4Foam",    // make sure that the original object is not shadowed
                    this->mesh().lookupObject<T>(name)
                )
            );
            if(getOldTime) {
                if(debug) {
                    Info << "Getting oldTime of " << name << " has "
                        << this->mesh().lookupObject<T>(name).nOldTimes()
                        << endl;
                }
                if(
                    this->mesh().lookupObject<T>(name).nOldTimes()==0
                    &&
                    this->prevIterIsOldTime()
                ) {
                    if(debug) {
                        Info << "Using the previous iteration, because there is no oldTime" << endl;
                    }
                    f->oldTime()=this->mesh().lookupObject<T>(name).prevIter();
                }
            }
        } else if(
            searchOnDisc()
            &&
            getTypeOfField(name)==T::typeName
        ) {
            if(debug) {
                Pout << "Reading " << name << " from disc" << endl;
            }

            f.set(
                this->readAndRegister<T>(name,actualMesh).ptr()
            );
            // oldTime automatically read
        }

        if(debug) {
            Info << "autoPtr: valid()=" << f.valid()
                << " empty()=" << f.empty() << endl;
        }
        if(
            !f.valid()
            &&
            failIfNoneFound
        ) {
            FatalErrorIn("CommonValueExpressionDriver::getOrReadFieldInternal(const word &name)")
                << "Could not find the field " << name
                    << " (" << inName << ")"
                    << " in memory or on disc" << endl
                    << exit(FatalError);
        }

        if(f.valid()) {
            if(debug) {
                Info << "Valid " << name << " found. Removing dimensions"
                    << endl;
            }
            f->dimensions().reset(nullDim);
            if(f->nOldTimes()>0) {
                if(debug) {
                    Info << "Removing dimensions of oldTime of " << name
                        << " has " << f->nOldTimes()
                        << endl;
                }

#ifndef FOAM_OLDTIME_PROBLEM
                // go through ALL old times
                T *fp=f.operator->();
                while(fp->nOldTimes()>0) {
                    fp=&(fp->oldTime());  // 1.6-ext seems to have a problem here
                    fp->dimensions().reset(nullDim);
                }
#endif
            }
        }

        return f;
    }

    template<class Type>
    tmp<Field<Type> > CommonValueExpressionDriver::getVariable(
        const word &inName,
        const label expectedSize
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::getVariable. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        autoPtr<Field<Type> >vals;
        bool isSingleValue=false;

        if(
            hasVariable(name)
            &&
            variable(name).valueType()==pTraits<Type>::typeName
        ) {
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
            bool qSize=vals->size()==expectedSize;

            reduce(qSize,andOp<bool>());

            if(
                qSize
            ) {
                return tmp<Field<Type> >(
                    vals.ptr()
                );
            } else {
                if(!isSingleValue) {
                    WarningIn("CommonValueExpressionDriver::getVariable(const word &inName,const label expectedSize)")
                        << "Variable " << name
                            << " is not a single value but does not "
                            << "fit the size " << expectedSize
                            << ". Using average and "
                            << "hoping for the best"
                            << endl;
                }
                return tmp<Field<Type> >(
                    new Field<Type>(
                        expectedSize,gAverage(vals())
                    )
                );
            }
        } else {
            FatalErrorIn("CommonValueExpressionDriver::getVariable(const word &inName,const label expectedSize)")
                << "Variable " << inName << " (aliased to " << name
                    << ") not found."
                    << endl
                    << exit(FatalError);

            return tmp<Field<Type> >(
                new Field<Type>(
                    expectedSize,pTraits<Type>::zero
                )
            );
       }
    }

    template<class T,class Mesh>
    inline tmp<T> CommonValueExpressionDriver::readAndRegister(
        const word &inName,
        const Mesh &actualMesh
    )
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::readAndRegister. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

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
                actualMesh
            )
        );
        if(cacheReadFields()) {
            if(debug) {
                Pout << "Registering a copy of " << name
                    << " with mesh" << endl;
            }
            autoPtr<T> toCache(new T(f()));
            toCache->store(toCache);
        }
        return f;
    }

    template<class T>
    tmp<T> CommonValueExpressionDriver::interpolateForeignField(
        const word &inMeshName,
        const word &inFieldName,
        meshToMeshOrder theOrder
    ) const
    {
        word meshName(inMeshName);
        if(this->hasAlias(meshName)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::interpolateForeignField. MeshName: " << meshName
                    << " is an alias for " << this->getAlias(meshName) << endl;
            }
            meshName=this->getAlias(meshName);
        }
        word fieldName(inFieldName);
        if(this->hasAlias(fieldName)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::interpolateForeignField. fieldName: "
                    << fieldName
                    << " is an alias for " << this->getAlias(fieldName) << endl;
            }
            fieldName=this->getAlias(fieldName);
        }

        const fvMesh &theMesh=MeshesRepository::getRepository().getMesh(
            meshName
        );
        const meshToMesh &interpolation=MeshesRepository::getRepository().
            getMeshToMesh(
                meshName,
                this->mesh()
            );

        if(!theMesh.foundObject<T>(fieldName)) {
            if(debug) {
                Pout << "Not Found " << fieldName << " in memory I have" << endl;
            }
            autoPtr<T> f(
                new T(
                    IOobject
                    (
                        fieldName,
                        theMesh.time().timeName(),
                        theMesh,
                        IOobject::MUST_READ,
                        IOobject::NO_WRITE
                    ),
                    theMesh
                )
            );
            if(debug) {
                Pout << "Field " << fieldName << " in memory store I will"
                    << endl;
            }

            dimensionSet nullDim(0,0,0,0,0);
            f->dimensions().reset(nullDim);

            f->store(f);
        }
        if(debug) {
            Info << "Objects in mesh: " << theMesh.names() << endl;
        }
#ifdef FOAM_NEW_MESH2MESH
        return interpolation.mapSrcToTgt(
            theMesh.lookupObject<T>(fieldName),
            eqOp<typename T::value_type>()
        );
#else
        return interpolation.interpolate(
            theMesh.lookupObject<T>(fieldName),
            theOrder
#ifdef FOAM_MESHTOMESH_INTERPOLATE_REDUCE
            ,eqOp<typename T::value_type>()
#endif
        );
#endif
    }


    template<class T>
    bool CommonValueExpressionDriver::CommonValueExpressionDriver::isForeignField(
        const word &meshName,
        const word &inName
    ) const
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::isForeignField. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        if(!isForeignMesh(meshName)) {
            return false;
        }

        const fvMesh &theMesh=MeshesRepository::getRepository().getMesh(
            meshName
        );

        if(debug) {
            Pout << "CommonValueExpressionDriver::ForeignField. Name: " << name
                << " Type: " << T::typeName << endl;
        }

        if(theMesh.foundObject<T>(name)) {
            if(debug) {
                Pout << "Found " << name << " in memory" << endl;
            }
            return true;
        }

        if(
            getTypeOfFieldInternal(theMesh,name)==T::typeName
        ) {
            if(debug) {
                Pout << "Found " << name << " on disc" << endl;
            }
            return true;
        } else {
            if(debug) {
                Pout << name << " not found" << endl;
            }
            return false;
        }
    }

    template<class T>
    bool CommonValueExpressionDriver::CommonValueExpressionDriver::isThere(
        const word &inName
    ) const
    {
        word name(inName);
        if(this->hasAlias(name)) {
            if(debug) {
                Pout << "CommonValueExpressionDriver::isThere. Name: " << name
                    << " is an alias for " << this->getAlias(name) << endl;
            }
            name=this->getAlias(name);
        }

        if(debug) {
            Pout << "CommonValueExpressionDriver::isThere. Name: " << name
                << " Type: " << T::typeName << " searchInMemory: "
                << searchInMemory()
                << " searchOnDisc_: " << searchOnDisc() << endl;
        }
        if(searchInMemory()) {
            if(this->mesh().foundObject<T>(name)) {
                if(debug) {
                    Pout << "Found " << name << " in memory" << endl;
                }
                return true;
            } else if(debug) {
                Info << "No " << name << " of type " << T::typeName
                    << " found in memory";
                if(this->mesh().foundObject<IOobject>(name)) {
                    Info<< " but of type "
                        << this->mesh().lookupObject<IOobject>(name).headerClassName();
                    if(
                        this->mesh().lookupObject<IOobject>(name).headerClassName()
                        ==
                        T::typeName
                    ) {
                        // Info. This actually happens with clang 3.3
                        Info << " - strange. Type names are the same";
                        Info<< " Type fits: " << isA<T>(
                            this->mesh().lookupObject<IOobject>(name)
                        );
                        // this should fail
                        const T &f=this->mesh().lookupObject<T>(name);
                    }
                }
                Info << endl;
            }
        }

        if(
            searchOnDisc()
            &&
            getTypeOfField(name)==T::typeName
        ) {
            if(debug) {
                Pout << "Found " << name << " on disc" << endl;
            }
            return true;
        } else {
            if(debug) {
                Pout << name << " not found" << endl;
            }
            return false;
        }
    }

    inline bool CommonValueExpressionDriver::isLine(const word &name) const
    {
        return lines_.found(name);
    }

    inline bool CommonValueExpressionDriver::isLookup(const word &name) const
    {
        return lookup_.found(name);
    }

    template <class Op,class Type>
    tmp<Field<bool> > CommonValueExpressionDriver::doCompare(
        const Field<Type> &a,
        Op op,
        const Field<Type> &b
    )
    {
        assert(a.size()==b.size());

        tmp<Field<bool> > res(
            new Field<bool>(a.size(),false)
        );

        forAll(res(),i) {
            res()[i]=op(a[i],b[i]);
        }

        return res;
    }

    template <class Op>
    tmp<Field<bool> > CommonValueExpressionDriver::doLogicalOp(
        const Field<bool> &a,
        Op op,
        const Field<bool> &b
    )
    {
        assert(a.size()==b.size());

        tmp<Field<bool> > res(
            new Field<bool>(a.size(),false)
        );

        forAll(res(),i) {
            res()[i]=op(a[i],b[i]);
        }

        return res;
    }

    template<class Type>
    tmp<Field<Type> > CommonValueExpressionDriver::doConditional(
        const Field<bool> &d,
        const Field<Type> &yes,
        const Field<Type> &no
    )
    {
        assert(yes.size()==no.size() && d.size()==yes.size());

        tmp<Field<Type> > res(
            new Field<Type>(yes.size())
        );

        forAll(res(),i) {
            res()[i] = d[i] ? yes[i] : no[i];
        }

        return res;
    }

    tmp<Field<bool> > CommonValueExpressionDriver::doLogicalNot(
        const Field<bool> &a
    )
    {
        tmp<Field<bool> > res(
            new Field<bool>(a.size())
        );;

        forAll(res(),i) {
            res()[i] = a[i];
        }

        return res;
    }

    template<class Location>
    void CommonValueExpressionDriver::error (
        const Location& l,
        const std::string& m
    )
    {
        std::ostringstream buff;
        buff << l;
        std::string place=" ";
        std::string place2="-";
        for(unsigned int i=0;i<l.begin.column;i++) {
            place+=" ";
            place2+="-";
        }
        bool firstArrow=true;
        for(unsigned int i=l.begin.column;i<l.end.column;i++) {
            place+="^";
            if(firstArrow) {
                place2+="|";
                firstArrow=false;
            } else {
                place2+=" ";
            }
        }
        for(unsigned int i=l.end.column;i<content_.size();i++) {
            //            place+=" ";
        }

        FatalErrorIn("parsingValue")
            //        << Foam::args.executable()
            << " Parser Error for driver " << type() << " at "
                << buff.str() << " :"  << m << Foam::endl
                << content_ << Foam::endl
                << place.c_str() << Foam::endl
                << place2.c_str() << Foam::endl
                << getContextString().c_str()
                << Foam::exit(Foam::FatalError);
        //    Foam::Pout << buff.str() << ": " << m << Foam::endl;
    }

    template<class Type>
    Type CommonValueExpressionDriver::calcWeightedAverage(
        const Field<Type> &result
    ) const
    {
        const scalarField weights(
            this->weights(
                result.size(),
                this->result().isPoint()
            )
        );
        const scalar wSum=gSum(weights);
        const Type tSum=gSum(weights*result);
        return tSum/wSum;
    }

    template<class T>
    void CommonValueExpressionDriver::addUniformVariable(
        const word &name,
        const T &val
    ) {
        ExpressionResult e;
        e.setSingleValue(val);
        variables_.set(name,e);
    }

    bool CommonValueExpressionDriver::toBool(const scalar f) {
        return f>=0.5;
    }
}


// ************************************************************************* //
