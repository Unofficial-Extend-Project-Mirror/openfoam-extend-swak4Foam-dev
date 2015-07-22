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
    This file is part of OpenFOAM.

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
    Foam::fv::executeFunctionObjectsFvOption

Description
    This fvOption does not contribute to the equation but executes a list
    of function objects

    One application is getting output in inner iterations (if the equations
    use fvOptions)

SourceFiles
    executeFunctionObjectsFvOption.C

Contributors/Copyright:
    2014-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef executeFunctionObjectsFvOption_H
#define executeFunctionObjectsFvOption_H

#include "autoPtr.H"
#include "topoSetSource.H"
#include "cellSet.H"
#include "fvMesh.H"
#include "volFields.H"
#include "fvOption.H"
#include "functionObjectList.H"

#include "swakFvOption.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class executeFunctionObjectsFvOption Declaration
\*---------------------------------------------------------------------------*/

class executeFunctionObjectsFvOption
:
    public option
{
    // Private data

    functionObjectList functions_;

    bool verbose_;

    bool doCorrect_;

    bool doAddSup_;

    bool doSetValue_;

    bool doMakeRelative_;

    bool doMakeAbsolute_;

    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeFunctionObjectsFvOption(const executeFunctionObjectsFvOption&);

    //- Disallow default bitwise assignment
    void operator=(const executeFunctionObjectsFvOption&);

    void executeFunctionObjects(const string &message);

public:

    //- Runtime type information
    TypeName("executeFunctionObjectsFvOption");


    // Constructors

    //- Construct from explicit source name and mesh
    executeFunctionObjectsFvOption
    (
        const word& sourceName,
        const word& modelType,
        const dictionary& dict,
        const fvMesh& mesh
    );

    ~executeFunctionObjectsFvOption();

    // Member Functions

    // Evaluate

    // Correct

    //- Scalar
    virtual void correct(volScalarField& fld);

    //- Vector
    virtual void correct(volVectorField& fld);

    //- Spherical tensor
    virtual void correct(volSphericalTensorField& fld);

    //- Symmetric tensor
    virtual void correct(volSymmTensorField& fld);

    //- Tensor
    virtual void correct(volTensorField& fld);

    // Add explicit and implicit contributions

    //- Scalar
    virtual void addSup
    (
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
    // Add explicit and implicit contributions to compressible equations

    //- Scalar
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        const volScalarField& rho,
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

    // Add explicit and implicit contributions to phase equations

    //- Scalar
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<tensor>& eqn,
        const label fieldI
    );
#endif

    // Set values directly

    //- Scalar
    virtual void setValue
    (
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void setValue
    (
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void setValue
    (
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void setValue
    (
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void setValue
    (
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

    // Flux manipulations

    //- Make the given absolute flux relative
    virtual void makeRelative(surfaceScalarField& phi) const;

    //- Make the given absolute boundary flux relative
    virtual void makeRelative
    (
        FieldField<fvsPatchField, scalar>& phi
    ) const;

    //- Make the given absolute mass-flux relative
    virtual void makeRelative
    (
        const surfaceScalarField& rho,
        surfaceScalarField& phi
    ) const;

    //- Make the given relative flux absolute
    virtual void makeAbsolute(surfaceScalarField& phi) const;

    //- Make the given relative mass-flux absolute
    virtual void makeAbsolute
    (
        const surfaceScalarField& rho,
        surfaceScalarField& phi
    ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
