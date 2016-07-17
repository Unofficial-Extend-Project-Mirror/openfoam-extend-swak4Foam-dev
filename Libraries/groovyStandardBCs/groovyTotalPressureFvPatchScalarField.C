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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
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

Contributors/Copyright:
    2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyTotalPressureFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::groovyTotalPressureFvPatchScalarField::groovyTotalPressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    totalPressureFvPatchScalarField(p, iF),
    p0Expression_("0"),
    driver_(this->patch())
{}


Foam::groovyTotalPressureFvPatchScalarField::groovyTotalPressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    totalPressureFvPatchScalarField(p, iF,dict),
    p0Expression_(
        dict.lookup("p0Expression"),
        dict
    ),
    driver_(dict,this->patch())
{
}


Foam::groovyTotalPressureFvPatchScalarField::groovyTotalPressureFvPatchScalarField
(
    const groovyTotalPressureFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    totalPressureFvPatchScalarField(ptf, p, iF, mapper),
    p0Expression_(ptf.p0Expression_),
    driver_(this->patch(),ptf.driver_)
{}


Foam::groovyTotalPressureFvPatchScalarField::groovyTotalPressureFvPatchScalarField
(
    const groovyTotalPressureFvPatchScalarField& tppsf
)
:
    totalPressureFvPatchScalarField(tppsf),
    p0Expression_(tppsf.p0Expression_),
    driver_(this->patch(),tppsf.driver_)
{}


Foam::groovyTotalPressureFvPatchScalarField::groovyTotalPressureFvPatchScalarField
(
    const groovyTotalPressureFvPatchScalarField& tppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    totalPressureFvPatchScalarField(tppsf, iF),
    p0Expression_(tppsf.p0Expression_),
    driver_(this->patch(),tppsf.driver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::groovyTotalPressureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    driver_.clearVariables();

    p0()=driver_.evaluate<scalar>(this->p0Expression_);

    totalPressureFvPatchScalarField::updateCoeffs();
}

void Foam::groovyTotalPressureFvPatchScalarField::write(Ostream& os) const
{
    totalPressureFvPatchScalarField::write(os);

    os.writeKeyword("p0Expression")
        << p0Expression_ << token::END_STATEMENT << nl;

    driver_.writeCommon(os,debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        groovyTotalPressureFvPatchScalarField
    );
}

// ************************************************************************* //
