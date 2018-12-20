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
    2017 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "groovyBCJumpFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedJumpFvPatchField<Type>(p, iF),
    driver_(this->patch()),
    jumpExpression_("0")
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField 1" << endl;
    }
}


template<class Type>
groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField
(
    const groovyBCJumpFvPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedJumpFvPatchField<Type>(ptf, p, iF, mapper),
    driver_(this->patch(),ptf.driver_),
    jumpExpression_(ptf.jumpExpression_)
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField 2" << endl;
    }
}


template<class Type>
groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, volMesh>& iF,
    const dictionary& dict
)
:
    fixedJumpFvPatchField<Type>(p, iF),
    driver_(dict,this->patch()),
    jumpExpression_(
        dict.lookup("jumpExpression"),
        dict
    )
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField 3" << endl;
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
#ifdef FOAM_PSTREAM_COMMSTYPE_IS_ENUMCLASS
        this->evaluate(Pstream::commsTypes::blocking);
#else
        this->evaluate(Pstream::blocking);
#endif
    }
}


template<class Type>
groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField
(
    const groovyBCJumpFvPatchField<Type>& ptf
)
:
    fixedJumpFvPatchField<Type>(ptf),
    driver_(this->patch(),ptf.driver_),
    jumpExpression_(ptf.jumpExpression_)
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField 4" << endl;
    }
}


template<class Type>
groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField
(
    const groovyBCJumpFvPatchField<Type>& ptf,
    const DimensionedField<Type, volMesh>& iF
)
:
    fixedJumpFvPatchField<Type>(ptf, iF),
    driver_(this->patch(),ptf.driver_),
    jumpExpression_(ptf.jumpExpression_)
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::groovyBCJumpFvPatchField 5" << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

#ifdef FOAM_JUMP_IS_JUMP_CYCLIC
template<class Type>
tmp<Field<Type> > groovyBCJumpFvPatchField<Type>::jump() const
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::jump() with "
            << jumpExpression_ << endl;
    }

    PatchValueExpressionDriver &driver=const_cast<PatchValueExpressionDriver &>(driver_);

    driver.clearVariables();

    tmp<Field<Type> > tjf(
        new Field<Type>(this->size())
    );
    Field<Type> &jf=tjf();

    jf = driver.evaluate<Type>(this->jumpExpression_);

    return tjf;
}

#else

template<class Type>
void Foam::groovyBCJumpFvPatchField<Type>::updateCoeffs()
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::jump() with "
            << jumpExpression_ << endl;
    }

    driver_.clearVariables();

    this->jump_ = driver_.evaluate<Type>(this->jumpExpression_);

    fixedJumpFvPatchField<Type>::updateCoeffs();
}
#endif

template<class Type>
void groovyBCJumpFvPatchField<Type>::write(Ostream& os) const
{
    if(debug) {
        Info << "groovyBCJumpFvPatchField<Type>::write" << endl;
    }
    fixedJumpFvPatchField<Type>::write(os);
#ifdef FOAM_JUMP_IS_JUMP_CYCLIC
    os.writeKeyword("patchType") << "cyclic" << token::END_STATEMENT << nl;
    os.writeKeyword("jumpValue") << jump() << token::END_STATEMENT << nl;
#endif
    os.writeKeyword("jumpExpression")
        << jumpExpression_ << token::END_STATEMENT << nl;

    driver_.writeCommon(os,debug);

    //    this->writeEntry("value", os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
