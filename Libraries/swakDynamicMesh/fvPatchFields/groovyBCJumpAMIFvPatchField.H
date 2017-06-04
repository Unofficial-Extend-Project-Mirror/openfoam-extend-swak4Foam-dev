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
    Foam::groovyBCJumpAMIFvPatchField

Description
    Foam::groovyBCJumpAMIFvPatchField

SourceFiles
    groovyBCJumpAMIFvPatchField.C

Contributors/Copyright:
    2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyBCJumpAMIFvPatchField_H
#define groovyBCJumpAMIFvPatchField_H

#include "swak.H"

#if defined(FOAM_HAS_AMI_INTERFACE) && !defined(FOAM_OLD_AMI_ACMI)

#include "jumpCyclicAMIFvPatchField.H"

#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class groovyBCFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyBCJumpAMIFvPatchField
:
    public jumpCyclicAMIFvPatchField<Type>
{

protected:

    // Protected data

    PatchValueExpressionDriver driver_;

    exprString jumpExpression_;

public:

    //- Runtime type information
    TypeName("groovyBCJumpAMI");


    // Constructors

    //- Construct from patch and internal field
    groovyBCJumpAMIFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&
    );

    //- Construct from patch, internal field and dictionary
    groovyBCJumpAMIFvPatchField
    (
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const dictionary&
    );

    //- Construct by mapping given groovyBCJumpAMIFvPatchField onto a new patch
    groovyBCJumpAMIFvPatchField
    (
        const groovyBCJumpAMIFvPatchField<Type>&,
        const fvPatch&,
        const DimensionedField<Type, volMesh>&,
        const fvPatchFieldMapper&
    );

    //- Construct as copy
    groovyBCJumpAMIFvPatchField
    (
        const groovyBCJumpAMIFvPatchField<Type>&
    );

    //- Construct and return a clone
    virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
                (
                    new groovyBCJumpAMIFvPatchField<Type>(*this)
                );
        }

    //- Construct as copy setting internal field reference
    groovyBCJumpAMIFvPatchField
    (
        const groovyBCJumpAMIFvPatchField<Type>&,
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
                    new groovyBCJumpAMIFvPatchField<Type>(*this, iF)
                );
        }


    //- Return the "jump" across the patch as a "half" field
    tmp<Field<Type> > jump() const;

    // Member functions

    //- Write
    virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #ifdef NoRepository
    #   include "groovyBCJumpAMIFvPatchField.C"
    #endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

#endif

// ************************************************************************* //
