/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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

\*---------------------------------------------------------------------------*/

#include "groovyBCFaPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCFaPatchField<Type>::groovyBCFaPatchField
(
    const faPatch& p,
    const DimensionedField<Type, areaMesh>& iF
)
:
    mixedFaPatchField<Type>(p, iF),
    groovyBCCommon<Type>(true),
    driver_(this->patch())
{
    this->refValue() = pTraits<Type>::zero;
    this->refGrad() = pTraits<Type>::zero;
    this->valueFraction() = 0.0;
}


template<class Type>
groovyBCFaPatchField<Type>::groovyBCFaPatchField
(
    const groovyBCFaPatchField<Type>& ptf,
    const faPatch& p,
    const DimensionedField<Type, areaMesh>& iF,
    const faPatchFieldMapper& mapper
)
:
    mixedFaPatchField<Type>(ptf, p, iF, mapper),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{}


template<class Type>
groovyBCFaPatchField<Type>::groovyBCFaPatchField
(
    const faPatch& p,
    const DimensionedField<Type, areaMesh>& iF,
    const dictionary& dict
)
:
    mixedFaPatchField<Type>(p, iF),
    groovyBCCommon<Type>(dict,true),
    driver_(this->patch())
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 3" << endl;
    }

    driver_.readVariablesAndTables(dict);

    this->refValue() = this->patchInternalField();

    if (dict.found("value"))
    {
        faPatchField<Type>::operator=
        (
            Field<Type>("value", dict, p.size())
        );
    }
    else
    {
        faPatchField<Type>::operator=(this->refValue());
        WarningIn(
            "groovyBCFaPatchField<Type>::groovyBCFaPatchField"
            "("
            "const faPatch& p,"
            "const DimensionedField<Type, areaMesh>& iF,"
            "const dictionary& dict"
            ")"
        ) << "No value defined for " << this->dimensionedInternalField().name()
            << " on " << this->patch().name() << " therefore using "
            << "the internal field next to the patch"
            << endl;
    }

    this->refGrad() = pTraits<Type>::zero;
    this->valueFraction() = 1;
}


template<class Type>
groovyBCFaPatchField<Type>::groovyBCFaPatchField
(
    const groovyBCFaPatchField<Type>& ptf
)
:
    mixedFaPatchField<Type>(ptf),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{}


template<class Type>
groovyBCFaPatchField<Type>::groovyBCFaPatchField
(
    const groovyBCFaPatchField<Type>& ptf,
    const DimensionedField<Type, areaMesh>& iF
)
:
    mixedFaPatchField<Type>(ptf, iF),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void groovyBCFaPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "groovyBCFaPatchField<Type>::updateCoeffs" << endl;
        Info << "Value: " << this->valueExpression_ << endl;
        Info << "Gradient: " << this->gradientExpression_ << endl;
        Info << "Fraction: " << this->fractionExpression_ << endl;
        Info << "Variables: ";
        driver_.writeVariableStrings(Info) << endl;
    }

    if (this->updated())
    {
        return;
    }

     driver_.clearVariables();

    this->refValue() = driver_.evaluate<Type>(this->valueExpression_);
    this->refGrad() = driver_.evaluate<Type>(this->gradientExpression_);
    this->valueFraction() = driver_.evaluate<scalar>(this->fractionExpression_);

    mixedFaPatchField<Type>::updateCoeffs();
}


template<class Type>
void groovyBCFaPatchField<Type>::write(Ostream& os) const
{
    mixedFaPatchField<Type>::write(os);
    groovyBCCommon<Type>::write(os);

    driver_.writeCommon(os,this->debug_ || debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
