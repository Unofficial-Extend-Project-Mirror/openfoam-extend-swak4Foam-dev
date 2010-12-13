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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "groovyBCFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
string groovyBCFvPatchField<Type>::nullValue()
{
    if(string(pTraits<Type>::typeName)==string("vector")) {
        return string("vector(0,0,0)");
    } else if(string(pTraits<Type>::typeName)==string("tensor")) {
        return string("tensor(0,0,0,0,0,0,0,0,0)");
    } else if(string(pTraits<Type>::typeName)==string("symmTensor")) {
        return string("symmTensor(0,0,0,0,0,0)");
    } else if(string(pTraits<Type>::typeName)==string("sphericalTensor")) {
        return string("sphericalTensor(0)");
    } else {
        OStringStream tmp;
        tmp << pTraits<Type>::zero;
        return tmp.str();
    }
}

template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    mixedFvPatchField<Type>(p, iF),
    fractionExpression_("0"),
    driver_(this->patch())
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 1" << endl;
    }

    this->refValue() = pTraits<Type>::zero;
    valueExpression_ = nullValue();
    this->refGrad() = pTraits<Type>::zero;
    gradientExpression_ = nullValue();
    this->valueFraction() = 0.0;
}


template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const groovyBCFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mixedFvPatchField<Type>(ptf, p, iF, mapper),
    valueExpression_(ptf.valueExpression_),
    gradientExpression_(ptf.gradientExpression_),
    fractionExpression_(ptf.fractionExpression_),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 2" << endl;
    }
}


template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    mixedFvPatchField<Type>(p, iF),
    fractionExpression_(dict.lookupOrDefault("fractionExpression",string("1"))),
    driver_(this->patch())
{
    driver_.setVariableStrings(dict);

    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 3" << endl;
    }

    if (dict.found("valueExpression"))
    {
        dict.lookup("valueExpression") >> valueExpression_;
    } else {
        valueExpression_ = nullValue();
    }
    if (dict.found("gradientExpression"))
    {
        dict.lookup("gradientExpression") >> gradientExpression_;
    } else {
        gradientExpression_ = nullValue();
    }
    if(dict.found("timelines")) {
        driver_.readLines(dict.lookup("timelines"));
    }

    this->refValue() = pTraits<Type>::zero;

    if (dict.found("value"))
    {
        fvPatchField<Type>::operator=
        (
            Field<Type>("value", dict, p.size())
        );
    }
    else
    {
        fvPatchField<Type>::operator=(this->refValue());
    }

    this->refGrad() = pTraits<Type>::zero;
    this->valueFraction() = 1;
}


template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const groovyBCFvPatchField<Type>& ptf
)
:
    mixedFvPatchField<Type>(ptf),
    valueExpression_(ptf.valueExpression_),
    gradientExpression_(ptf.gradientExpression_),
    fractionExpression_(ptf.fractionExpression_),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 4" << endl;
    }
}


template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const groovyBCFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    mixedFvPatchField<Type>(ptf, iF),
    valueExpression_(ptf.valueExpression_),
    gradientExpression_(ptf.gradientExpression_),
    fractionExpression_(ptf.fractionExpression_),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void groovyBCFvPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::updateCoeffs" << endl;
        Info << "Value: " << valueExpression_ << endl;
        Info << "Gradient: " << gradientExpression_ << endl;
        Info << "Fraction: " << fractionExpression_ << endl;
        Info << "Variables: ";
        driver_.writeVariableStrings(Info) << endl;
    }
    if (this->updated())
    {
        return;
    }

    if(debug) {
        Info << "groovyBCFvPatchField<Type>::updateCoeffs - updating" << endl;
    }

    driver_.clearVariables();

    this->refValue() = driver_.evaluate<Type>(valueExpression_);
    this->refGrad() = driver_.evaluate<Type>(gradientExpression_);
    this->valueFraction() = driver_.evaluate<scalar>(fractionExpression_);
    
    mixedFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void groovyBCFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::write" << endl;
    }
    mixedFvPatchField<Type>::write(os);
    os.writeKeyword("valueExpression")
        << valueExpression_ << token::END_STATEMENT << nl;
    os.writeKeyword("gradientExpression")
        << gradientExpression_ << token::END_STATEMENT << nl;
    os.writeKeyword("fractionExpression")
        << fractionExpression_ << token::END_STATEMENT << nl;
    os.writeKeyword("variables");
    driver_.writeVariableStrings(os) << token::END_STATEMENT << nl;
    os.writeKeyword("timelines");
    driver_.writeLines(os);
    os << token::END_STATEMENT << nl;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
