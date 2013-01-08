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
    calculate the offsets-entry for (direct)Mapped patches

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

using namespace Foam;

namespace Foam {
// instantiatino to keep the linker happy
    defineTemplateTypeNameAndDebug(IOPtrList<entry>,0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

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
    {
        Info << "Reading boundary from " << boundaryDict.filePath() << endl;

        // this way it doesn't matter that the file is not of the right class
        IFstream inStream(boundaryDict.filePath());
        boundaryDict.readHeader(inStream);
        boundaryDict.readData(inStream);
    }

    Info << endl;

    bool changed=false;

    forAll(boundaryDict,patchI) {
        const word &name=boundaryDict[patchI].keyword();
        dictionary &dict=boundaryDict[patchI].dict();

    }

    if(changed) {
        Info << endl << "Writing " << boundaryDict.filePath() << endl;

        // this way the class is not overwritten
        word actualClass=boundaryDict.headerClassName();

        OStringStream headerStream;
        boundaryDict.writeHeader(headerStream);
        string newHeader=headerStream.str().replace("dictionary",actualClass);

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
