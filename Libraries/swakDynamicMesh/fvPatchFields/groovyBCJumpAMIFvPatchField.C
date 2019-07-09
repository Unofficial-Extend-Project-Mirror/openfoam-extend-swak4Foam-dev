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
    2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"

#if defined(FOAM_HAS_AMI_INTERFACE) && !defined(FOAM_OLD_AMI_ACMI)

#include "groovyBCJumpAMIFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    jumpCyclicAMIFvPatchField<Type>(p, iF),
    driver_(this->patch()),
    jumpExpression_("0")
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField 1" << endl;
    }
}


template<class Type>
groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField
(
    const groovyBCJumpAMIFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    jumpCyclicAMIFvPatchField<Type>(ptf, p, iF, mapper),
    driver_(this->patch(),ptf.driver_),
    jumpExpression_(ptf.jumpExpression_)
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField 2" << endl;
    }
}


template<class Type>
groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    jumpCyclicAMIFvPatchField<Type>(p, iF),
    driver_(dict,this->patch()),
    jumpExpression_(
        dict.lookup("jumpExpression"),
        dict
    )
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField 3" << endl;
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
        this->evaluate(
#ifdef FOAM_PSTREAM_COMMSTYPE_IS_ENUMCLASS
            Pstream::commsTypes::blocking
#else
            Pstream::blocking
#endif
        );
    }
}


template<class Type>
groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField
(
    const groovyBCJumpAMIFvPatchField<Type>& ptf
)
:
    jumpCyclicAMIFvPatchField<Type>(ptf),
    driver_(this->patch(),ptf.driver_),
    jumpExpression_(ptf.jumpExpression_)
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField 4" << endl;
    }
}


template<class Type>
groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField
(
    const groovyBCJumpAMIFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    jumpCyclicAMIFvPatchField<Type>(ptf, iF),
    driver_(this->patch(),ptf.driver_),
    jumpExpression_(ptf.jumpExpression_)
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::groovyBCJumpAMIFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
tmp<Field<Type> > groovyBCJumpAMIFvPatchField<Type>::jump() const
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::jump() with "
            << jumpExpression_ << endl;
    }

    if(this->cyclicAMIPatch().owner()) {
        PatchValueExpressionDriver &driver
            = const_cast<PatchValueExpressionDriver &>(driver_);

        driver.clearVariables();

        tmp<Field<Type> > tjf(
            new Field<Type>(this->size())
        );
        Field<Type> &jf=const_cast<Field<Type>&>(tjf());

        jf = driver.evaluate<Type>(this->jumpExpression_);
        return tjf;
    } else {
       const groovyBCJumpAMIFvPatchField& nbrPatch =
           refCast<const groovyBCJumpAMIFvPatchField<Type> >(
               this->neighbourPatchField()
           );
       if (this->cyclicAMIPatch().applyLowWeightCorrection()) {
           return this->cyclicAMIPatch().interpolate(
               nbrPatch.jump(),
               Field<Type>(this->size(), Zero)
           );
       } else {
           return this->cyclicAMIPatch().interpolate(nbrPatch.jump());
       }
    }
}

template<class Type>
void groovyBCJumpAMIFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "groovyBCJumpAMIFvPatchField<Type>::write" << endl;
    }
    fvPatchField<Type>::write(os);

    os.writeKeyword("patchType") << "cyclic"
        << token::END_STATEMENT << nl;
    if(this->cyclicAMIPatch().owner()) {
        os.writeKeyword("jumpValue") << jump()
            << token::END_STATEMENT << nl;
    }
    os.writeKeyword("jumpExpression")
        << jumpExpression_ << token::END_STATEMENT << nl;

    driver_.writeCommon(os,debug);

#ifdef FOAM_WRITEENTRY_NOT_MEMBER_OF_LIST
    writeEntry(os, "value", *this);
#else
    this->writeEntry("value",os);
#endif
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#endif

// ************************************************************************* //
