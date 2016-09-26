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
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"

#include "streamFunctionPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#ifdef FOAM_HAS_SYMMETRY_PLANE_POLY_PATCH
#include "symmetryPlanePolyPatch.H"
#endif

#include "symmetryPolyPatch.H"
#include "emptyPolyPatch.H"
#include "wedgePolyPatch.H"

#include "addToRunTimeSelectionTable.H"

#include "fvc.H"

#ifdef FOAM_PATCHFIELDTYPE_IN_GEOFIELD_IS_NOW_PATCH
#define PatchFieldType Patch
#define GeometricBoundaryField Boundary
#endif

namespace Foam {

defineTypeNameAndDebug(streamFunctionPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, streamFunctionPluginFunction , name, streamFunction);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

streamFunctionPluginFunction::streamFunctionPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word("pointScalarField"),
        string("phi internalField surfaceScalarField")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void streamFunctionPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0);

    phi_.set(
        new surfaceScalarField(
            //            dynamicCast<const FieldValueExpressionDriver &>(
            dynamic_cast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<surfaceScalarField>()
        )
    );
}

void streamFunctionPluginFunction::doEvaluation()
{
    label nD = mesh().nGeometricD();

    if (nD != 2)
    {
        FatalErrorIn("streamFunctionPluginFunction::doEvaluation()")
            << "Case is not 2D, stream-function cannot be computed"
                << exit(FatalError);
    }

    Vector<label> slabNormal((Vector<label>::one - mesh().geometricD())/2);
    const direction slabDir
        (
            slabNormal
            & Vector<label>(Vector<label>::X, Vector<label>::Y, Vector<label>::Z)
        );

    scalar thickness = vector(
			      slabNormal.x(),
			      slabNormal.y(),
			      slabNormal.z()
		       ) & mesh().bounds().span();

    const pointMesh& pMesh = pointMesh::New(mesh());

    autoPtr<pointScalarField> pStreamFunction(
        new pointScalarField(
            IOobject(
                "streamFunctionField",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            pMesh,
            dimensionedScalar("nonOr",dimless,0)
        )
    );
    pointScalarField &streamFunction=pStreamFunction();

    const surfaceScalarField &phi=phi_();

    // from the streamFunction-utility
    labelList visitedPoint(mesh().nPoints());
    forAll(visitedPoint, pointI)
    {
        visitedPoint[pointI] = 0;
    }
    label nVisited = 0;
    label nVisitedOld = 0;

    const UList<face>& faces = mesh().faces();
    const pointField& points = mesh().points();

    label nInternalFaces = mesh().nInternalFaces();

    vectorField unitAreas(mesh().faceAreas());
    unitAreas /= mag(unitAreas);

    const polyPatchList& patches = mesh().boundaryMesh();

    bool finished = true;

    // Find the boundary face with zero flux. set the stream function
    // to zero on that face
    bool found = false;

    do
    {
        found = false;

        forAll(patches, patchI)
        {
            const primitivePatch& bouFaces = patches[patchI];

            if (!isType<emptyPolyPatch>(patches[patchI]))
            {
                forAll(bouFaces, faceI)
                {
                    if
                        (
                            magSqr(phi.boundaryField()[patchI][faceI])
                            < SMALL
                        )
                    {
                        const labelList& zeroPoints = bouFaces[faceI];

                        // Zero flux face found
                        found = true;
                        forAll(zeroPoints, pointI)
                        {
                            if (visitedPoint[zeroPoints[pointI]] == 1)
                            {
                                found = false;
                                break;
                            }
                        }

                        if (found)
                        {
                            // Info<< "Zero face: patch: " << patchI
                            //     << "    face: " << faceI << endl;

                            forAll(zeroPoints, pointI)
                            {
                                streamFunction[zeroPoints[pointI]] = 0;
                                visitedPoint[zeroPoints[pointI]] = 1;
                                nVisited++;
                            }

                            break;
                        }
                    }
                }
            }

            if (found) break;
        }

        if (!found)
        {
            // Info<< "zero flux boundary face not found. "
            //     << "Using cell as a reference."
            //     << endl;

            const cellList& c = mesh().cells();

            forAll(c, cI)
            {
                labelList zeroPoints = c[cI].labels(mesh().faces());

                bool found = true;

                forAll(zeroPoints, pointI)
                {
                    if (visitedPoint[zeroPoints[pointI]] == 1)
                    {
                        found = false;
                        break;
                    }
                }

                if (found)
                {
                    forAll(zeroPoints, pointI)
                    {
                        streamFunction[zeroPoints[pointI]] = 0.0;
                        visitedPoint[zeroPoints[pointI]] = 1;
                        nVisited++;
                    }

                    break;
                }
                else
                {
 		    FatalErrorIn("streamFunctionPluginFunction::doEvaluation()")
                        << "Cannot find initialisation face or a cell."
                            << abort(FatalError);
                }
            }
        }

        // Loop through all faces. If one of the points on
        // the face has the streamfunction value different
        // from -1, all points with -1 ont that face have the
        // streamfunction value equal to the face flux in
        // that point plus the value in the visited point
        do
        {
            finished = true;

            for
                (
                    label faceI = nInternalFaces;
                    faceI<faces.size();
                    faceI++
                )
            {
                const labelList& curBPoints = faces[faceI];
                bool bPointFound = false;

                scalar currentBStream = 0.0;
                vector currentBStreamPoint(0, 0, 0);

                forAll(curBPoints, pointI)
                {
                    // Check if the point has been visited
                    if (visitedPoint[curBPoints[pointI]] == 1)
                    {
                        // The point has been visited
                        currentBStream =
                            streamFunction[curBPoints[pointI]];
                        currentBStreamPoint =
                            points[curBPoints[pointI]];

                        bPointFound = true;

                        break;
                    }
                }

                if (bPointFound)
                {
                    // Sort out other points on the face
                    forAll(curBPoints, pointI)
                    {
                        // Check if the point has been visited
                        if (visitedPoint[curBPoints[pointI]] == 0)
                        {
                            label patchNo =
                                mesh().boundaryMesh().whichPatch(faceI);

                            if
                                (
                                    !isType<emptyPolyPatch>
                                    (patches[patchNo])
#ifdef FOAM_HAS_SYMMETRY_PLANE_POLY_PATCH
                                    && !isType<symmetryPlanePolyPatch>
                                    (patches[patchNo])
#endif
                                    && !isType<symmetryPolyPatch>
                                    (patches[patchNo])
                                    && !isType<wedgePolyPatch>
                                    (patches[patchNo])
                                )
                            {
                                label faceNo =
                                    mesh().boundaryMesh()[patchNo]
                                    .whichFace(faceI);

                                vector edgeHat =
                                    points[curBPoints[pointI]]
                                    - currentBStreamPoint;
                                edgeHat.replace(slabDir, 0);
                                edgeHat /= mag(edgeHat);

                                vector nHat = unitAreas[faceI];

                                if (edgeHat.y() > VSMALL)
                                {
                                    visitedPoint[curBPoints[pointI]] =
                                        1;
                                    nVisited++;

                                    streamFunction[curBPoints[pointI]]
                                        =
                                        currentBStream
                                        + phi.boundaryField()
                                        [patchNo][faceNo]
                                        *sign(nHat.x());
                                }
                                else if (edgeHat.y() < -VSMALL)
                                {
                                    visitedPoint[curBPoints[pointI]] =
                                        1;
                                    nVisited++;

                                    streamFunction[curBPoints[pointI]]
                                        =
                                        currentBStream
                                        - phi.boundaryField()
                                        [patchNo][faceNo]
                                        *sign(nHat.x());
                                }
                                else
                                {
                                    if (edgeHat.x() > VSMALL)
                                    {
                                        visitedPoint
                                            [curBPoints[pointI]] = 1;
                                        nVisited++;

                                        streamFunction
                                            [curBPoints[pointI]] =
                                            currentBStream
                                            + phi.boundaryField()
                                            [patchNo][faceNo]
                                            *sign(nHat.y());
                                    }
                                    else if (edgeHat.x() < -VSMALL)
                                    {
                                        visitedPoint
                                            [curBPoints[pointI]] = 1;
                                        nVisited++;

                                        streamFunction
                                            [curBPoints[pointI]] =
                                            currentBStream
                                            - phi.boundaryField()
                                            [patchNo][faceNo]
                                            *sign(nHat.y());
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    finished = false;
                }
            }

            for (label faceI=0; faceI<nInternalFaces; faceI++)
            {
                // Get the list of point labels for the face
                const labelList& curPoints = faces[faceI];

                bool pointFound = false;

                scalar currentStream = 0.0;
                point currentStreamPoint(0, 0, 0);

                forAll(curPoints, pointI)
                {
                    // Check if the point has been visited
                    if (visitedPoint[curPoints[pointI]] == 1)
                    {
                        // The point has been visited
                        currentStream =
                            streamFunction[curPoints[pointI]];
                        currentStreamPoint =
                            points[curPoints[pointI]];
                        pointFound = true;

                        break;
                    }
                }

                if (pointFound)
                {
                    // Sort out other points on the face
                    forAll(curPoints, pointI)
                    {
                        // Check if the point has been visited
                        if (visitedPoint[curPoints[pointI]] == 0)
                        {
                            vector edgeHat =
                                points[curPoints[pointI]]
                                - currentStreamPoint;

                            edgeHat.replace(slabDir, 0);
                            edgeHat /= mag(edgeHat);

                            vector nHat = unitAreas[faceI];

                            if (edgeHat.y() > VSMALL)
                            {
                                visitedPoint[curPoints[pointI]] = 1;
                                nVisited++;

                                streamFunction[curPoints[pointI]] =
                                    currentStream
                                    + phi[faceI]*sign(nHat.x());
                            }
                            else if (edgeHat.y() < -VSMALL)
                            {
                                visitedPoint[curPoints[pointI]] = 1;
                                nVisited++;

                                streamFunction[curPoints[pointI]] =
                                    currentStream
                                    - phi[faceI]*sign(nHat.x());
                            }
                        }
                    }
                }
                else
                {
                    finished = false;
                }
            }

            // Info<< ".";

            if (nVisited == nVisitedOld)
            {
                // Find new seed.  This must be a
                // multiply connected domain
                // Info<< nl << "Exhausted a seed. Looking for new seed "
                //     << "(this is correct for multiply connected "
                //     << "domains).";

                break;
            }
            else
            {
                nVisitedOld = nVisited;
            }
        } while (!finished);

        //        Info<< endl;
    } while (!finished);

    // Normalise the stream-function by the 2D mesh thickness
    streamFunction /= thickness;
    const_cast<pointScalarField::GeometricBoundaryField&>(
        streamFunction.boundaryField()
    ) = 0.0;
    // end of 'borrowed' code

    result().setObjectResult(pStreamFunction);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
