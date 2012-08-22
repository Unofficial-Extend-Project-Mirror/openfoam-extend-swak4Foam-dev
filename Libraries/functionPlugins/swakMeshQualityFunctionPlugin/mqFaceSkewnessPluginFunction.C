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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "mqFaceSkewnessPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(mqFaceSkewnessPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, mqFaceSkewnessPluginFunction , name, mqFaceSkewness);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

mqFaceSkewnessPluginFunction::mqFaceSkewnessPluginFunction(
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

void mqFaceSkewnessPluginFunction::doEvaluation()
{
    autoPtr<surfaceScalarField> pSkewness(
        new surfaceScalarField(
            IOobject(
                "faceSkewness",
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

    surfaceScalarField &skewness=pSkewness();

    const pointField& p = mesh().points();
    const faceList& fcs = mesh().faces();

    const labelList& own = mesh().faceOwner();
    const labelList& nei = mesh().faceNeighbour();
    const vectorField& cellCtrs = mesh().cellCentres();
    const vectorField& faceCtrs = mesh().faceCentres();
    const vectorField& fAreas = mesh().faceAreas();

    forAll(nei, faceI)
    {
        vector Cpf = faceCtrs[faceI] - cellCtrs[own[faceI]];
        vector d = cellCtrs[nei[faceI]] - cellCtrs[own[faceI]];

        // Skewness vector
        vector sv =
            Cpf - ((fAreas[faceI] & Cpf)/((fAreas[faceI] & d) + SMALL))*d;
        vector svHat = sv/(mag(sv) + VSMALL);

        // Normalisation distance calculated as the approximate distance
        // from the face centre to the edge of the face in the direction of
        // the skewness
        scalar fd = 0.2*mag(d) + VSMALL;
        const face& f = fcs[faceI];
        forAll(f, pi)
        {
            fd = max(fd, mag(svHat & (p[f[pi]] - faceCtrs[faceI])));
        }

        // Normalised skewness
        skewness[faceI] = mag(sv)/fd;
    }


    // Boundary faces: consider them to have only skewness error.
    // (i.e. treat as if mirror cell on other side)

    forAll(skewness.boundaryField(),patchI)
    {
        fvsPatchField<scalar> &thePatch=skewness.boundaryField()[patchI];

        forAll(thePatch,i)
        {
            scalar faceI=i+thePatch.patch().patch().start();

            vector Cpf = faceCtrs[faceI] - cellCtrs[own[faceI]];

            vector normal = fAreas[faceI];
            normal /= mag(normal) + VSMALL;
            vector d = normal*(normal & Cpf);


            // Skewness vector
            vector sv = Cpf - ((fAreas[faceI]&Cpf)/((fAreas[faceI]&d)+VSMALL))*d;
            vector svHat = sv/(mag(sv) + VSMALL);

            // Normalisation distance calculated as the approximate distance
            // from the face centre to the edge of the face in the direction of
            // the skewness
            scalar fd = 0.4*mag(d) + VSMALL;
            const face& f = fcs[faceI];
            forAll(f, pi)
            {
                fd = max(fd, mag(svHat & (p[f[pi]] - faceCtrs[faceI])));
            }

            // Normalised skewness
            thePatch[i] = mag(sv)/fd;
        }
    }

    result().setObjectResult(pSkewness);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
