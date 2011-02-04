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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::groovyBCPointPatchField

Description

SourceFiles
    groovyBCPointPatchField.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef groovyBCPointPatchField_H
#define groovyBCPointPatchField_H

#ifdef FOAM_DEV

#   include "MixedPointPatchField.H"
    // Predefine extra template paramters
#   define mixedPointPatchFieldType MixedPointPatchField<pointPatchField,pointMesh,pointPatch,DummyMatrix,Type>
#   define pointPatchFieldMapper PointPatchFieldMapper

#else

#    include "mixedPointPatchField.H"
#    define mixedPointPatchFieldType mixedPointPatchField<Type>

#endif

#include "PatchValueExpressionDriver.H"

#include "groovyBCCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class groovyBCPointPatchField Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyBCPointPatchField
:
    public mixedPointPatchFieldType,
    public groovyBCCommon<Type>
{

protected:

    // Protected data

    PatchValueExpressionDriver driver_;

    string nullValue();

public:

    //- Runtime type information
    TypeName("groovyBC");


    // Constructors

    //- Construct from patch and internal field
    groovyBCPointPatchField
    (
        const pointPatch&,
        const DimensionedField<Type, pointMesh>&
    );

    //- Construct from patch, internal field and dictionary
    groovyBCPointPatchField
    (
        const pointPatch&,
        const DimensionedField<Type, pointMesh>&,
        const dictionary&
    );

    //- Construct by mapping given patchField<Type> onto a new patch
    groovyBCPointPatchField
    (
        const groovyBCPointPatchField<Type>&,
        const pointPatch&,
        const DimensionedField<Type, pointMesh>&,
        const pointPatchFieldMapper&
    );

    //- Construct and return a clone
    virtual autoPtr<pointPatchField<Type> > clone() const
        {
            return autoPtr<pointPatchField<Type> >
                (
                    new groovyBCPointPatchField<Type>
                    (
                        *this
                    )
                );
        }

    //- Construct as copy setting internal field reference
    groovyBCPointPatchField
    (
        const groovyBCPointPatchField<Type>&,
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
                    new groovyBCPointPatchField<Type>
                    (
                        *this,
                        iF
                    )
                );
        }


    // Member functions

    // Evaluation functions

    //- Update the patch field
    virtual void updateCoeffs();


    //- Write
    virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#    include "groovyBCPointPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
