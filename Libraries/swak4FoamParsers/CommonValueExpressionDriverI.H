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
 
    template<class Type>
    tmp<Field<Type> > CommonValueExpressionDriver::evaluate(const string &expr,bool qPoint)
    {
        parse(expr);

        return getResult<Type>(qPoint);
    }

    template<class Type>
    Type CommonValueExpressionDriver::evaluateUniform(const string &expr,bool qPoint)
    {
        parse(expr);

        ExpressionResult result=this->getUniform(1,true);
        Type val=result.getResult<Type>()()[0];

        return val;
    }

    template<class Type>
    Field<Type> *CommonValueExpressionDriver::makeField(Type val,bool isPoint)
    {
        label size=this->size();
        if(isPoint) {
            size=this->pointSize();
        }
        return new Field<Type>(size,val);
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
    inline Type CommonValueExpressionDriver::getReduced(BinOp<Type> op,Type initial)
    {
        return result_.getReduced(op,initial);
    }

    template<class Type>
    bool CommonValueExpressionDriver::isVariable(const string &name,bool isPoint) const
    {
        if(debug) {
            Pout << "CommonValueExpressionDriver::isVariable " 
                << " looking for " << name << " isPoint: " << isPoint;
        }

        if(variables_.found(name)) {
            if(debug) {
                Pout << " - name found";
            }
            if(
                variables_[name].type()==pTraits<Type>::typeName 
                && 
                variables_[name].isPoint()==isPoint
            ) {
                if(debug) {
                    Pout << " - OK" << endl;
                }

                return true;
            } 
        }

        if(debug) {
            Pout << " - not OK -> trying global" << endl;
        }

        return isGlobal<Type>(name,isPoint);
    }

    template<class Type>
    bool CommonValueExpressionDriver::isGlobal(
        const word &name,
        bool isPoint
    ) const
    {
        if(debug) {
            Pout << "CommonValueExpressionDriver::isGlobal "
                << " looking for " << name << " isPoint: " << isPoint << flush;
        }

        const ExpressionResult &aGlobal=lookupGlobal(name);

        if (debug) {
            Pout << " - found type " << aGlobal.type()  
                << " isPoint " << isPoint;
        }

        if(
            aGlobal.type()==pTraits<Type>::typeName 
            && 
            aGlobal.isPoint()==isPoint
        ) {
            if(debug) {
                Pout << " - OK" << endl;
            }
            return true;
        } else {
            if(debug) {
                Pout << " - not OK" << endl;
            }
            return false;
        }
    }

    template<class Type>
    bool CommonValueExpressionDriver::is(const string &name,bool isPoint) const
    {
        if(isVariable<Type>(name,isPoint)) {
            return true;
        } else {
            return isField<Type>(name,isPoint);
        }
    }

    template<class Type>
    bool CommonValueExpressionDriver::isField(const string &name,bool isPoint) const
    {
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
        const word &name,
        SetOrigin origin)
    {
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
                            Pout << "Rereading from " << header.filePath() << endl;
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
                            Pout << "Found " << name << " and rereading it" << endl;
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
                    Pout << "Origin " << int(origin) << " not implemented" << endl;
                }
        }

        if(debug) {
            Pout << name << " old size " << oldSize << " new: " << theSet->size()
                << endl;
        }

        return updated;
    }

    template<class T>
    inline autoPtr<T> CommonValueExpressionDriver::getSet(const fvMesh &mesh,const word &name,SetOrigin &origin)
    {
        // avoid the possibility of name-clashes
        const word regName=name+"RegisteredNameFor"+T::typeName;

        if(debug) {
            Pout << "Looking for " << T::typeName << " named " << name 
                << " or  registered as " << regName << " with mesh ";
            Pout << "Caching: " << cacheSets() 
                << " Found: " << mesh.foundObject<T>(name)
                << " Found registered: " << mesh.foundObject<T>(regName) << endl;
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
                Pout << "Constructing a new " << T::typeName << " " << name << endl;
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
                    Pout << "Registering a copy of " << name << " with mesh" << endl;
                }
                T* toCache=new T(mesh,regName,s());
                toCache->store();
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
    inline void CommonValueExpressionDriver::correctField(volScalarField &f) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(volVectorField &f) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(volTensorField &f) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(volSymmTensorField &f) const
    {
        f.correctBoundaryConditions();
    }

    template<>
    inline void CommonValueExpressionDriver::correctField(volSphericalTensorField &f) const
    {
        f.correctBoundaryConditions();
    }

    template<class T>
    inline void CommonValueExpressionDriver::correctField(T &f) const
    {
    }

    template<class T>
    autoPtr<T> CommonValueExpressionDriver::getOrReadField(
        const string &name,
        bool failIfNoneFound
    )
    {
        if(debug) {
            Pout << "CommonValueExpressionDriver::getOrReadField. Name: " << name 
                << " Type: " << T::typeName << endl;
        }
        
        dimensionSet nullDim(0,0,0,0,0);
        
        autoPtr<T> f;
        
        typedef typename T::value_type Type;
            
        if(
            variables_.found(name)
            ||
            isGlobal<Type>(name,false)
        ) {
            if(debug) {
                Pout << "Getting " << name << " from variables" << endl;
            }
            
            f.set(
                new T(
                    IOobject
                    (
                        name,
                        this->mesh().time().timeName(),
                        this->mesh(),
                        IOobject::NO_READ,
                        IOobject::NO_WRITE
                    ),
                    this->mesh(),
                    dimensioned<Type>(name,nullDim,pTraits<Type>::zero),
                    "zeroGradient"
                )
            );

            Field<Type> vals;
            if(variables_.found(name)) {
                vals=variables_[name].getResult<Type>(true);
            } else {
                vals=const_cast<ExpressionResult &>(
                    lookupGlobal(name)
                ).getResult<Type>(true);
            }

            if(debug) {
                Pout << "sizes: " << vals.size() << " " << f->size() << endl;
            }
            if(vals.size()==f->size()) {
                f->internalField()=vals;
            } else {
                Type avg=gAverage(vals);

                bool noWarn=false;

                if(!noWarn) {
                    Type minVal=gMin(vals);
                    Type maxVal=gMax(vals);
                    if(mag(minVal-maxVal)>SMALL) {
                        WarningIn("CommonValueExpressionDriver::getOrReadField(const string &name)")
                            << "The minimum value " << minVal << " and the maximum " 
                                << maxVal << " differ. I will use the average " << avg
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
                    this->mesh().lookupObject<T>(name)
                )
            );
        } else if(
            searchOnDisc()
            &&
            getTypeOfField(name)==T::typeName
        ) {
            if(debug) {
                Pout << "Reading " << name << " from disc" << endl;
            }
        
            f.set(
                this->readAndRegister<T>(name).ptr()
            );
        }

        if(
            !f.valid()
            && 
            failIfNoneFound
        ) {
            FatalErrorIn("CommonValueExpressionDriver::getOrReadField(const string &name)")
                << "Could not find the field " << name 
                    << " in memory or on disc" << endl
                    << exit(FatalError);
        }
    
        if(f.valid()) {
            f->dimensions().reset(nullDim);
        }

        return f;
    }


    template<class T>
    inline tmp<T> CommonValueExpressionDriver::readAndRegister(const word &name)
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
                this->mesh()
            )
        );
        if(cacheReadFields()) {
            if(debug) {
                Pout << "Registering a copy of " << name << " with mesh" << endl;
            }
            T* toCache=new T(f());
            toCache->store();
        }
        return f;
    }

    template<class T>
    bool CommonValueExpressionDriver::CommonValueExpressionDriver::isThere(const string &name) const
    {
        if(debug) {
            Pout << "CommonValueExpressionDriver::isThere. Name: " << name 
                << " Type: " << T::typeName << " searchInMemory: " << searchInMemory()
                << " searchOnDisc_: " << searchOnDisc() << endl;
        }
        if(searchInMemory()) {
            if(this->mesh().foundObject<T>(name)) {
                if(debug) {
                    Pout << "Found " << name << " in memory" << endl;
                }
                return true;
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

    inline bool CommonValueExpressionDriver::isLine(const string &name) const 
    {
        return lines_.found(name);
    }

    inline bool CommonValueExpressionDriver::isLookup(const string &name) const 
    {
        return lookup_.found(name);
    }

    /*    template<class Type>
    Field<Type> *CommonValueExpressionDriver::getField(const string &name)
    {
        typedef GeometricField<Type,fvPatchField,volMesh> localVolField;
        typedef GeometricField<Type,fvsPatchField,surfaceMesh> localSurfaceField;
        typedef GeometricField<Type,pointPatchField,pointMesh> localPointField;

        if(variables_.found(name)) {
            return variables_[name].getResult<Type>(true).ptr();
        }

        if(
            this->mesh().
            objectRegistry::foundObject<localVolField>(name)
        ) {
            const typename localVolField::PatchFieldType &pField=
                patch_.lookupPatchField<localVolField,Type>(name);
            return new Field<Type>(pField);
        } else if(
            this->mesh().
            objectRegistry::foundObject<localSurfaceField>(name)
        ) {
            const typename localSurfaceField::PatchFieldType &pField=
                patch_.lookupPatchField<localSurfaceField,Type>(name);
            return new Field<Type>(pField);
        } else if(
            this->mesh().
            objectRegistry::foundObject<localPointField>(name)
        ) {
            const typename localPointField::PatchFieldType &pField=
                patch_.lookupPatchField<localPointField,Type>(name);
            return new Field<Type>(pField.patchInternalField());
        } else{
            FatalErrorIn("Field<Type> *CommonValueExpressionDriver::getField(const string &name)")
                << "No field of type " << pTraits<scalar>::typeName
                    << " with the name " << name << " found\n"
                    << endl
                    << exit(FatalError);

            return NULL;
        }
    }
        */

    template <class Op,class Type>
    Field<bool> *CommonValueExpressionDriver::doCompare(Field<Type> *a,Op op,Field<Type> *b)
    {
        assert(a->size()==b->size());

        Field<bool> *res=new Field<bool>(a->size(),false);

        forAll(*res,i) {
            (*res)[i]=op((*a)[i],(*b)[i]);
        }
        
        return res;
    }

    template <class Op>
    Field<bool> *CommonValueExpressionDriver::doLogicalOp(Field<bool> *a,Op op,Field<bool> *b)
    {
        assert(a->size()==b->size());

        Field<bool> *res=new Field<bool>(a->size(),false);

        forAll(*res,i) {
            (*res)[i]=op((*a)[i],(*b)[i]);
        }
        
        return res;
    }

    template<class Type>
    Field<Type> *CommonValueExpressionDriver::doConditional(Field<bool> *d,Field<Type> *yes,Field<Type> *no)
    {
        assert(yes->size()==no->size() && d->size()==yes->size());

        Field<Type> *res=new Field<Type>(yes->size());

        forAll(*res,i) {
            (*res)[i] = (*d)[i] ? (*yes)[i] : (*no)[i];
        }

        return res;
    }

    Field<bool> *CommonValueExpressionDriver::doLogicalNot(Field<bool> *a)
    {
        Field<bool> *res=new Field<bool>(a->size());

        forAll(*res,i) {
            (*res)[i] = (*a)[i];
        }

        return res;
    }
    
    template<class Location>
    void CommonValueExpressionDriver::error (const Location& l, const std::string& m)
    {
        std::ostringstream buff;
        buff << l;
        std::string place="";
        for(unsigned int i=0;i<l.begin.column;i++) {
            place+=" ";
        }
        for(unsigned int i=l.begin.column;i<l.end.column;i++) {
            place+="^";
        }
        for(unsigned int i=l.end.column;i<content_.size();i++) {
            place+=" ";
        }
        
        FatalErrorIn("parsingValue")
            //        << Foam::args.executable()
            << " Parser Error at " << buff.str() << " :"  << m << Foam::endl
                << content_ << Foam::endl << place
                << Foam::exit(Foam::FatalError);
        //    Foam::Pout << buff.str() << ": " << m << Foam::endl;
    }

    inline const HashTable<ExpressionResult,word> &CommonValueExpressionDriver::variables() const
    {
        return variables_;
    }
}


// ************************************************************************* //
