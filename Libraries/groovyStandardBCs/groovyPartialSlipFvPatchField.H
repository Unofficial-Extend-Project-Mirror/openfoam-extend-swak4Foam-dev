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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::groovyPartialSlipFvPatchField

Description
    Foam::groovyPartialSlipFvPatchField

SourceFiles
    groovyPartialSlipFvPatchField.C

Contributors/Copyright:
    2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyPartialSlipFvPatchField_H
#define groovyPartialSlipFvPatchField_H

#include "partialSlipFvPatchField.H"

#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class groovyTotalPressureFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyPartialSlipFvPatchField
:
    public partialSlipFvPatchField<Type>
{
    // Private data

    exprString valueFractionExpression_;

    PatchValueExpressionDriver driver_;

public:

    //- Runtime type information
    TypeName("groovyPartialSlip");


    // Constructors

        //- Construct from patch and internal field
        groovyPartialSlipFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        groovyPartialSlipFvPatchField
        (
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given groovyPartialSlipFvPatchField
        //  onto a new patch
        groovyPartialSlipFvPatchField
        (
            const groovyPartialSlipFvPatchField&,
            const fvPatch&,
            const DimensionedField<Type, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        groovyPartialSlipFvPatchField
        (
            const groovyPartialSlipFvPatchField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchField<Type> > clone() const
        {
            return tmp<fvPatchField<Type> >
            (
                new groovyPartialSlipFvPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        groovyPartialSlipFvPatchField
        (
            const groovyPartialSlipFvPatchField&,
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
                new groovyPartialSlipFvPatchField<Type>(*this, iF)
            );
        }

        // Mapping functions

        // Evaluation functions

            //- Update the coefficients associated with the patch field
            virtual void updateCoeffs();


        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #ifdef NoRepository
    #   include "groovyPartialSlipFvPatchField.C"
    #endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
