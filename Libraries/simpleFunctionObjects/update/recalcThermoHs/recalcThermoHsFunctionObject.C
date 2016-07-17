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

Contributors/Copyright:
    2008-2011, 2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "foamVersion4swak.H"

#if FOAM_VERSION4SWAK_MAJOR<2 || (FOAM_VERSION4SWAK_MAJOR==2 && FOAM_VERSION4SWAK_MINOR<2)

#include "recalcThermoHsFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "basicThermo.H"

#include "fixedEnthalpyFvPatchScalarField.H"
#include "gradientEnthalpyFvPatchScalarField.H"
#include "mixedEnthalpyFvPatchScalarField.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    defineTypeNameAndDebug(recalcThermoHsFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        recalcThermoHsFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

recalcThermoHsFunctionObject::recalcThermoHsFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    updateSimpleFunctionObject(name,t,dict)
{
}

void recalcThermoHsFunctionObject::recalc()
{
    basicThermo &thermo=const_cast<basicThermo&>(
        obr_.lookupObject<basicThermo>("thermophysicalProperties")
    );
    Info << "Recalculating sensible enthalpy hs" << endl;

    const volScalarField &T=thermo.T();
    volScalarField &hs=thermo.hs();

    labelList allCells(T.size());
    forAll(allCells,cellI) {
        allCells[cellI]=cellI;
    }
    hs.internalField()=thermo.hs(
        T.internalField(),
        allCells
    );
    forAll(hs.boundaryField(), patchi)
    {
        hs.boundaryField()[patchi] ==
            thermo.hs(
                T.boundaryField()[patchi],
                patchi
            );
    }

    // hBoundaryCorrection
    volScalarField::GeometricBoundaryField& hbf = hs.boundaryField();

    forAll(hbf, patchi)
    {
        if (isA<gradientEnthalpyFvPatchScalarField>(hbf[patchi]))
        {
            refCast<gradientEnthalpyFvPatchScalarField>(hbf[patchi]).gradient()
                = hbf[patchi].fvPatchField::snGrad();
        }
        else if (isA<mixedEnthalpyFvPatchScalarField>(hbf[patchi]))
        {
            refCast<mixedEnthalpyFvPatchScalarField>(hbf[patchi]).refGrad()
                = hbf[patchi].fvPatchField::snGrad();
        }
    }

}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

#endif

// ************************************************************************* //
