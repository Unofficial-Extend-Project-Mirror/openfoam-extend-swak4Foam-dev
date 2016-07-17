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

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "mqFaceNonOrthoPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "swak.H"

namespace Foam {

defineTypeNameAndDebug(mqFaceNonOrthoPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, mqFaceNonOrthoPluginFunction , name, mqFaceNonOrtho);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

mqFaceNonOrthoPluginFunction::mqFaceNonOrthoPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("surfaceScalarField"),
        string("")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void mqFaceNonOrthoPluginFunction::doEvaluation()
{
    autoPtr<surfaceScalarField> pNonOrto(
        new surfaceScalarField(
            IOobject(
                "faceNonOrthogonality",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("nonOr",dimless,0),
            "fixedValue"
        )
    );

    surfaceScalarField &nonOrto=pNonOrto();

    const vectorField& centres = mesh().cellCentres();
    const vectorField& areas = mesh().faceAreas();

    const labelList& own = mesh().faceOwner();
    const labelList& nei = mesh().faceNeighbour();

    forAll(nonOrto,faceI)
    {
        vector d = centres[nei[faceI]] - centres[own[faceI]];
        const vector& s = areas[faceI];

        scalar dDotS = max(
            -1,
            min(
                1,
                (d & s)/(mag(d)*mag(s) + VSMALL)
            )
        );

        // Info << d << s << dDotS << endl;
        // Info << ::acos(dDotS) << endl;

        nonOrto[faceI]=::acos(dDotS)/
#ifdef FOAM_NO_SEPARATE_CONSTANT_NAMESPACE
            Foam::mathematicalConstant::pi
#else
            constant::mathematical::pi
#endif
            *180.0;
    }

    result().setObjectResult(pNonOrto);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
