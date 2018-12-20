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

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2009-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyBCFvPatchField.H"
#include "cyclicFvPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    mixedFvPatchField<Type>(p, iF),
    groovyBCCommon<Type>(true),
    driver_(this->patch()),
    cyclicSlave_(false)
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 1" << endl;
    }

    this->refValue() = pTraits<Type>::zero;
    this->refGrad() = pTraits<Type>::zero;
    this->valueFraction() = 1;
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
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_),
    cyclicSlave_(ptf.cyclicSlave_)
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
    groovyBCCommon<Type>(dict,true),
    driver_(dict,this->patch()),
    cyclicSlave_(dict.lookupOrDefault<bool>("cyclicSlave",false))
{
    const_cast<word&>(this->patchType()) = dict.lookupOrDefault<word>("patchType", word::null);
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 3" << endl;
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
	if (!dict.found("refValue")) {
 	    // make sure that refValue has a sensible value for the "update" below
	    this->refValue() = Field<Type>("value", dict, p.size());
	}
    }
    else
    {
        fvPatchField<Type>::operator=(this->refValue());
        WarningIn(
            "groovyBCFvPatchField<Type>::groovyBCFvPatchField"
            "("
            "const fvPatch& p,"
            "const DimensionedField<Type, volMesh>& iF,"
            "const dictionary& dict"
            ")"
        ) << "No value defined for "
#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
            << this->internalField().name()
#else
            << this->dimensionedInternalField().name()
#endif
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
        this->valueFraction() = Field<scalar>("valueFraction", dict, p.size());
    } else {
        this->valueFraction() = 1;
    }

    if(this->evaluateDuringConstruction()) {
        // make sure that this works with potentialFoam or other solvers that don't evaluate the BCs
        this->evaluate();
    } else {
        // emulate mixedFvPatchField<Type>::evaluate, but avoid calling "our" updateCoeffs
        if (!this->updated())
        {
            this->mixedFvPatchField<Type>::updateCoeffs();
        }

        Field<Type>::operator=
            (
                this->valueFraction()*this->refValue()
                +
                (1.0 - this->valueFraction())*
                (
                    this->patchInternalField()
                    + this->refGrad()/this->patch().deltaCoeffs()
                )
            );

        fvPatchField<Type>::evaluate();
    }
}


template<class Type>
groovyBCFvPatchField<Type>::groovyBCFvPatchField
(
    const groovyBCFvPatchField<Type>& ptf
)
:
    mixedFvPatchField<Type>(ptf),
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_),
    cyclicSlave_(ptf.cyclicSlave_)
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
    groovyBCCommon<Type>(ptf),
    driver_(this->patch(),ptf.driver_),
    cyclicSlave_(ptf.cyclicSlave_)
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::groovyBCFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
tmp<Field<Type>> groovyBCFvPatchField<Type>::patchNeighbourField() const
{
    if(isA<cyclicFvPatch>(this->patch())) {
#ifdef FOAM_CYCLIC_FV_PATCH_FIELD_HAS_NEIGHBOUR_PATCH
        // reimplement the cyclicFvPatchField<Type>::patchNeighbourField()

        const cyclicFvPatch &cyclicPatch=dynamicCast<const cyclicFvPatch>(this->patch());
        const Field<Type>& iField = this->primitiveField();
        const labelUList& nbrFaceCells =
            cyclicPatch.neighbPatch().faceCells();

        tmp<Field<Type>> tpnf(new Field<Type>(this->size()));
        Field<Type>& pnf = tpnf.ref();

        bool doTransform=!(cyclicPatch.parallel() || pTraits<Type>::rank==0);
        if (doTransform)
        {
            forAll(pnf, facei)
            {
                pnf[facei] = transform
                    (
                        cyclicPatch.forwardT()[0], iField[nbrFaceCells[facei]]
                    );
            }
        }
        else
        {
            forAll(pnf, facei)
            {
                pnf[facei] = iField[nbrFaceCells[facei]];
            }
        }
        return tpnf;
#else
        FatalErrorInFunction
            << "No patchNeighbourField in this OpenFOAM-version"
                << endl
                << exit(FatalError);
        return *this;
#endif
    } else {
        NotImplemented;
        return *this;
    }
}

template<class Type>
void groovyBCFvPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::updateCoeffs" << endl;
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
        Info << "groovyBCFvPatchField<Type>::updateCoeffs - updating" << endl;
    }

    if(
        isA<cyclicFvPatch>(this->patch())
        &&
        cyclicSlave_
    ) {
#ifdef FOAM_CYCLIC_FV_PATCH_FIELD_HAS_NEIGHBOUR_PATCH
        const cyclicFvPatch &cyclicPatch=dynamicCast<const cyclicFvPatch>(this->patch());
        const GeometricField<Type, fvPatchField, volMesh>& fld =
            static_cast<const GeometricField<Type, fvPatchField, volMesh>&>
            (
                this->primitiveField()
            );

        if(
            !isA<groovyBCFvPatchField<Type> >(
                fld.boundaryField()[
                    cyclicPatch.neighbPatchID()
                ]
            )
        ) {
            FatalErrorInFunction
                << "specified 'cyclicSlave' on " << this->patch().name() << " but "
                    << fld.boundaryField()[cyclicPatch.neighbPatchID()].patch().name()
                    << " is not a groovyBC"
                    << endl
                    << exit(FatalError);
        }

        groovyBCFvPatchField<Type> &other=const_cast<groovyBCFvPatchField<Type>& >(
            dynamicCast<const groovyBCFvPatchField<Type> >(
                fld.boundaryField()[
                    cyclicPatch.neighbPatchID()
                ]
            )
        );
        other.updateCoeffs();
        this->refValue() = other.refValue();
        this->refGrad() = other.refGrad();
        this->valueFraction() = other.valueFraction();
#else
        FatalErrorInFunction
            << "No patchNeighbourField in this OpenFOAM-version"
                << endl
                << exit(FatalError);
#endif
    } else {
        driver_.clearVariables();

        this->refValue() = driver_.evaluate<Type>(this->valueExpression_);
        this->refGrad() = driver_.evaluate<Type>(this->gradientExpression_);
        this->valueFraction() = driver_.evaluate<scalar>(this->fractionExpression_);
    }

    mixedFvPatchField<Type>::updateCoeffs();
}


template<class Type>
void groovyBCFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "groovyBCFvPatchField<Type>::write" << endl;
    }
    mixedFvPatchField<Type>::write(os);
    groovyBCCommon<Type>::write(os);

    os.writeKeyword("cyclicSlave")
        << cyclicSlave_ << token::END_STATEMENT << nl;

    driver_.writeCommon(os,this->debug_ || debug);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
