/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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
    2008 Hannes Kroeger (hannes@kroegeronline.net)
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "mtv.H"
#include "fileName.H"
#include "OFstream.H"
#include "faceList.H"
#include "OSspecific.H"
#include "triSurface.H"
#include "transform.H"
#include "transformField.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::mtv<Type>::mtv()
:
    surfaceWriter<Type>()
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
Foam::mtv<Type>::~mtv()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::mtv<Type>::write
(
    const fileName& samplePath,
    const fileName& timeDir,
    const fileName& surfaceName,
    const pointField& points,
    const faceList& faces,
    const fileName& fieldName,
    const Field<Type>& values,
    const bool verbose
) const
{
    if (values.size()==faces.size())
    {
        FatalErrorIn("mtv::write()")
            << "Only interpolated planes are currently supported."
                << abort(FatalError);
    }


    fileName surfaceDir(samplePath/timeDir);

    if (!exists(surfaceDir))
    {
        mkDir(surfaceDir);
    }


    // Convert faces to triangles.
    DynamicList<labelledTri> tris(faces.size());

    forAll(faces, i)
    {
        const face& f = faces[i];

        faceList triFaces(f.nTriangles(points));
        label nTris = 0;
        f.triangles(points, nTris, triFaces);

        forAll(triFaces, triI)
        {
            const face& tri = triFaces[triI];
            tris.append(labelledTri(tri[0], tri[1], tri[2], 0));
        }
    }

    triSurface tS
    (
        tris.shrink(),
        geometricSurfacePatchList
        (
            1,
            geometricSurfacePatch
            (
                "patch",                            // geometricType
                string::validate<word>(fieldName),  // fieldName
                0                                   // index
            )
        ),
        points
    );

    // get normal vector
    vector n=average(tS.faceNormals());

    if (mag(n)<SMALL)
    {
        FatalErrorIn("mtv::write()")
            << "Could not determine plane normal direction."
                << "Maybe the surface was not flat."
                << "Only flat surfaces are currently supported."
                << abort(FatalError);
    }

    n/=mag(n);

    // transform face centres to local frame
    vector rotbase=average(tS.localPoints());
    tensor rot=rotationTensor(n, vector(0,0,1));
    //tensor rotback=inv(rot);
    //Field<vector> tloc=transform(rot, patch().Cf()-rotbase);

    for (label cmpt=0; cmpt<pTraits<Type>::nComponents; cmpt++)
    {
        fileName planeFName;

        if (pTraits<Type>::nComponents==1)
        {
                planeFName=surfaceDir/fieldName + '_' +
                surfaceName + ".mtv";
        }
        else
        {
                planeFName=surfaceDir/fieldName + pTraits<Type>::componentNames[cmpt]
                + '_' + surfaceName + ".mtv";
        }

        if(verbose) {
            Info<< "Writing field " << fieldName << " to " << planeFName << endl;
        }

        OFstream f(planeFName);
        f<<"$ DATA=CONTCURVE Name="<<surfaceName<<nl;
        f<<"% toplabel=\""<<string::validate<word>(fieldName);
        if (pTraits<Type>::nComponents>1)
        {
          f<<string::validate<word>(pTraits<Type>::componentNames[cmpt]);
        }
        f<<" ("<<string::validate<word>(surfaceName)<<")\""<<nl;
        f<<"% contfill"<<nl;
        f<<"% contstyle=2 nsteps=50 linetypes=3 interp=3"<<nl;

        const List<labelledTri>& fc=tS.localFaces();

        forAll(fc, fI)
        {
            forAll(fc[fI], pI)
            {
                label pii=fc[fI][pI];
                vector ploc=transform
                    (
                        rot,
                        tS.localPoints()[pii]-rotbase
                    );
                f<<ploc.x()<<" "<<ploc.y()<<" "<<component(values[pii], cmpt)<<" "<<pii<<nl;
            }
            f<<nl;
        }

    }

}


// ************************************************************************* //
