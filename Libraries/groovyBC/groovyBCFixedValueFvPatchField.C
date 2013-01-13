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
    2009-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyBCFixedValueFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedValueFvPatchField<Type>(p, iF),
    groovyBCCommon<Type>(false),
    driver_(this->patch())
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField 1" << endl;
    }
}


template<class Type>
groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField
(
    const groovyBCFixedValueFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchField<Type>(ptf, p, iF, mapper),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField 2" << endl;
    }
}


template<class Type>
groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchField<Type>(p, iF),
    groovyBCCommon<Type>(dict,false),
    driver_(dict,this->patch())
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField 3" << endl;
    }

    driver_.readVariablesAndTables(dict);

    if (dict.found("value"))
    {
        fvPatchField<Type>::operator=
        (
            Field<Type>("value", dict, p.size())
        );
    }
    else
    {
        (*this)==pTraits<Type>::zero;

        WarningIn(
            "groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField"
            "("
            "const fvPatch& p,"
            "const DimensionedField<Type, volMesh>& iF,"
            "const dictionary& dict"
            ")"
        ) << "No value defined for " << this->dimensionedInternalField().name()
            << " on " << this->patch().name() << " therefore would be undefined "
            << pTraits<Type>::zero
            << endl;
    }

    if(this->evaluateDuringConstruction()) {
        // make sure that this works with potentialFoam or other solvers that don't evaluate the BCs
        this->evaluate();
    } else {
        // original does nothing
    }
}


template<class Type>
groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField
(
    const groovyBCFixedValueFvPatchField<Type>& ptf
)
:
    fixedValueFvPatchField<Type>(ptf),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField 4" << endl;
    }
}


template<class Type>
groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField
(
    const groovyBCFixedValueFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedValueFvPatchField<Type>(ptf, iF),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_)
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::groovyBCFixedValueFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void groovyBCFixedValueFvPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::updateCoeffs" << endl;
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
        Info << "groovyBCFixedValueFvPatchField<Type>::updateCoeffs - updating" << endl;
    }

    driver_.clearVariables();

    (*this) == driver_.evaluate<Type>(this->valueExpression_);

    fixedValueFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void groovyBCFixedValueFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "groovyBCFixedValueFvPatchField<Type>::write" << endl;
    }
    fixedValueFvPatchField<Type>::write(os);
    groovyBCCommon<Type>::write(os);

    driver_.writeCommon(os,this->debug_ || debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
