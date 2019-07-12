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

#include "groovyFixedNormalSlipFvPatchField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "surfaceFields.H"

#include "groovyBCCommon.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::groovyFixedNormalSlipFvPatchField<Type>::groovyFixedNormalSlipFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedNormalSlipFvPatchField<Type>(p, iF),
    fixedValueExpression_(groovyBCCommon<Type>::nullValue()),
    driver_(this->patch())
{}


template<class Type>
Foam::groovyFixedNormalSlipFvPatchField<Type>::groovyFixedNormalSlipFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fixedNormalSlipFvPatchField<Type>(p, iF),
    fixedValueExpression_(
        dict.lookup("fixedValueExpression"),
        dict
    ),
    driver_(dict,this->patch())
{
    this->evaluate();
}


template<class Type>
Foam::groovyFixedNormalSlipFvPatchField<Type>::groovyFixedNormalSlipFvPatchField
(
    const groovyFixedNormalSlipFvPatchField& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedNormalSlipFvPatchField<Type>(ptf, p, iF, mapper),
    fixedValueExpression_(ptf.fixedValueExpression_),
    driver_(this->patch(),ptf.driver_)
{}


template<class Type>
Foam::groovyFixedNormalSlipFvPatchField<Type>::groovyFixedNormalSlipFvPatchField
(
    const groovyFixedNormalSlipFvPatchField& tppsf
)
:
    fixedNormalSlipFvPatchField<Type>(tppsf),
    fixedValueExpression_(tppsf.fixedValueExpression_),
    driver_(this->patch(),tppsf.driver_)
{}


template<class Type>
Foam::groovyFixedNormalSlipFvPatchField<Type>::groovyFixedNormalSlipFvPatchField
(
    const groovyFixedNormalSlipFvPatchField& tppsf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedNormalSlipFvPatchField<Type>(tppsf, iF),
    fixedValueExpression_(tppsf.fixedValueExpression_),
    driver_(this->patch(),tppsf.driver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::groovyFixedNormalSlipFvPatchField<Type>::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    driver_.clearVariables();

    this->fixedValue()=driver_.evaluate<Type>(this->fixedValueExpression_);

    fixedNormalSlipFvPatchField<Type>::updateCoeffs();
}

template<class Type>
void Foam::groovyFixedNormalSlipFvPatchField<Type>::write(Ostream& os) const
{
    fixedNormalSlipFvPatchField<Type>::write(os);

#ifdef FOAM_WRITEENTRY_NOT_MEMBER_OF_LIST
    writeEntry(os, "value", *this);
#else
    this->writeEntry("value",os);
#endif

    os.writeKeyword("fixedValueExpression")
        << fixedValueExpression_ << token::END_STATEMENT << nl;

    driver_.writeCommon(os,debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
