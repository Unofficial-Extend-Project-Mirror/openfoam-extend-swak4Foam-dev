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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "manipulatePatchField.H"

#include "PatchValueExpressionDriver.H"

namespace Foam {
    defineTypeNameAndDebug(manipulatePatchField,0);
}

Foam::manipulatePatchField::manipulatePatchField
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
)
:
    active_(true),
    writeManipulated_(false),
    obr_(obr),
    dict_(dict)
{
    if (!isA<fvMesh>(obr_))
    {
        active_=false;
        WarningIn("manipulatePatchField::manipulatePatchField")
            << "Not a fvMesh. Nothing I can do"
                << endl;
    }
    read(dict);
    execute();
}

Foam::manipulatePatchField::~manipulatePatchField()
{}

template<class TField>
void Foam::manipulatePatchField::manipulate(
    const TField &field,
    const Field<bool> &mask
)
{
    const typename TField::PatchFieldType &pField=field.boundaryField()[
        field.mesh().boundaryMesh().findPatchID(this->patchName_)
    ];
    Field<typename TField::value_type> value(pField);

    driver_->parse(this->expression_);
    if(
        driver_->getResultType()
        !=
        pTraits<typename TField::value_type>::typeName
    ) {
        FatalErrorIn("void Foam::manipulatePatchField::manipulate")
            << "Expression " << expression_ << " evaluates to "
                << driver_->getResultType() << " but a "
                << pTraits<typename TField::value_type>::typeName
                << " is needed"
                << endl
                << exit(FatalError);

    }

    Field<typename TField::value_type> newValue(
        driver_->getResult<typename TField::value_type>()
    );
    if(
        value.size()!=mask.size()
        ||
        value.size()!=newValue.size()
    ) {
        FatalErrorIn("void Foam::manipulatePatchField::manipulate")
            << "Sizes of patch field " << patchName_ << " " << value.size()
                << " the new value " << newValue.size() << " and the mask "
                << mask.size()  << " don't match"
                << endl
                << exit(FatalError);

    }
    label cnt=0;
    forAll(value,cellI) {
        if(mask[cellI]) {
            cnt++;
            value[cellI]=newValue[cellI];
        }
    }

    reduce(cnt,plusOp<label>());
    const_cast<typename TField::PatchFieldType &>(pField)==value;

    Info << "Manipulated field " << name_ << " on patch " << patchName_ << " on " << cnt
        << " faces with the expression " << expression_ << endl;

    if(
        obr_.time().outputTime()
        &&
        field.writeOpt()==IOobject::AUTO_WRITE
    ) {
        if(this->writeManipulated_) {
            Info << "Rewriting manipulated field " << field.name() << endl;

            field.write();
        } else {
            Info << "Manipulated field " << field.name()
                << " not rewritten. Set 'writeManipulated'" << endl;
        }
    }
}

template<class TField>
void Foam::manipulatePatchField::manipulatePoint(
    const TField &field,
    const Field<bool> &mask
)
{
    const typename TField::PatchFieldType &pField=field.boundaryField()[
        //        dynamicCast<const fvMesh &>(
        dynamic_cast<const fvMesh &>(
            this->obr_
        ).boundaryMesh().findPatchID(this->patchName_)
    ];
    Field<typename TField::value_type> value(pField.patchInternalField());

    driver_->parse(this->expression_);
    if(
        driver_->getResultType()
        !=
        pTraits<typename TField::value_type>::typeName
    ) {
        FatalErrorIn("void Foam::manipulatePatchField::manipulate")
            << "Expression " << expression_ << " evaluates to "
                << driver_->getResultType() << " but a "
                << pTraits<typename TField::value_type>::typeName
                << " is needed"
                << endl
                << exit(FatalError);

    }

    Field<typename TField::value_type> newValue(
        driver_->getResult<typename TField::value_type>()
    );

    if(
        value.size()!=mask.size()
        ||
        value.size()!=newValue.size()
    ) {
        FatalErrorIn("void Foam::manipulatePatchField::manipulate")
            << "Sizes of patch field " << patchName_ << " " << value.size()
                << " the new value " << newValue.size() << " and the mask "
                << mask.size()  << " don't match"
                << endl
                << exit(FatalError);

    }
    label cnt=0;
    forAll(value,cellI) {
        if(mask[cellI]) {
            cnt++;
            value[cellI]=newValue[cellI];
        }
    }

    reduce(cnt,plusOp<label>());
    const_cast<typename TField::PatchFieldType &>(pField)==value;

    Info << "Manipulated field " << name_ << " on patch " << patchName_ << " on " << cnt
        << " points with the expression " << expression_ << endl;

    if(
        obr_.time().outputTime()
        &&
        field.writeOpt()==IOobject::AUTO_WRITE
    ) {
        if(this->writeManipulated_) {
            Info << "Rewriting manipulated field " << field.name() << endl;

            field.write();
        } else {
            Info << "Manipulated field " << field.name()
                << " not rewritten. Set 'writeManipulated'" << endl;
        }
    }
}

void Foam::manipulatePatchField::read(const dictionary& dict)
{
    if(active_) {
        name_=word(dict.lookup("fieldName"));
        patchName_=word(dict.lookup("patchName"));
        expression_=string(dict.lookup("expression"));
        maskExpression_=string(dict.lookup("mask"));
        writeManipulated_=dict.lookupOrDefault<bool>("writeManipulated",false);

        const fvMesh& mesh = refCast<const fvMesh>(obr_);
        const label patchID=mesh.boundaryMesh().findPatchID(patchName_);
        if(patchID<0) {
            FatalErrorIn("Foam::manipulatePatchField::read(const dictionary& dict)")
                << "Patch " << patchName_ << " not exisiting in "
                    << mesh.boundaryMesh().names()
                    << endl
                    << exit(FatalError);
        }

        const fvPatch &thePatch=mesh.boundary()[patchID];

        driver_.set(
            new PatchValueExpressionDriver(
                dict,
                thePatch
            )
        );

        //        driver_->readVariablesAndTables(dict_);

        // this might not work when rereading ... but what is consistent in that case?
        driver_->createWriterAndRead(name_+"_"+type());
    }
}

void Foam::manipulatePatchField::execute()
{
    if(active_) {
        PatchValueExpressionDriver &driver=driver_();

        driver.clearVariables();

        driver.parse(maskExpression_);

        if(
            driver.getResultType()
            !=
            pTraits<bool>::typeName
        ) {
            FatalErrorIn("manipulatePatchField::execute()")
                << maskExpression_ << " does not evaluate to a logical expression"
                    << " but a " << driver.getResultType()
                    << endl
                    << exit(FatalError);
        }

        Field<bool> conditionField(driver.getResult<bool>());

        if(obr_.foundObject<volScalarField>(name_)) {
            manipulate(
                obr_.lookupObject<volScalarField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<surfaceScalarField>(name_)) {
            manipulate(
                obr_.lookupObject<surfaceScalarField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<pointScalarField>(name_)) {
            manipulatePoint(
                obr_.lookupObject<pointScalarField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<volVectorField>(name_)) {
            manipulate(
                obr_.lookupObject<volVectorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<surfaceVectorField>(name_)) {
            manipulate(
                obr_.lookupObject<surfaceVectorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<pointVectorField>(name_)) {
            manipulatePoint(
                obr_.lookupObject<pointVectorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<volTensorField>(name_)) {
            manipulate(
                obr_.lookupObject<volTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<surfaceTensorField>(name_)) {
            manipulate(
                obr_.lookupObject<surfaceTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<pointTensorField>(name_)) {
            manipulatePoint(
                obr_.lookupObject<pointTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<volSymmTensorField>(name_)) {
            manipulate(
                obr_.lookupObject<volSymmTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<surfaceSymmTensorField>(name_)) {
            manipulate(
                obr_.lookupObject<surfaceSymmTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<pointSymmTensorField>(name_)) {
            manipulatePoint(
                obr_.lookupObject<pointSymmTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<volSphericalTensorField>(name_)) {
            manipulate(
                obr_.lookupObject<volSphericalTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<surfaceSphericalTensorField>(name_)) {
            manipulate(
                obr_.lookupObject<surfaceSphericalTensorField>(name_),
                conditionField
            );
        } else if(obr_.foundObject<pointSphericalTensorField>(name_)) {
            manipulatePoint(
                obr_.lookupObject<pointSphericalTensorField>(name_),
                conditionField
            );
        } else {
            FatalErrorIn("Foam::manipulatePatchField::execute()")
                << "No supported field with the name " << name_ << " found"
                    << endl
                    << exit(FatalError);
        }
    }

    driver_->tryWrite();
}


void Foam::manipulatePatchField::end()
{
    execute();
}

void Foam::manipulatePatchField::write()
{
}

// ************************************************************************* //
