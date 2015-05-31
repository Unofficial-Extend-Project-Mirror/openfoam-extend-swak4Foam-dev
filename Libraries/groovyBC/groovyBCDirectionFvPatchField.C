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
    2011-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyBCDirectionFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    directionMixedFvPatchField<Type>(p, iF),
    groovyBCCommon<Type>(
        true,
        false,
        "symmTensor(1,0,0,1,0,1)"
    ),
    driver_(this->patch())
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField 1" << endl;
    }

    this->refValue() = pTraits<Type>::zero;
    this->refGrad() = pTraits<Type>::zero;
}


template<class Type>
groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField
(
    const groovyBCDirectionFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    directionMixedFvPatchField<Type>(ptf, p, iF, mapper),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField 2" << endl;
    }
}


template<class Type>
groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    directionMixedFvPatchField<Type>(p, iF),
    groovyBCCommon<Type>(
        dict,
        true,
        false,
        "symmTensor(1,0,0,1,0,1)"
    ),
    driver_(dict,this->patch())
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField 3" << endl;
    }

    driver_.readVariablesAndTables(dict);

    if (dict.found("refValue")) {
        this->refValue() = Field<Type>("refValue", dict, p.size());
    } else {
        this->refValue() = this->patchInternalField();
    }

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
        WarningIn(
            "groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField"
            "("
            "const fvPatch& p,"
            "const DimensionedField<Type, volMesh>& iF,"
            "const dictionary& dict"
            ")"
        ) << "No value defined for " << this->dimensionedInternalField().name()
            << " on " << this->patch().name() << " therefore using "
            << "the internal field next to the patch"
            << endl;
    }

    if (dict.found("refGradient")) {
        this->refGrad() = Field<Type>("refGradient", dict, p.size());
    } else {
        this->refGrad() = pTraits<Type>::zero;
    }

    if (dict.found("valueFraction")) {
        this->valueFraction() = Field<symmTensor>("valueFraction", dict, p.size());
    } else {
        this->valueFraction() = I;
    }

    if(this->evaluateDuringConstruction()) {
        // make sure that this works with potentialFoam or other solvers that don't evaluate the BCs
        this->evaluate();
    } else {
        // emulate the evaluate of the parent-class
        if (!this->updated())
        {
            this->directionMixedFvPatchField<Type>::updateCoeffs();
        }

        tmp<Field<Type> > normalValue = transform(this->valueFraction(), this->refValue());

        tmp<Field<Type> > gradValue =
            this->patchInternalField() + this->refGrad()/this->patch().deltaCoeffs();

        tmp<Field<Type> > transformGradValue =
            transform(I - this->valueFraction(), gradValue);

        Field<Type>::operator=(normalValue + transformGradValue);

        transformFvPatchField<Type>::evaluate();
    }
}


template<class Type>
groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField
(
    const groovyBCDirectionFvPatchField<Type>& ptf
)
:
    directionMixedFvPatchField<Type>(ptf),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField 4" << endl;
    }
}


template<class Type>
groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField
(
    const groovyBCDirectionFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    directionMixedFvPatchField<Type>(ptf, iF),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::groovyBCDirectionFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void groovyBCDirectionFvPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::updateCoeffs" << endl;
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

    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::updateCoeffs - updating" << endl;
    }

    driver_.clearVariables();

    this->refValue() = driver_.evaluate<Type>(this->valueExpression_);
    this->refGrad() = driver_.evaluate<Type>(this->gradientExpression_);
    this->valueFraction() = driver_.evaluate<symmTensor>(this->fractionExpression_);

    directionMixedFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void groovyBCDirectionFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "groovyBCDirectionFvPatchField<Type>::write" << endl;
    }
    directionMixedFvPatchField<Type>::write(os);
    groovyBCCommon<Type>::write(os);

    driver_.writeCommon(os,this->debug_ || debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
