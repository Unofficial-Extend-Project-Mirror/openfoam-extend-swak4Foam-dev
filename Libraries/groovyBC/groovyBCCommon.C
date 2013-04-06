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
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "groovyBCCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
string groovyBCCommon<Type>::nullValue()
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
groovyBCCommon<Type>::groovyBCCommon
(
    bool hasGradient,
    bool isPoint,
    string fractionExpression
)
:
    evaluateDuringConstruction_(false),  
    debug_(false),
    hasGradient_(hasGradient),
    fractionExpression_(isPoint ? "toPoint("+fractionExpression+")" : fractionExpression)
{
    valueExpression_ = nullValue();
    if(hasGradient_) {
        gradientExpression_ = nullValue();
    }
}


template<class Type>
groovyBCCommon<Type>::groovyBCCommon
(
    const groovyBCCommon<Type>& ptf
)
:
    evaluateDuringConstruction_(ptf.evaluateDuringConstruction_),  
    debug_(ptf.debug_),
    hasGradient_(ptf.hasGradient_),
    valueExpression_(ptf.valueExpression_),
    gradientExpression_(ptf.gradientExpression_),
    fractionExpression_(ptf.fractionExpression_)
{
}


template<class Type>
groovyBCCommon<Type>::groovyBCCommon
(
    const dictionary& dict,
    bool hasGradient,
    bool isPoint,
    string fractionExpression
)
:
    evaluateDuringConstruction_(
        dict.lookupOrDefault<bool>("evaluateDuringConstruction",false)
    ),  
    debug_(dict.lookupOrDefault<bool>("debug",false)),
    hasGradient_(hasGradient),
    fractionExpression_(dict.lookupOrDefault(
                            "fractionExpression",
                            isPoint ? string("toPoint("+fractionExpression+")") : string(fractionExpression))
    )
{
    if (dict.found("valueExpression"))
    {
        dict.lookup("valueExpression") >> valueExpression_;
    } else {
        valueExpression_ = nullValue();
    }
    if (dict.found("gradientExpression") && hasGradient)
    {
        dict.lookup("gradientExpression") >> gradientExpression_;
    } else {
        gradientExpression_ = nullValue();
    }
}



// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void groovyBCCommon<Type>::write(Ostream& os) const
{
    os.writeKeyword("valueExpression")
        << valueExpression_ << token::END_STATEMENT << nl;
    if(hasGradient_) {
        os.writeKeyword("gradientExpression")
            << gradientExpression_ << token::END_STATEMENT << nl;
        os.writeKeyword("fractionExpression")
            << fractionExpression_ << token::END_STATEMENT << nl;
    }
    os.writeKeyword("evaluateDuringConstruction")
        << evaluateDuringConstruction_ << token::END_STATEMENT << nl;

    // debug_ not written on purpose
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
