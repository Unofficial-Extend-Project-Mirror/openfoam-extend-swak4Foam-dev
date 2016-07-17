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
    \\  /    A nd           | Copyright  held by original author
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
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "manipulateTopoSet.H"

#include "FieldValueExpressionDriver.H"

#include "cellSet.H"
#include "faceSet.H"
#include "pointSet.H"

namespace Foam {
    defineTemplateDebugSwitch(manipulateTopoSet<cellSet>,0);
    template<>
    const word manipulateTopoSet<cellSet>::typeName="manipulateTopoSet<cellSet>";
    defineTemplateDebugSwitch(manipulateTopoSet<faceSet>,0);
    template<>
    const word manipulateTopoSet<faceSet>::typeName="manipulateTopoSet<faceSet>";
    defineTemplateDebugSwitch(manipulateTopoSet<pointSet>,0);
    template<>
    const word manipulateTopoSet<pointSet>::typeName="manipulateTopoSet<pointSet>";
}

template<class TopoSetType>
Foam::manipulateTopoSet<TopoSetType>::manipulateTopoSet
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    active_(true),
    writeManipulated_(false),
    createMissing_(false),
    obr_(obr),
    dict_(dict)
{
    if (!isA<fvMesh>(obr_))
    {
        active_=false;
        WarningIn("manipulateTopoSet<"+TopoSetType::typeName+">::manipulateTopoSet")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    write();
}

template<class TopoSetType>
Foam::manipulateTopoSet<TopoSetType>::~manipulateTopoSet()
{}

template<class TopoSetType>
void Foam::manipulateTopoSet<TopoSetType>::timeSet()
{
    // Do nothing
}

template<class TopoSetType>
void Foam::manipulateTopoSet<TopoSetType>::read(const dictionary& dict)
{
    if(active_) {
        name_=word(dict.lookup(TopoSetType::typeName+"Name"));
        maskExpression_=exprString(
            dict.lookup("mask"),
            dict
        );
        writeManipulated_=dict.lookupOrDefault<bool>("writeManipulated",true);
        createMissing_=dict.lookupOrDefault<bool>("createMissing",false);

        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        driver_.set(
            new FieldValueExpressionDriver(
                mesh.time().timeName(),
                mesh.time(),
                mesh,
                false, // no caching. No need
                true,  // search fields in memory
                false  // don't look up files in memory
            )
        );

        driver_->readVariablesAndTables(dict_);

        // this might not work when rereading ... but what is consistent in that case?
        driver_->createWriterAndRead(name_+"_"+type());
    }
}

template<class TopoSetType>
void Foam::manipulateTopoSet<TopoSetType>::write()
{
    if(active_) {
        FieldValueExpressionDriver &driver=driver_();

        driver.clearVariables();

        driver.parse(maskExpression_);

        if(!driver.isLogical()) {
            FatalErrorIn("manipulateTopoSet<"+TopoSetType::typeName+">::execute()")
                << maskExpression_ << " does not evaluate to a logical expression"
                    << endl
                    << exit(FatalError);
        }

        calcMask();

        if(
            !obr_.foundObject<TopoSetType>(name_)
        ) {
            if(createMissing_) {
                Info << TopoSetType::typeName << " with name " << name_
                    << " not found. Creating" << endl;
                autoPtr<TopoSetType> set(
                    new TopoSetType(
                        dynamic_cast<const polyMesh&>(obr_),
                        name_,
                        128
                    )
                );
                set->store(set);
            } else {
                WarningIn("manipulateTopoSet<"+TopoSetType::typeName+">::execute()")
                    << "No " << TopoSetType::typeName << " named " << name_ << nl
                        << "set 'createMissing' to true in " << dict_.name()
                        << "to avoid crash"
                        << endl;
            }
        }

        TopoSetType &set=const_cast<TopoSetType&>(
                obr_.lookupObject<TopoSetType>(name_)
        );

        labelHashSet oldSet(set);
        set.clear();

        label cnt=0;
        label removed=0;
        label added=0;

        forAll(mask_,i) {
            if(mask_[i]) {
                cnt++;
                set.insert(i);
                if(!oldSet.found(i)) {
                    added++;
                }
            } else {
                if(oldSet.found(i)) {
                    removed++;
                }
            }
        }

        reduce(cnt,plusOp<label>());
        reduce(added,plusOp<label>());
        reduce(removed,plusOp<label>());
        label maskSize=mask_.size();
        reduce(maskSize,plusOp<label>());
        label oldSetSize=oldSet.size();
        reduce(oldSetSize,plusOp<label>());

        Info << "Manipulated " << TopoSetType::typeName << " " << name_
            << " with the expression " << maskExpression_ << ". Size: "
            << cnt << " of " << maskSize << " (old size: " << oldSetSize << ")"
            << " Added: " << added << " Removed: " << removed << endl;

        if(
            obr_.time().outputTime()
        ) {
            if(this->writeManipulated_) {
                Info << "Rewriting manipulated " << TopoSetType::typeName
                    << " " << set.name() << endl;

                if(Pstream::parRun()) {
                    set.instance()=
                        (word("processor") + Foam::name(Pstream::myProcNo()))
                        /
                        obr_.time().timeName();
                } else {
                    set.instance()=obr_.time().timeName();
                }
                set.write();
            } else {
                Info << "Manipulated field " << set.name()
                    << " not rewritten. Set 'writeManipulated'" << endl;
            }
        }
    }

    driver_->tryWrite();
}


template<class TopoSetType>
void Foam::manipulateTopoSet<TopoSetType>::end()
{
    execute();
}

template<class TopoSetType>
void Foam::manipulateTopoSet<TopoSetType>::execute()
{
}

template<class TopoSetType>
void Foam::manipulateTopoSet<TopoSetType>::toMask(const scalarField &mask)
{
    if(mask_.size()!=mask.size()) {
        mask_.resize(mask.size());
    }
    forAll(mask,i) {
        mask_[i]=driver_->toBool(mask[i]);
    }
}

namespace Foam {
    template<>
    void manipulateTopoSet<cellSet>::calcMask()
    {
        toMask(driver_->getResult<volScalarField>().internalField());
    }
    template<>
    void manipulateTopoSet<faceSet>::calcMask()
    {
        toMask(driver_->getResult<surfaceScalarField>().internalField());
    }
    template<>
    void manipulateTopoSet<pointSet>::calcMask()
    {
        toMask(driver_->getResult<pointScalarField>().internalField());
    }
    template class manipulateTopoSet<cellSet>;
    template class manipulateTopoSet<faceSet>;
    template class manipulateTopoSet<pointSet>;
}
// ************************************************************************* //
