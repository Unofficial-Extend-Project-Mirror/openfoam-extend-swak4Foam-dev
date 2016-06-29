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
    2014-2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "reportMatrixFvOption.H"
#include "fvMatrices.H"
#include "DimensionedField.H"
#include "IFstream.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(reportMatrixFvOption, 0);

    addToRunTimeSelectionTable
    (
        option,
        reportMatrixFvOption,
        dictionary
    );
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::reportMatrixFvOption::reportMatrixFvOption
(
    const word& sourceName,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    option(sourceName, modelType, dict, mesh),
    doAddSup_(readBool(coeffs_.lookup("doAddSup"))),
    doSetValue_(readBool(coeffs_.lookup("doSetValue")))
{
    coeffs_.lookup("fieldNames") >> fieldNames_;
    applied_.setSize(fieldNames_.size(), false);
}

Foam::fv::reportMatrixFvOption::~reportMatrixFvOption()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::fv::reportMatrixFvOption::reportMatrix(
    const string &message,
    const fvMatrix<Type> &matrix
) {
    Info << name() << " Reporting: " << message.c_str()
        << " - Matrix for " << matrix.psi().name() << endl;

    if(!matrix.hasDiag()) {
        Info << "  Has no diagonal. Nothing to report" << endl;
        return;
    }
    const scalarField &diag=matrix.diag();
    scalarField diagMag(mag(diag));

    Info << "  Diagonal: [ " << min(diag) << " , " << max(diag)
        << " ] Average: " << average(diag) << " average(mag) "
        << average(diagMag) << endl;

    if(matrix.hasUpper() && matrix.hasLower()) {
        scalarField offDiag(diag.size(),0);
        matrix.sumMagOffDiag(offDiag);

        Info << "  sum(mag(OffDiagonal)): [ " << min(offDiag)
            << " , " << max(offDiag) << " ] Average: "
            << average(offDiag) << endl;

        scalarField diagDom(offDiag/stabilise(diagMag,1e-15));

        Info << "  Diagonal dominance: [ " << min(diagDom) << " , "
            << max(diagDom) << " ] Average: " << average(diagDom)
            << " average(mag) " << average(mag(diagDom)) << endl;

        Field<Type> res(matrix.residual());

        Info << "  Residual: [ " << min(res) << " , " << max(res)
            << " ] Average: " << average(res) << " average(mag) "
            << average(mag(res)) << endl;
    }

    Field<Type> DD(matrix.DD());

    Info << "  DD: [ " << min(DD) << " , " << max(DD)
        << " ] Average: " << average(DD) << " average(mag) "
        << average(mag(DD)) << endl;

    scalarField A(matrix.A());

    Info << "  A: [ " << min(A) << " , " << max(A)
        << " ] Average: " << average(A) << " average(mag) "
        << average(mag(A)) << endl;

    scalarField H1(matrix.H1());

    Info << "  H1: [ " << min(H1) << " , " << max(H1)
        << " ] Average: " << average(H1) << " average(mag) "
        << average(mag(H1)) << endl;

    const Field<Type> &src=matrix.source();

    Info << "  Source: [ " << min(src) << " , " << max(src)
        << " ] Average: " << average(src) << " average(mag) "
        << average(mag(src)) << endl;
}

void Foam::fv::reportMatrixFvOption::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(fvMatrix<vector>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+rho.name()+
            ",fvMatrix<vector>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+alpha.name()+
            ",const volScalarField &"+rho.name()+
            ",fvMatrix<vector>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}
#endif


void Foam::fv::reportMatrixFvOption::setValue
(
    fvMatrix<vector>& eqn,
    const label fieldI
)
{
    if(doSetValue_) {
        reportMatrix(
            "setValue(fvMatrix<vector>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(fvMatrix<scalar>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+rho.name()+
            ",fvMatrix<scalar>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+alpha.name()+
            ",const volScalarField &"+rho.name()+
            ",fvMatrix<scalar>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}
#endif


void Foam::fv::reportMatrixFvOption::setValue
(
    fvMatrix<scalar>& eqn,
    const label fieldI
)
{
    if(doSetValue_) {
        reportMatrix(
            "setValue(fvMatrix<scalar>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(fvMatrix<tensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+rho.name()+
            ",fvMatrix<tensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+alpha.name()+
            ",const volScalarField &"+rho.name()+
            ",fvMatrix<tensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}
#endif


void Foam::fv::reportMatrixFvOption::setValue
(
    fvMatrix<tensor>& eqn,
    const label fieldI
)
{
    if(doSetValue_) {
        reportMatrix(
            "setValue(fvMatrix<tensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(fvMatrix<symmTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+rho.name()+
            ",fvMatrix<symmTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+alpha.name()+
            ",const volScalarField &"+rho.name()+
            ",fvMatrix<symmTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}
#endif


void Foam::fv::reportMatrixFvOption::setValue
(
    fvMatrix<symmTensor>& eqn,
    const label fieldI
)
{
    if(doSetValue_) {
        reportMatrix(
            "setValue(fvMatrix<symmTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(fvMatrix<sphericalTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& rho,
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+rho.name()+
            ",fvMatrix<sphericalTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}

void Foam::fv::reportMatrixFvOption::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doAddSup_) {
        reportMatrix(
            "addSup(const volScalarField &"+alpha.name()+
            ",const volScalarField &"+rho.name()+
            ",fvMatrix<sphericalTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}
#endif


void Foam::fv::reportMatrixFvOption::setValue
(
    fvMatrix<sphericalTensor>& eqn,
    const label fieldI
)
{
    if(doSetValue_) {
        reportMatrix(
            "setValue(fvMatrix<sphericalTensor>& "+eqn.psi().name()+"/"+fieldNames_[fieldI]+")",
            eqn
        );
    }
}


// ************************************************************************* //
