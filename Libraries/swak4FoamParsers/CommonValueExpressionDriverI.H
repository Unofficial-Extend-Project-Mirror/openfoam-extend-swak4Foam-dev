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
                    << abort(FatalError);
        }

        return result_.getResult<Type>();
    }

    template<class Type>
    bool CommonValueExpressionDriver::isVariable(const string &name,bool isPoint) const
    {
        if(variables_.found(name)) {
            if(
                variables_[name].type()==pTraits<Type>::typeName 
                && 
                variables_[name].isPoint()==isPoint
            ) {
                return true;
            } 
        }

        return false;
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
                Info << "Registering a copy of " << name << " with mesh" << endl;
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
            Info << "CommonValueExpressionDriver::isThere. Name: " << name 
                << " Type: " << T::typeName << " searchInMemory: " << searchInMemory()
                << " searchOnDisc_: " << searchOnDisc() << endl;
        }
        if(searchInMemory()) {
            if(this->mesh().foundObject<T>(name)) {
                if(debug) {
                    Info << "Found " << name << " in memory" << endl;
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
                Info << "Found " << name << " on disc" << endl;
            }
            return true;
        } else {
            if(debug) {
                Info << name << " not found" << endl;
            }
            return false;
        }
    }

    inline bool CommonValueExpressionDriver::isLine(const string &name) const 
    {
        return lines_.found(name);
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
                    << abort(FatalError);

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
        
        Foam::FatalErrorIn("parsingValue")
            //        << Foam::args.executable()
            << " Parser Error at " << buff.str() << " :"  << m << Foam::endl
                << content_ << Foam::endl << place
                << Foam::exit(Foam::FatalError);
        //    Foam::Info << buff.str() << ": " << m << Foam::endl;
    }

}


// ************************************************************************* //
