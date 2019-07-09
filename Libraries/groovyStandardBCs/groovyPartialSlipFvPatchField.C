/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyPartialSlipFvPatchField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::groovyPartialSlipFvPatchField<Type>::groovyPartialSlipFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    partialSlipFvPatchField<Type>(p, iF),
    valueFractionExpression_("0"),
    driver_(this->patch())
{}


template<class Type>
Foam::groovyPartialSlipFvPatchField<Type>::groovyPartialSlipFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    partialSlipFvPatchField<Type>(p, iF),
    valueFractionExpression_(
        dict.lookup("fractionExpression"),
        dict
    ),
    driver_(dict,this->patch())
{
}


template<class Type>
Foam::groovyPartialSlipFvPatchField<Type>::groovyPartialSlipFvPatchField
(
    const groovyPartialSlipFvPatchField& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    partialSlipFvPatchField<Type>(ptf, p, iF, mapper),
    valueFractionExpression_(ptf.valueFractionExpression_),
    driver_(this->patch(),ptf.driver_)
{}


template<class Type>
Foam::groovyPartialSlipFvPatchField<Type>::groovyPartialSlipFvPatchField
(
    const groovyPartialSlipFvPatchField& tppsf
)
:
    partialSlipFvPatchField<Type>(tppsf),
    valueFractionExpression_(tppsf.valueFractionExpression_),
    driver_(this->patch(),tppsf.driver_)
{}


template<class Type>
Foam::groovyPartialSlipFvPatchField<Type>::groovyPartialSlipFvPatchField
(
    const groovyPartialSlipFvPatchField& tppsf,
    const DimensionedField<Type, volMesh>& iF
)
:
    partialSlipFvPatchField<Type>(tppsf, iF),
    valueFractionExpression_(tppsf.valueFractionExpression_),
    driver_(this->patch(),tppsf.driver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::groovyPartialSlipFvPatchField<Type>::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    driver_.clearVariables();

    this->valueFraction()=driver_.evaluate<scalar>(this->valueFractionExpression_);

    partialSlipFvPatchField<Type>::updateCoeffs();
}

template<class Type>
void Foam::groovyPartialSlipFvPatchField<Type>::write(Ostream& os) const
{
    partialSlipFvPatchField<Type>::write(os);

    os.writeKeyword("fractionExpression")
        << valueFractionExpression_ << token::END_STATEMENT << nl;

#ifdef FOAM_WRITEENTRY_NOT_MEMBER_OF_LIST
    writeEntry(os, "value", *this);
#else
    this->writeEntry("value",os);
#endif

    driver_.writeCommon(os,debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
