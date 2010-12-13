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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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

#include "groovyBCPointPatchField.H"

#ifdef FOAM_DEV
#include "PointPatchFieldMapper.H"
#else
#include "pointPatchFieldMapper.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

const fvPatch &getFvPatch(const pointPatch &pp) {
    if(!isA<fvMesh>(pp.boundaryMesh().mesh().db())) {
        FatalErrorIn("getFvPatch(const pointPatch &pp)")
            << " This will only work if I can find a fvMesh, but I only found a "
                << typeid(pp.boundaryMesh().mesh().db()).name()
                << endl
                << abort(FatalError);
    }
    const fvMesh &fv=dynamic_cast<const fvMesh &>(pp.boundaryMesh().mesh().db());
    return fv.boundary()[pp.index()];
}


template<class Type>
string groovyBCPointPatchField<Type>::nullValue()
{
    if(string(pTraits<Type>::typeName)==string("vector")) {
        return string("toPoint(vector(0,0,0))");
    } else if(string(pTraits<Type>::typeName)==string("tensor")) {
        return string("toPoint(tensor(0,0,0,0,0,0,0,0,0))");
    } else if(string(pTraits<Type>::typeName)==string("symmTensor")) {
        return string("toPoint(symmTensor(0,0,0,0,0,0))");
    } else if(string(pTraits<Type>::typeName)==string("sphericalTensor")) {
        return string("toPoint(sphericalTensor(0))");
    } else {
        OStringStream tmp;
        tmp << "toPoint(" << pTraits<Type>::zero << ")";
        return tmp.str();
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCPointPatchField<Type>::groovyBCPointPatchField
(
    const pointPatch& p,
    const DimensionedField<Type, pointMesh>& iF
)
:
    mixedPointPatchFieldType(p, iF),
    fractionExpression_("toPoint(0)"),
    driver_(getFvPatch(this->patch()))
{
    this->refValue() = pTraits<Type>::zero;
    valueExpression_ = nullValue();
    //    this->refGrad() = pTraits<Type>::zero;
    //    gradientExpression_ = nullValue();
    this->valueFraction() = 0.0;
}


template<class Type>
groovyBCPointPatchField<Type>::groovyBCPointPatchField
(
    const pointPatch& p,
    const DimensionedField<Type, pointMesh>& iF,
    const dictionary& dict
)
:
    mixedPointPatchFieldType(p, iF),
    fractionExpression_(dict.lookupOrDefault("fractionExpression",string("toPoint(1)"))),
    driver_(getFvPatch(this->patch()))
{
    driver_.setVariableStrings(dict);

    if (dict.found("valueExpression"))
    {
        dict.lookup("valueExpression") >> valueExpression_;
    } else {
        valueExpression_ = nullValue();
    }
//     if (dict.found("gradientExpression"))
//     {
//         dict.lookup("gradientExpression") >> gradientExpression_;
//     } else {
//         gradientExpression_ = nullValue();
//     }
    if(dict.found("timelines")) {
        driver_.readLines(dict.lookup("timelines"));
    }

    this->refValue() = pTraits<Type>::zero;

    if (dict.found("value"))
    {
        Field<Type>::operator=
        (
            Field<Type>("value", dict, p.size())
        );
    }
    else
    {
        Field<Type>::operator=(this->refValue());
    }

    //    this->refGrad() = pTraits<Type>::zero;
    this->valueFraction() = 1;
}


template<class Type>
groovyBCPointPatchField<Type>::groovyBCPointPatchField
(
    const groovyBCPointPatchField<Type>& ptf,
    const pointPatch& p,
    const DimensionedField<Type, pointMesh>& iF,
    const pointPatchFieldMapper& mapper
)
:
    mixedPointPatchFieldType
    (
        ptf,
        p,
        iF,
        mapper
    ),
    valueExpression_(ptf.valueExpression_),
    //    gradientExpression_(ptf.gradientExpression_),
    fractionExpression_(ptf.fractionExpression_),
    driver_(getFvPatch(this->patch()),ptf.driver_)
{
}


template<class Type>
groovyBCPointPatchField<Type>::groovyBCPointPatchField
(
    const groovyBCPointPatchField<Type>& ptf,
    const DimensionedField<Type, pointMesh>& iF
)
:
    mixedPointPatchFieldType(ptf, iF),
    valueExpression_(ptf.valueExpression_),
    //    gradientExpression_(ptf.gradientExpression_),
    fractionExpression_(ptf.fractionExpression_),
    driver_(getFvPatch(this->patch()),ptf.driver_)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


// Evaluate patch field
template<class Type>
void groovyBCPointPatchField<Type>::updateCoeffs()
{
    if(debug) 
    {
        Info << "groovyBCFvPatchField<Type>::updateCoeffs" << endl;
        Info << "Value: " << valueExpression_ << endl;
        //        Info << "Gradient: " << gradientExpression_ << endl;
        Info << "Fraction: " << fractionExpression_ << endl;
        Info << "Variables: ";
        driver_.writeVariableStrings(Info)  << endl;
    }
//     if (this->updated())
//     {
//         return;
//     }

    driver_.clearVariables();

    this->refValue() = driver_.evaluate<Type>(valueExpression_,true);
    //    this->refGrad() = driver_.evaluate<Type>(gradientExpression_,true);
    this->valueFraction() = driver_.evaluate<scalar>(fractionExpression_,true);
    
    mixedPointPatchFieldType::updateCoeffs();
}


// Write
template<class Type>
void groovyBCPointPatchField<Type>::write(Ostream& os) const
{
    mixedPointPatchFieldType::write(os);

    this->writeEntry("value", os);

    os.writeKeyword("valueExpression")
        << valueExpression_ << token::END_STATEMENT << nl;
//     os.writeKeyword("gradientExpression")
//         << gradientExpression_ << token::END_STATEMENT << nl;
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
