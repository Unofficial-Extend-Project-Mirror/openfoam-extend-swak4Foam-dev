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
    \\  /    A nd           | Copyright (C) 2006-2010 OpenCFD Ltd.
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
    Foam::groovyFlowRateInletVelocityFvPatchVectorField

Description

SourceFiles
    groovyFlowRateInletVelocityFvPatchVectorField.C

Contributors/Copyright:
    2011, 2013-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyFlowRateInletVelocityFvPatchVectorField_H
#define groovyFlowRateInletVelocityFvPatchVectorField_H

#include "flowRateInletVelocityFvPatchVectorField.H"

#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#if (FOAM_VERSION4SWAK_MINOR>=1 && FOAM_VERSION4SWAK_MAJOR>=2) || FOAM_VERSION4SWAK_MAJOR>2
#define FOAM_FLOWRATE_BASED_ON_FIXEDVALUE
#endif

namespace Foam
{
/*---------------------------------------------------------------------------*\
               Class groovyFlowRateInletVelocityFvPatch Declaration
\*---------------------------------------------------------------------------*/

class groovyFlowRateInletVelocityFvPatchVectorField
:
#ifdef FOAM_FLOWRATE_BASED_ON_FIXEDVALUE
    public fixedValueFvPatchVectorField
#else
    public flowRateInletVelocityFvPatchVectorField
#endif
{
    // Private data

#ifdef FOAM_FLOWRATE_BASED_ON_FIXEDVALUE
        //- Name of the flux transporting the field
        word phiName_;

        //- Name of the density field used to normalize the mass flux
        word rhoName_;
#endif

    exprString flowRateExpression_;

    PatchValueExpressionDriver driver_;

public:

   //- Runtime type information
   TypeName("groovyFlowRateInletVelocity");


   // Constructors

        //- Construct from patch and internal field
        groovyFlowRateInletVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        groovyFlowRateInletVelocityFvPatchVectorField
        (
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given
        //  groovyFlowRateInletVelocityFvPatchVectorField
        //  onto a new patch
        groovyFlowRateInletVelocityFvPatchVectorField
        (
            const groovyFlowRateInletVelocityFvPatchVectorField&,
            const fvPatch&,
            const DimensionedField<vector, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        groovyFlowRateInletVelocityFvPatchVectorField
        (
            const groovyFlowRateInletVelocityFvPatchVectorField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchVectorField> clone() const
        {
            return tmp<fvPatchVectorField>
            (
                new groovyFlowRateInletVelocityFvPatchVectorField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        groovyFlowRateInletVelocityFvPatchVectorField
        (
            const groovyFlowRateInletVelocityFvPatchVectorField&,
            const DimensionedField<vector, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchVectorField> clone
        (
            const DimensionedField<vector, volMesh>& iF
        ) const
        {
            return tmp<fvPatchVectorField>
            (
                new groovyFlowRateInletVelocityFvPatchVectorField(*this, iF)
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

#endif

// ************************************************************************* //
