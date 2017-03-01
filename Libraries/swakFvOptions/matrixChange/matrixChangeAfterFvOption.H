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
    Foam::fv::matrixChangeAfterFvOption

Description
    This fvOption looks for a field set by a matrixChangeBefore and subtracts
    the current residual value from it (giving the change)

SourceFiles
    matrixChangeAfterFvOption.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef matrixChangeAfterFvOption_H
#define matrixChangeAfterFvOption_H

#include "autoPtr.H"
#include "topoSetSource.H"
#include "cellSet.H"
#include "fvMesh.H"
#include "volFields.H"
#include "fvOption.H"
#include "functionObjectList.H"

#include "matrixChangeBaseFvOption.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class matrixChangeAfterFvOption Declaration
\*---------------------------------------------------------------------------*/

class matrixChangeAfterFvOption
:
    public matrixChangeBaseFvOption
{
    // Private data

    // Private Member Functions

    //- Disallow default bitwise copy construct
    matrixChangeAfterFvOption(const matrixChangeAfterFvOption&);

    //- Disallow default bitwise assignment
    void operator=(const matrixChangeAfterFvOption&);

    template<class T>
    void subtractResidual(
        const fvMatrix<T>&
    );

public:

    //- Runtime type information
    TypeName("matrixChangeAfter");


    // Constructors

    //- Construct from explicit source name and mesh
    matrixChangeAfterFvOption
    (
        const word& sourceName,
        const word& modelType,
        const dictionary& dict,
        const fvMesh& mesh
    );

    ~matrixChangeAfterFvOption();

    // Member Functions

    // Evaluate

    // Correct

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


    //- in newer versions setValue was renamed
    virtual void constrain(fvMatrix<scalar>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<vector>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<tensor>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<symmTensor>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
    virtual void constrain(fvMatrix<sphericalTensor>& eqn, const label fieldI) {
        setValue(eqn,fieldI);
    }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
