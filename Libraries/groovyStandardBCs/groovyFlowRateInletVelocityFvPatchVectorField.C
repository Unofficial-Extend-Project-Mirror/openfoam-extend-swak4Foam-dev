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
    flowRateInletVelocityFvPatchVectorField(p, iF),
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
    flowRateInletVelocityFvPatchVectorField(ptf, p, iF, mapper),
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
    flowRateInletVelocityFvPatchVectorField(p, iF, dict),
    flowRateExpression_(dict.lookup("flowRateExpression")),
    driver_(dict,this->patch())
{}


Foam::
groovyFlowRateInletVelocityFvPatchVectorField::
groovyFlowRateInletVelocityFvPatchVectorField
(
    const groovyFlowRateInletVelocityFvPatchVectorField& ptf
)
:
    flowRateInletVelocityFvPatchVectorField(ptf),
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
    flowRateInletVelocityFvPatchVectorField(ptf, iF),
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

    flowRate()=driver_.evaluateUniform<scalar>(this->flowRateExpression_);
    
    flowRateInletVelocityFvPatchVectorField::updateCoeffs();
}


void Foam::groovyFlowRateInletVelocityFvPatchVectorField::write(Ostream& os) const
{
    flowRateInletVelocityFvPatchVectorField::write(os);

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
