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
    2011, 2013-2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyBCCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "fvMesh.H"
#include "pointMesh.H"
#include "swak.H"

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
exprString groovyBCCommon<Type>::nullValue()
{
    if(string(pTraits<Type>::typeName)==string("vector")) {
        return exprString("vector(0,0,0)");
    } else if(string(pTraits<Type>::typeName)==string("tensor")) {
        return exprString("tensor(0,0,0,0,0,0,0,0,0)");
    } else if(string(pTraits<Type>::typeName)==string("symmTensor")) {
        return exprString("symmTensor(0,0,0,0,0,0)");
    } else if(string(pTraits<Type>::typeName)==string("sphericalTensor")) {
        return exprString("sphericalTensor(0)");
    } else {
        OStringStream tmp;
        tmp << pTraits<Type>::zero;
        return exprString(tmp.str().c_str());
    }
}

template<class Type>
const fvPatch &groovyBCCommon<Type>::getFvPatch(const pointPatch &pp) {
    if(!isA<fvMesh>(pp.boundaryMesh().mesh().db())) {
        FatalErrorIn("getFvPatch(const pointPatch &pp)")
            << " This will only work if I can find a fvMesh, but I only found a "
                << typeid(pp.boundaryMesh().mesh().db()).name()
                << endl
                << exit(FatalError);
    }
    const fvMesh &fv=dynamic_cast<const fvMesh &>(pp.boundaryMesh().mesh().db());
    return fv.boundary()[pp.index()];
}


template<class Type>
groovyBCCommon<Type>::groovyBCCommon
(
    bool hasGradient,
    bool isPoint,
    exprString fractionExpression
)
:
    evaluateDuringConstruction_(false),
    debug_(false),
    hasGradient_(hasGradient),
    fractionExpression_(
        isPoint ? "toPoint("+fractionExpression+")" : fractionExpression,
        dictionary::null
    )
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
    exprString fractionExpression
)
:
    evaluateDuringConstruction_(
        dict.lookupOrDefault<bool>("evaluateDuringConstruction",false)
    ),
    debug_(dict.lookupOrDefault<bool>("debug",false)),
    hasGradient_(hasGradient),
    fractionExpression_(
        dict.lookupOrDefault(
            "fractionExpression",
            isPoint ? string("toPoint("+fractionExpression+")") : string(fractionExpression)),
        dict
    )
{
    if (dict.found("valueExpression"))
    {
        valueExpression_=exprString(
            dict.lookup("valueExpression"),
            dict
        );
    } else {
        valueExpression_ = nullValue();
    }
    if (dict.found("gradientExpression") && hasGradient)
    {
        gradientExpression_=exprString(
            dict.lookup("gradientExpression"),
            dict
        );
    } else {
        gradientExpression_ = nullValue();
    }
    if(
        Pstream::parRun()
        &&
#ifdef FOAM_DEFAULTCOMMSTYPE_IS_METHOD
        Pstream::defaultCommsType()
#else
        Pstream::defaultCommsType
#endif
        ==
        Pstream::blocking
    ) {
        WarningIn("groovyBCCommon<Type>::groovyBCCommon")
            << "The commsType is set to 'blocking'. This might cause the run to"
                << " fail for groovyBC (or similar) like " << dict.name() << nl
                << "If you experience a MPI-related failure of this run go to "
                << "the file '$WM_PROJECT_DIR/etc/controlDict' and change the "
                << "setting 'commsType' to something different than 'blocking'"
                << endl;
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
