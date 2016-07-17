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

Class
    Foam::groovyBCJumpFvPatchField

Description
    Foam::groovyBCJumpFvPatchField

SourceFiles
    groovyBCJumpFvPatchField.C

Contributors/Copyright:
    2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyBCJumpFvPatchField_H
#define groovyBCJumpFvPatchField_H

#include "swak.H"

#ifdef FOAM_JUMP_IS_JUMP_CYCLIC
#include "jumpCyclicFvPatchField.H"
#define fixedJumpFvPatchField jumpCyclicFvPatchField
#else
#include "fixedJumpFvPatchField.H"
#endif

#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class groovyBCFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyBCJumpFvPatchField
:
    public fixedJumpFvPatchField<Type>
{

protected:

    // Protected data

    PatchValueExpressionDriver driver_;

    exprString jumpExpression_;

public:

    //- Runtime type information
    TypeName("groovyBCJump");


    // Constructors

    //- Construct from patch and internal field
    groovyBCJumpFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct from patch, internal field and dictionary
    groovyBCJumpFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const dictionary&
    );

    //- Construct by mapping given groovyBCJumpFvPatchField onto a new patch
    groovyBCJumpFvPatchField
    (
        const groovyBCJumpFvPatchField<Type>&,
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const fvPatchFieldMapper&
    );

    //- Construct as copy
    groovyBCJumpFvPatchField
    (
        const groovyBCJumpFvPatchField<Type>&
    );

    //- Construct and return a clone
    virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
                (
                    new groovyBCJumpFvPatchField<Type>(*this)
                );
        }

    //- Construct as copy setting internal field reference
    groovyBCJumpFvPatchField
    (
        const groovyBCJumpFvPatchField<Type>&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct and return a clone setting internal field reference
    virtual tmp<fvPatchField<Type> > clone
    (
        const DimensionedField<Type, volMesh>& iF
    ) const
        {
            return tmp<fvPatchField<Type> >
                (
                    new groovyBCJumpFvPatchField<Type>(*this, iF)
                );
        }


#ifdef FOAM_JUMP_IS_JUMP_CYCLIC
    //- Return the "jump" across the patch as a "half" field
    tmp<Field<scalar> > jump() const;
#else
    // Evaluation functions

    //- Update the coefficients associated with the patch field
    virtual void updateCoeffs();
#endif
    // Member functions

    //- Write
    virtual void write(Ostream&) const;
};

#ifndef FOAM_JUMP_IS_JUMP_CYCLIC
//- Specialisation of the jump-condition for the pressure
template<>
void groovyBCJumpFvPatchField<scalar>::updateCoeffs();
#endif

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #ifdef NoRepository
    #   include "groovyBCJumpFvPatchField.C"
    #endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #endif

// ************************************************************************* //
