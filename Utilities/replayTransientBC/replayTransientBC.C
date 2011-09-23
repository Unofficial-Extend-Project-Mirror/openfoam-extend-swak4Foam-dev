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
    sampleCells

Description

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "argList.H"
#include "Time.H"
#include "polyMesh.H"
#include "OFstream.H"
#include "interpolationCellPoint.H"
#include "vectorList.H"
#include "LPtrList.H"

using namespace Foam;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
#   include "addRegionOption.H"
    argList::validOptions.insert("allowFunctionObjects","");

#   include "setRootCase.H"
#   include "createTime.H"

    if(!args.options().found("allowFunctionObjects")) {
        runTime.functionObjects().off();
    }

#   include "createNamedMesh.H"

    IOdictionary replayDict
    (
        IOobject
        (
            "replayTransientBCDict",
            mesh.time().system(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    const wordList fieldNames = replayDict.lookup("fields");

    SLPtrList<volScalarField> scalarFields;
    SLPtrList<volVectorField> vectorFields;

    forAll(fieldNames,fieldI) {
        word fName=fieldNames[fieldI];

        IOobject f
            (
                fName,
                runTime.timeName(),
                mesh,
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            );
        f.headerOk();
        word className=f.headerClassName();

        if(className=="volScalarField") {
            Info << "Reading scalar field " << fName << endl;
            scalarFields.append(
                new volScalarField
                (
                    IOobject
                    (
                        fName,
                        runTime.timeName(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::AUTO_WRITE
                    ),
                    mesh
                )
            );
        } else if(className=="volVectorField") {
            Info << "Reading vector field " << fName << endl;
            vectorFields.append(
                new volVectorField
                (
                    IOobject
                    (
                        fName,
                        runTime.timeName(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::AUTO_WRITE
                    ),
                    mesh
                )
            );
        } else {
            Info << "Field " << fName << " not found " << endl;
        }
    }

    for (runTime++; !runTime.end(); runTime++)
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        // force the boundary conditions to be updated
        forAllIter(SLPtrList<volScalarField>,scalarFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volVectorField>,vectorFields,it) {
            (*it).correctBoundaryConditions();
        }
        runTime.write();
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
