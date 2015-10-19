/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright (C) 1991-2005 OpenCFD Ltd.
    \\/      M anipulation   |
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

Application
    calculate the offsets-entry for (direct)Mapped patches. Does so by applying
    a number of coordinate transformations to the face-centers of the patch and
    calculating the difference of the coordinates

Description

Contributors/Copyright:
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "argList.H"
#include "swakTime.H"
#include "polyMesh.H"
#include "OFstream.H"
#include "IOPtrList.H"
#include "IFstream.H"
#include "ListListOps.H"
#include "meshTools.H"

#include "swak.H"

#ifdef FOAM_MAPPED_IS_DIRECTMAPPED
#include "directMappedPatchBase.H"

namespace Foam {
    // these typedefs should keep the difference between the 1.7 and the 2.1 code minimal
    typedef directMappedPatchBase mappedPatchBase;
}

#else
#include "mappedPatchBase.H"
#endif

#include "transform.H"

using namespace Foam;

namespace Foam {
// instantiatino to keep the linker happy
    defineTemplateTypeNameAndDebug(IOPtrList<entry>,0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

tmp<vectorField> transformPoints(
    const vectorField &original,
    const vector &transposeFirst,
    const vector &scalingBefore,
    const tensor &rotation,
    const vector &scalingAfter,
    const vector &transposeAfter
) {
    tmp<vectorField> result(
        new vectorField(original)
    );
    vectorField &tr=result();

    tr+=transposeFirst;
    forAll(tr,i) {
        tr[i]=cmptMultiply(tr[i],scalingBefore);
        tr[i]=transform(rotation,tr[i]);
        tr[i]=cmptMultiply(tr[i],scalingAfter);
    };

    tr+=transposeAfter;

    return result;
}

enum specificationMode {
    SPECIFYALL,
    AUTOTRANSPOSE
};
const NamedEnum<specificationMode, 2> specificationModeNames;
template<>
const char* Foam::NamedEnum<specificationMode,2>::names[] =
{
    "specifyAll",
    "autoTranspose"
};

int main(int argc, char *argv[])
{
#   include "addRegionOption.H"

#   include "setRootCase.H"
#   include "createTime.H"

#   include "createNamedMesh.H"

    IOdictionary calcDict
    (
        IOobject
        (
            "calcNonUniformOffsetsDict",
            mesh.time().system(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    wordList otherRegions(calcDict.lookup("additionalMeshes"));
    PtrList<polyMesh> additionalMeshes(otherRegions.size());

    Info << "Reading other meshes" << endl;

    forAll(otherRegions,i) {
        const word &name=otherRegions[i];
        Info << "Reading  region " << name << endl;
        if(name==regionName){
            FatalErrorIn(args.executable())
                << name << " is the default mesh"
                    << endl
                    << exit(FatalError);
        }

        additionalMeshes.set(
            i,
            new polyMesh(
                Foam::IOobject
                (
                    regionName,
                    runTime.timeName(),
                    runTime,
                    Foam::IOobject::MUST_READ
                )
            )
        );
    }

    dictionary spec(calcDict.subDict("offsetSpecifications"));

    fileName instanceName=runTime.findInstance(mesh.meshDir(), "boundary");
    IOPtrList<entry> boundaryDict(
        IOobject(
            instanceName/mesh.meshDir()/"boundary",
            runTime,
            Foam::IOobject::NO_READ,
            Foam::IOobject::NO_WRITE,
            false
        )
    );
    //    boundaryDict.checkOut();
    {
        IOobject::debug=1;

        Info << "Reading boundary from " << boundaryDict.filePath() << endl;

        // this way it doesn't matter that the file is not of the right class
        IFstream inStream(boundaryDict.filePath());
        boundaryDict.readHeader(inStream);

        // Necessary because IOPtrList does not implement readHeader
        PtrList<entry> data(
            inStream
        );
        boundaryDict.PtrList<entry>::operator=(data);
   }

    Info << endl;

    bool changed=false;

    forAll(boundaryDict,patchI) {
        const word &name=boundaryDict[patchI].keyword();
        dictionary &dict=boundaryDict[patchI].dict();

        Info << "Treating patch " << name << endl;
        if(!spec.found(name)) {
            Info << "Nothing specified. Continuing" << endl << endl;
            continue;
        }

        label patchID=mesh.boundaryMesh().findPatchID(name);
        if(patchID<0) {
            FatalErrorIn(args.executable())
                << "Patch " << name << " does not exist"
                    << endl
                    << exit(FatalError);
        }
        const polyPatch &thePatch=mesh.boundaryMesh()[patchID];
        if(!isA<const mappedPatchBase>(thePatch)) {
            WarningIn(args.executable())
                << name << " is not a subclass of " << mappedPatchBase::typeName
                    << endl << endl;
            continue;
        }
        //        const mappedPatchBase &mb=dynamicCast<const mappedPatchBase&>(
        const mappedPatchBase &mb=dynamic_cast<const mappedPatchBase&>(
            thePatch
        );
        autoPtr<pointField> otherMeshPoints;
        autoPtr<pointField> allOtherPoints;
        if(
            mb.mode()==mappedPatchBase::NEARESTPATCHFACE
#ifdef FOAM_HAS_ABI_PATCHES
            ||
            mb.mode()==mappedPatchBase::NEARESTPATCHFACEAMI
#endif
        ) {
            Info << "Getting points from other patch " << mb.samplePatch()
                << " on mesh " << mb.sampleRegion() << endl;

            const polyPatch &otherPatch=mb.samplePolyPatch();
            otherMeshPoints.set(
                new pointField(otherPatch.localPoints())
            );
            List<pointField> globalOtherPoints(Pstream::nProcs());
            globalOtherPoints[Pstream::myProcNo()]=otherMeshPoints;
            allOtherPoints.set(
                new pointField(
                    ListListOps::combine<pointField>(
                        globalOtherPoints,
                        accessOp<pointField>()
                    )
                )
            );
        }

        vectorField facePoints(thePatch.localPoints());
        List<pointField> globalFacePoints(Pstream::nProcs());
        globalFacePoints[Pstream::myProcNo()]=facePoints;
        pointField allFacePoints=ListListOps::combine<pointField>(
            globalFacePoints,
            accessOp<pointField>()
        );

        vector transposeFirst(0,0,0);
        vector scaleBefore(1,1,1);
        tensor rotation(sphericalTensor(1));
        vector scaleAfter(1,1,1);
        vector transposeAfter(0,0,0);

        const dictionary &para=spec.subDict(name);

        word mode(para.lookup("mode"));
        Info << "Doing mode " << mode << endl;
        specificationMode theMode=specificationModeNames[mode];

        switch(theMode) {
            case SPECIFYALL:
                {
                    transposeFirst=vector(para.lookup("transposeFirst"));
                    scaleBefore=vector(para.lookup("scaleBeforeRotation"));
                    vector from(para.lookup("rotationFrom"));
                    scaleAfter=vector(para.lookup("scaleAfterRotation"));
                    vector to(para.lookup("rotationTo"));
                    rotation=rotationTensor(from/mag(from),to/mag(to));
                    transposeAfter=vector(para.lookup("transposeAfter"));
                }
                break;
            case AUTOTRANSPOSE:
                {
                    if(!allOtherPoints.valid()){
                        FatalErrorIn(args.executable())
                            << mode << " only working for modes that map from a patch"
                                << endl
                                << exit(FatalError);

                    }
                    boundBox from(allFacePoints);
                    boundBox to(allOtherPoints());
                    transposeFirst=to.midpoint()-from.midpoint();
                    Info << "Transposing by " << transposeFirst << endl;
                }
                break;
            default:
                FatalErrorIn(args.executable())
                    << "Currently mode " << mode << " is not implemented"
                        << endl
                        << exit(FatalError);
        }

        Info << "Transforming face centers" << endl;

        vectorField faceCentres(thePatch.faceCentres());

        vectorField moved(
            transformPoints(
                faceCentres,
                transposeFirst,scaleBefore,rotation,scaleAfter,transposeAfter
            )
        );
        vectorField movedPoints(
            transformPoints(
                facePoints,
                transposeFirst,scaleBefore,rotation,scaleAfter,transposeAfter
            )
        );

        if(dict.found("offset")) {
            // otherwise certain implementations will still assume uniform
            dict.remove("offset");
        }
        dict.set("offsets",moved-faceCentres);
        dict.set("offsetMode","nonuniform");

        dict.set("movedCentersForDebugging",moved);

        changed=true;

        List<pointField> globalFaceCenters(Pstream::nProcs());
        List<pointField> globalMoved(Pstream::nProcs());
        List<pointField> globalMovedPoints(Pstream::nProcs());
        globalFaceCenters[Pstream::myProcNo()]=faceCentres;
        globalMoved[Pstream::myProcNo()]=moved;
        globalMovedPoints[Pstream::myProcNo()]=movedPoints;
        pointField allFaceCentres=ListListOps::combine<pointField>(
            globalFaceCenters,
            accessOp<pointField>()
        );
        pointField allMoved=ListListOps::combine<pointField>(
            globalMoved,
            accessOp<pointField>()
        );
        pointField allMovedPoints=ListListOps::combine<pointField>(
            globalMovedPoints,
            accessOp<pointField>()
        );

        Info << "Mapping box " << boundBox(allFacePoints) << " to "
            << boundBox(allMovedPoints) << endl;

        if(Pstream::master()) {
            OFstream str
                (
                    mesh.time().path()
                    / name + "_" + mesh.name()
                    + "_nonUniformMapping.obj"
                );
            Pout<< "Dumping mapping as lines from patch centres to"
                << " calculated location to file " << str.name() << endl;

            label vertI = 0;

            forAll(allFaceCentres, i)
            {
                meshTools::writeOBJ(str, allFaceCentres[i]);
                vertI++;
                meshTools::writeOBJ(str, allMoved[i]);
                vertI++;
                str << "l " << vertI-1 << ' ' << vertI << nl;
            }
        }

        if(
            otherMeshPoints.valid()
        ) {
            Info << "Checking whether mapped patches line up  " << flush;
            boundBox moved(allMovedPoints);
            boundBox mappedTo(allOtherPoints());

            const scalar thres=1e-5*mag(moved.span());
            if(
                mag(moved.min()-mappedTo.min())>thres
                ||
                mag(moved.max()-mappedTo.max())>thres
            ) {
                Info << "not OK" << endl;
                WarningIn(args.executable())
                    << "Boxes of moved points " << moved << " and mapped from "
                        << mappedTo << " differ by more than threshold "
                        << thres
                        << endl;
            } else {
                Info << "OK" << endl;
            }
        } else if(
            mb.mode()==mappedPatchBase::NEARESTCELL
            ||
            mb.mode()==mappedPatchBase::NEARESTFACE
        ) {
            Info << "Checking whether all transformed points are in region "
                << mb.sampleRegion() << " " << flush;

            const polyMesh &otherMesh=mb.sampleMesh();
            label pointsOutside=0;

            forAll(allMoved,i) {
                label cellI=otherMesh.findCell(allMoved[i]);
                reduce(cellI,maxOp<label>());
                if(cellI<0) {
                    pointsOutside++;
                }
            }

            if(pointsOutside>0) {
                Info << " not OK" << endl;
                WarningIn(args.executable())
                    << pointsOutside << " of " << allMoved.size()
                        << " points outside of region " << otherMesh.name()
                        << endl;
            } else {
                Info << " OK" << endl;
            }
        }

        Info << endl;
    }

    if(changed) {
        Info << endl << "Writing " << boundaryDict.filePath() << endl;

        // this way the class is not overwritten
        word actualClass=boundaryDict.headerClassName();

        OStringStream headerStream;
        boundaryDict.writeHeader(headerStream);
        string newHeader=headerStream.str().replace("IOPtrList<entry>",actualClass);

        OFstream outStream(boundaryDict.filePath());
        outStream << newHeader.c_str();
        boundaryDict.writeData(outStream);
    } else {
        Info << "Nothing changed .. nothing written" << endl;
    }

    Info << "End\n" << endl;

    return 0;
}

// ************************************************************************* //
