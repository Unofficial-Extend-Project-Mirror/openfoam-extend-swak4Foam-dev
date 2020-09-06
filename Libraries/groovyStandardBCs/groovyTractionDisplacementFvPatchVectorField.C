/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2004-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
                            | Copyright (C) 2011-2016 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "groovyTractionDisplacementFvPatchVectorField.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"

#include "groovyBCCommon.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

groovyTractionDisplacementFvPatchVectorField::
groovyTractionDisplacementFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedGradientFvPatchVectorField(p, iF),
    traction_(p.size(), pTraits<vector>::zero),
    pressure_(p.size(), pTraits<scalar>::zero),
    tractionValueExpression_(groovyBCCommon<vector>::nullValue()),
    pressureValueExpression_(groovyBCCommon<scalar>::nullValue()),
    driver_(this->patch())
{
    fvPatchVectorField::operator=(patchInternalField());
    gradient() = pTraits<vector>::zero;
}


groovyTractionDisplacementFvPatchVectorField::
groovyTractionDisplacementFvPatchVectorField
(
    const groovyTractionDisplacementFvPatchVectorField& tdpvf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedGradientFvPatchVectorField(tdpvf, p, iF, mapper),
#ifdef FOAM_NO_MAPPER_CONSTRUCTOR_FOR_FIELDS
    traction_(mapper(tdpvf.traction_)),
    pressure_(mapper(tdpvf.pressure_)),
#else
    traction_(tdpvf.traction_, mapper),
    pressure_(tdpvf.pressure_, mapper),
#endif
    tractionValueExpression_(tdpvf.tractionValueExpression_),
    pressureValueExpression_(tdpvf.pressureValueExpression_),
    driver_(this->patch(),tdpvf.driver_)
{}


groovyTractionDisplacementFvPatchVectorField::
groovyTractionDisplacementFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    fixedGradientFvPatchVectorField(p, iF),
    traction_("traction", dict, p.size()),
    pressure_("pressure", dict, p.size()),
    tractionValueExpression_(
        dict.lookup("tractionExpression"),
        dict
    ),
    pressureValueExpression_(
        dict.lookup("pressureExpression"),
        dict
    ),
    driver_(dict,this->patch())
{
    fvPatchVectorField::operator=(patchInternalField());
    gradient() = pTraits<vector>::zero;
    //    this->evaluate();
}


groovyTractionDisplacementFvPatchVectorField::
groovyTractionDisplacementFvPatchVectorField
(
    const groovyTractionDisplacementFvPatchVectorField& tdpvf
)
:
    fixedGradientFvPatchVectorField(tdpvf),
    traction_(tdpvf.traction_),
    pressure_(tdpvf.pressure_),
    tractionValueExpression_(tdpvf.tractionValueExpression_),
    pressureValueExpression_(tdpvf.pressureValueExpression_),
    driver_(this->patch(),tdpvf.driver_)
{}


groovyTractionDisplacementFvPatchVectorField::
groovyTractionDisplacementFvPatchVectorField
(
    const groovyTractionDisplacementFvPatchVectorField& tdpvf,
    const DimensionedField<vector, volMesh>& iF
)
:
    fixedGradientFvPatchVectorField(tdpvf, iF),
    traction_(tdpvf.traction_),
    pressure_(tdpvf.pressure_),
    tractionValueExpression_(tdpvf.tractionValueExpression_),
    pressureValueExpression_(tdpvf.pressureValueExpression_),
    driver_(this->patch(),tdpvf.driver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void groovyTractionDisplacementFvPatchVectorField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    fixedGradientFvPatchVectorField::autoMap(m);
#ifdef FOAM_AUTOMAP_NOT_MEMBER_OF_FIELD
    m(traction_, traction_);
    m(pressure_, pressure_);
#else
    traction_.autoMap(m);
    pressure_.autoMap(m);
#endif
}


void groovyTractionDisplacementFvPatchVectorField::rmap
(
    const fvPatchVectorField& ptf,
    const labelList& addr
)
{
    fixedGradientFvPatchVectorField::rmap(ptf, addr);

    const groovyTractionDisplacementFvPatchVectorField& dmptf =
        refCast<const groovyTractionDisplacementFvPatchVectorField>(ptf);

    traction_.rmap(dmptf.traction_, addr);
    pressure_.rmap(dmptf.pressure_, addr);
}


void groovyTractionDisplacementFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    driver_.clearVariables();

    this->traction()=driver_.evaluate<vector>(this->tractionValueExpression_);
    this->pressure()=driver_.evaluate<scalar>(this->pressureValueExpression_);

    const dictionary& mechanicalProperties =
        db().lookupObject<IOdictionary>("mechanicalProperties");

    const dictionary& thermalProperties =
        db().lookupObject<IOdictionary>("thermalProperties");


    const fvPatchField<scalar>& rho =
        patch().lookupPatchField<volScalarField, scalar>("rho");

    const fvPatchField<scalar>& rhoE =
        patch().lookupPatchField<volScalarField, scalar>("E");

    const fvPatchField<scalar>& nu =
        patch().lookupPatchField<volScalarField, scalar>("nu");

    scalarField E(rhoE/rho);
    scalarField mu(E/(2.0*(1.0 + nu)));
    scalarField lambda(nu*E/((1.0 + nu)*(1.0 - 2.0*nu)));
    scalarField threeK(E/(1.0 - 2.0*nu));

    if (
        readBool(
            mechanicalProperties.lookup("planeStress")
        )
    )
    {
        lambda = nu*E/((1.0 + nu)*(1.0 - nu));
        threeK = E/(1.0 - nu);
    }

    scalarField twoMuLambda(2*mu + lambda);

    vectorField n(patch().nf());

    const fvPatchField<symmTensor>& sigmaD =
        patch().lookupPatchField<volSymmTensorField, symmTensor>("sigmaD");

    gradient() =
    (
        (traction_ - pressure_*n)/rho
      + twoMuLambda*fvPatchField<vector>::snGrad() - (n & sigmaD)
    )/twoMuLambda;

    if (
        readBool(
            thermalProperties.lookup("thermalStress")
        )
    )
    {
        const fvPatchField<scalar>&  threeKalpha=
            patch().lookupPatchField<volScalarField, scalar>("threeKalpha");

        const fvPatchField<scalar>& T =
            patch().lookupPatchField<volScalarField, scalar>("T");

        gradient() += n*threeKalpha*T/twoMuLambda;
    }

    fixedGradientFvPatchVectorField::updateCoeffs();
}


void groovyTractionDisplacementFvPatchVectorField::write(Ostream& os) const
{
    //    fixedGradientFvPatchVectorField::write(os);
    fvPatchVectorField::write(os);
#ifdef FOAM_WRITEENTRY_NOT_MEMBER_OF_LIST
    writeEntry(os, "traction", traction_);
    writeEntry(os, "pressure", pressure_);
    writeEntry(os, "value", *this);
#else
    traction_.writeEntry("traction", os);
    pressure_.writeEntry("pressure", os);
    writeEntry("value", os);
#endif

    os.writeKeyword("tractionExpression")
        << tractionValueExpression_ << token::END_STATEMENT << nl;
    os.writeKeyword("pressureExpression")
        << pressureValueExpression_ << token::END_STATEMENT << nl;

    driver_.writeCommon(os,debug);

}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

makePatchTypeField
(
    fvPatchVectorField,
    groovyTractionDisplacementFvPatchVectorField
);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
