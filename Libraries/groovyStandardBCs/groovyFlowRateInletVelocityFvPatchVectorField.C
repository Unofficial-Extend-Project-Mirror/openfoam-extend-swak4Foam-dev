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
    \\  /    A nd           | Copyright (C) 2006-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "groovyFlowRateInletVelocityFvPatchVectorField.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::
groovyFlowRateInletVelocityFvPatchVectorField::
groovyFlowRateInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    fixedValueFvPatchField<vector>(p, iF),
    phiName_("phi"),
    rhoName_("rho"),
#else
    flowRateInletVelocityFvPatchVectorField(p, iF),
#endif
    flowRateExpression_("0"),
    driver_(this->patch())
{}


Foam::
groovyFlowRateInletVelocityFvPatchVectorField::
groovyFlowRateInletVelocityFvPatchVectorField
(
    const groovyFlowRateInletVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    fixedValueFvPatchField<vector>(ptf, p, iF, mapper),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
#else
    flowRateInletVelocityFvPatchVectorField(ptf, p, iF, mapper),
#endif
    flowRateExpression_(ptf.flowRateExpression_),
    driver_(this->patch(),ptf.driver_)
{}


Foam::
groovyFlowRateInletVelocityFvPatchVectorField::
groovyFlowRateInletVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    fixedValueFvPatchField<vector>(p, iF, dict),
    phiName_(dict.lookupOrDefault<word>("phi", "phi")),
    rhoName_(dict.lookupOrDefault<word>("rho", "rho")),
#else
    flowRateInletVelocityFvPatchVectorField(p, iF, dict),
#endif
    flowRateExpression_(dict.lookup("flowRateExpression")),
    driver_(dict,this->patch())
{
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    WarningIn(
        "groovyFlowRateInletVelocityFvPatchVectorField::"
        "groovyFlowRateInletVelocityFvPatchVectorField"
        "("
        "const fvPatch& p,"
        "const DimensionedField<vector, volMesh>& iF,"
        "const dictionary& dict"
        ")"
    ) 
        << "Starting with OF 2.1 this boundary condition is deprecated. Use the"
            << " regular flowRateInletVelocityFvPatch with the swak4Foam-DataEntry"
            << endl
            << endl;
#endif
}


Foam::
groovyFlowRateInletVelocityFvPatchVectorField::
groovyFlowRateInletVelocityFvPatchVectorField
(
    const groovyFlowRateInletVelocityFvPatchVectorField& ptf
)
:
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    fixedValueFvPatchField<vector>(ptf),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
#else
    flowRateInletVelocityFvPatchVectorField(ptf),
#endif
    flowRateExpression_(ptf.flowRateExpression_),
    driver_(this->patch(),ptf.driver_)
{}


Foam::
groovyFlowRateInletVelocityFvPatchVectorField::
groovyFlowRateInletVelocityFvPatchVectorField
(
    const groovyFlowRateInletVelocityFvPatchVectorField& ptf,
    const DimensionedField<vector, volMesh>& iF
)
:
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    fixedValueFvPatchField<vector>(ptf, iF),
    phiName_(ptf.phiName_),
    rhoName_(ptf.rhoName_),
#else
    flowRateInletVelocityFvPatchVectorField(ptf, iF),
#endif
    flowRateExpression_(ptf.flowRateExpression_),
    driver_(this->patch(),ptf.driver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::groovyFlowRateInletVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    driver_.clearVariables();

#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    const scalar flowRate=driver_.evaluateUniform<scalar>(this->flowRateExpression_);

    // Borrowed from the regular flowRateInletVelocityFvPatchVectorField
    const scalar avgU = -flowRate/gSum(patch().magSf());

    tmp<vectorField> n = patch().nf();

    const surfaceScalarField& phi =
        db().lookupObject<surfaceScalarField>(phiName_);

    if (phi.dimensions() == dimVelocity*dimArea)
    {
        // volumetric flow-rate
        operator==(n*avgU);
    }
    else if (phi.dimensions() == dimDensity*dimVelocity*dimArea)
    {
        const fvPatchField<scalar>& rhop =
            patch().lookupPatchField<volScalarField, scalar>(rhoName_);

        // mass flow-rate
        operator==(n*avgU/rhop);
    }
    else
    {
        FatalErrorIn
        (
            "flowRateInletVelocityFvPatchVectorField::updateCoeffs()"
        )   << "dimensions of " << phiName_ << " are incorrect" << nl
            << "    on patch " << this->patch().name()
            << " of field " << this->dimensionedInternalField().name()
            << " in file " << this->dimensionedInternalField().objectPath()
            << nl << exit(FatalError);
    }

    fixedValueFvPatchField<vector>::updateCoeffs();
#else
    flowRate()=driver_.evaluateUniform<scalar>(this->flowRateExpression_);

    flowRateInletVelocityFvPatchVectorField::updateCoeffs();
#endif
}


void Foam::groovyFlowRateInletVelocityFvPatchVectorField::write(Ostream& os) const
{
#if FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2
    fixedValueFvPatchField<vector>::write(os);
#else
    flowRateInletVelocityFvPatchVectorField::write(os);
#endif

    os.writeKeyword("flowRateExpression")
        << flowRateExpression_ << token::END_STATEMENT << nl;
    
    driver_.writeCommon(os,debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
   makePatchTypeField
   (
       fvPatchVectorField,
       groovyFlowRateInletVelocityFvPatchVectorField
   );
}


// ************************************************************************* //
