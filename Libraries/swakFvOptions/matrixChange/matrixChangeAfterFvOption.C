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

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "matrixChangeAfterFvOption.H"
#include "fvMatrices.H"
#include "DimensionedField.H"
#include "IFstream.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(matrixChangeAfterFvOption, 0);

    addToRunTimeSelectionTable
    (
        option,
        matrixChangeAfterFvOption,
        dictionary
    );
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::matrixChangeAfterFvOption::matrixChangeAfterFvOption
(
    const word& sourceName,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    matrixChangeBaseFvOption(sourceName, modelType, dict, mesh)
{
}

Foam::fv::matrixChangeAfterFvOption::~matrixChangeAfterFvOption()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


template<class T>
void Foam::fv::matrixChangeAfterFvOption::subtractResidual(
    const fvMatrix<T>& matrix
) {
    Info << this->name() << " subtracting from residual for "
        << matrix.psi().name() << " to " << this->fieldName() << endl;

    typedef GeometricField<T,fvPatchField,volMesh> fType;

    fType &theRes=const_cast<fType&>(
        matrix.psi().mesh().template lookupObject<fType>(
            this->fieldName()
        )
    );
    theRes=this->calcResiduum(matrix)-theRes;
}

void Foam::fv::matrixChangeAfterFvOption::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}
#endif


void Foam::fv::matrixChangeAfterFvOption::addSup
(
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}
#endif


void Foam::fv::matrixChangeAfterFvOption::addSup
(
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}
#endif


void Foam::fv::matrixChangeAfterFvOption::addSup
(
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}
#endif


void Foam::fv::matrixChangeAfterFvOption::addSup
(
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}

void Foam::fv::matrixChangeAfterFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doAtAddSup()) {
        subtractResidual(eqn);
    }
}
#endif

void Foam::fv::matrixChangeAfterFvOption::setValue
(
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(!doAtAddSup()) {
        subtractResidual(eqn);
    }
}



void Foam::fv::matrixChangeAfterFvOption::setValue
(
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(!doAtAddSup()) {
        subtractResidual(eqn);
    }
}


void Foam::fv::matrixChangeAfterFvOption::setValue
(
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(!doAtAddSup()) {
        subtractResidual(eqn);
    }
}


void Foam::fv::matrixChangeAfterFvOption::setValue
(
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(!doAtAddSup()) {
        subtractResidual(eqn);
    }
}


void Foam::fv::matrixChangeAfterFvOption::setValue
(
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(!doAtAddSup()) {
        subtractResidual(eqn);
    }
}


// ************************************************************************* //
