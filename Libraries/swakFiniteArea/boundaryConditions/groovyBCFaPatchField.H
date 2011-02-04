/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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
    groovyBCFaPatchField

Description

SourceFiles
    groovyBCFaPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef groovyBCFaPatchField_H
#define groovyBCFaPatchField_H

#include "mixedFaPatchField.H"

#include "FaPatchValueExpressionDriver.H"

#include "groovyBCCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class groovyBCFaPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyBCFaPatchField
:
    public mixedFaPatchField<Type>,
    public groovyBCCommon<Type>
{

protected:

    // Protected data

    FaPatchValueExpressionDriver driver_;

public:

    //- Runtime type information
    TypeName("groovyBC");


    // Constructors

        //- Construct from patch and internal field
        groovyBCFaPatchField
        (
            const faPatch&,
            const DimensionedField<Type, areaMesh>&
        );

        //- Construct from patch, internal field and dictionary
        groovyBCFaPatchField
        (
            const faPatch&,
            const DimensionedField<Type, areaMesh>&,
            const dictionary&
        );

        //- Construct by mapping given groovyBCFaPatchField onto a new patch
        groovyBCFaPatchField
        (
            const groovyBCFaPatchField<Type>&,
            const faPatch&,
            const DimensionedField<Type, areaMesh>&,
            const faPatchFieldMapper&
        );

        //- Construct as copy
        groovyBCFaPatchField
        (
            const groovyBCFaPatchField<Type>&
        );

        //- Construct and return a clone
        virtual tmp<faPatchField<Type> > clone() const
        {
            return tmp<faPatchField<Type> >
            (
                new groovyBCFaPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        groovyBCFaPatchField
        (
            const groovyBCFaPatchField<Type>&,
            const DimensionedField<Type, areaMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<faPatchField<Type> > clone
        (
            const DimensionedField<Type, areaMesh>& iF
        ) const
        {
            return tmp<faPatchField<Type> >
            (
                new groovyBCFaPatchField<Type>(*this, iF)
            );
        }


    // Member functions

        //- Update the coefficients associated with the patch field
        virtual void updateCoeffs();

        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "groovyBCFaPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
