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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "argList.H"
#include "Time.H"
#include "polyMesh.H"
#include "OFstream.H"
#include "IOPtrList.H"
#include "IFstream.H"

#if FOAM_VERSION4SWAK_MAJOR<2
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
        const mappedPatchBase &mb=dynamicCast<const mappedPatchBase&>(
            thePatch
        );

        vector transposeFirst(0,0,0);
        vector scaleBefore(1,1,1);
        tensor rotation(sphericalTensor(1));
        vector scaleAfter(1,1,1);
        vector transposeAfter(0,0,0);

        const dictionary &para=spec.subDict(name);

        word mode(para.lookup("mode"));
        Info << "Doing mode " << mode << endl;

        if(mode=="specifyAll") {
               transposeFirst=vector(para.lookup("transposeFirst"));
            scaleBefore=vector(para.lookup("scaleBeforeRotation"));
            vector from(para.lookup("rotationFrom"));
            scaleAfter=vector(para.lookup("scaleAfterRotation"));
            vector to(para.lookup("rotationTo"));
            rotation=rotationTensor(from,to);
            transposeAfter=vector(para.lookup("transposeAfter"));
        } else {
            FatalErrorIn(args.executable())
                << "Currently only mode 'specifyAll' implemented. Not "
                    << mode
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

        if(dict.found("offset")) {
            // otherwise certain implementations will still assume uniform
            dict.remove("offset");
        }
        dict.set("offsets",moved-faceCentres);
        dict.set("offsetMode","nonuniform");

        dict.set("movedCentersForDebugging",moved);

        changed=true;

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
