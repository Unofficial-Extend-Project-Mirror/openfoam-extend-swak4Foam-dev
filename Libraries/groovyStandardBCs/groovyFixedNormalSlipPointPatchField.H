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
    Foam::groovyFixedNormalSlipPointPatchField

Description
    Foam::groovyFixedNormalSlipPointPatchField

SourceFiles
    groovyFixedNormalSlipPointPatchField.C

Contributors/Copyright:
    2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyFixedNormalSlipPointPatchField_H
#define groovyFixedNormalSlipPointPatchField_H

#include "swak.H"

#ifdef FOAM_POINTPATCHFIELD_HAS_FIVE_TEMPLATE_PARAMETERS

#include "SlipPointPatchField.H"
#define SlipPointPatchFieldType SlipPointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,Type>
#define pointPatchFieldMapper PointPatchFieldMapper

#else
#include "slipPointPatchField.H"
#define SlipPointPatchFieldType slipPointPatchField<Type>
#endif

#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class groovyTotalPressurePointPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyFixedNormalSlipPointPatchField
:
    public SlipPointPatchFieldType
{
    // Private data

    exprString fixedValueExpression_;

    exprString normalExpression_;

    PatchValueExpressionDriver driver_;

public:

    //- Runtime type information
    TypeName("groovyFixedNormalSlip");


    // Constructors

        //- Construct from patch and internal field
        groovyFixedNormalSlipPointPatchField
        (
            const pointPatch&,
            const DimensionedField<Type, pointMesh>&
        );

        //- Construct from patch, internal field and dictionary
        groovyFixedNormalSlipPointPatchField
        (
            const pointPatch&,
            const DimensionedField<Type, pointMesh>&,
            const dictionary&
        );

        //- Construct by mapping given groovyFixedNormalSlipPointPatchField
        //  onto a new patch
        groovyFixedNormalSlipPointPatchField
        (
            const groovyFixedNormalSlipPointPatchField&,
            const pointPatch&,
            const DimensionedField<Type, pointMesh>&,
            const pointPatchFieldMapper&
        );

        //- Construct as copy
        groovyFixedNormalSlipPointPatchField
        (
            const groovyFixedNormalSlipPointPatchField&
        );

        //- Construct and return a clone
        virtual autoPtr<pointPatchField<Type> > clone() const
        {
            return autoPtr<pointPatchField<Type> >
            (
                new groovyFixedNormalSlipPointPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        groovyFixedNormalSlipPointPatchField
        (
            const groovyFixedNormalSlipPointPatchField&,
            const DimensionedField<Type, pointMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual autoPtr<pointPatchField<Type> > clone
        (
            const DimensionedField<Type, pointMesh>& iF
        ) const
        {
            return autoPtr<pointPatchField<Type> >
            (
                new groovyFixedNormalSlipPointPatchField<Type>(*this, iF)
            );
        }

        // Mapping functions

        // Evaluation functions

            //- Update the patch field
            virtual void evaluate
            (
                const Pstream::commsTypes commsType =
#ifdef FOAM_PSTREAM_COMMSTYPE_IS_ENUMCLASS
                Pstream::commsTypes::blocking
#else
                Pstream::blocking
#endif
            );

        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #ifdef NoRepository
    #   include "groovyFixedNormalSlipPointPatchField.C"
    #endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
