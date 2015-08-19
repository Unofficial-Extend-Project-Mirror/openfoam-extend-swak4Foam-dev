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
    replayTransientBC

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
#include "interpolationCellPoint.H"
#include "vectorList.H"
#include "LPtrList.H"

using namespace Foam;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

#include "loadFieldFunction.H"

int main(int argc, char *argv[])
{
    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"
    argList::validOptions.insert("allowFunctionObjects","");
    argList::validOptions.insert("addDummyPhi","");

#   include "setRootCase.H"
#   include "createTime.H"

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

    if(
        !args.options().found("allowFunctionObjects")
        &&
        !replayDict.lookupOrDefault<bool>("useFunctionObjects",false)
    ) {
        runTime.functionObjects().off();
    }

    autoPtr<surfaceScalarField> dummyPhi;

    if(
        args.options().found("addDummyPhi")
        ||
        replayDict.lookupOrDefault<bool>("addDummyPhi",false)
    ) {
        Info << "Adding a dummy phi to make inletOutlet happy" << endl;
        dummyPhi.set(
            new surfaceScalarField(
                IOobject
                (
                    "phi",
                    mesh.time().system(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh,
                dimensionedScalar("phi",dimless,0)
            )
        );
    }

    const wordList fieldNames = replayDict.lookup("fields");

    SLPtrList<volScalarField> scalarFields;
    SLPtrList<volVectorField> vectorFields;
    SLPtrList<volTensorField> tensorFields;
    SLPtrList<volSymmTensorField> symmTensorFields;
    SLPtrList<volSphericalTensorField> sphericalTensorFields;

    forAll(fieldNames,fieldI) {
        word fName=fieldNames[fieldI];

        if(
            !loadFieldFunction(mesh,fName,scalarFields)
            &&
            !loadFieldFunction(mesh,fName,vectorFields)
            &&
            !loadFieldFunction(mesh,fName,tensorFields)
            &&
            !loadFieldFunction(mesh,fName,symmTensorFields)
            &&
            !loadFieldFunction(mesh,fName,sphericalTensorFields)
        ) {
            Info << "Field " << fName << " not found " << endl;
        }
    }

    wordList preloadFieldNames;
    if(!replayDict.found("preloadFields")) {
        WarningIn(args.executable())
            << "No list 'preloadFields' defined. Boundary conditions that depend "
                << "on other fields will fail"
                << endl;

    } else {
        preloadFieldNames=wordList(replayDict.lookup("preloadFields"));
    }

    Foam::instantList timeDirs;

    if(
        args.options().found("time")
        ||
        args.options().found("latestTime")
    ) {
        timeDirs = Foam::timeSelector::select0(runTime, args);
    }

    if(timeDirs.size()>0) {
        Info << endl << "Jumping to " << timeDirs.size()
            << " time directories instead of looping" << endl;
    }

    label timeDirCnt=0;

    while(
        timeDirs.size()>0
        ||
        runTime.loop()
    )
    {
        if(timeDirs.size()>0) {
            if(timeDirCnt>=timeDirs.size()) {
                break; // unorthodox way to end the loop. Makes both modes work
            }
            runTime.setTime(timeDirs[timeDirCnt],timeDirCnt);
            timeDirCnt++;
        }
        Info<< "Time = " << runTime.timeName() << nl << endl;
        Info<< "deltaT = " <<  runTime.deltaT().value() << endl;

        SLPtrList<volScalarField> scalarFieldsPre;
        SLPtrList<volVectorField> vectorFieldsPre;
        SLPtrList<volTensorField> tensorFieldsPre;
        SLPtrList<volSymmTensorField> symmTensorFieldsPre;
        SLPtrList<volSphericalTensorField> sphericalTensorFieldsPre;

        if(preloadFieldNames.size()>0) {
            Info << "Preloading fields" << endl;

            forAll(preloadFieldNames,fieldI) {
                word fName=preloadFieldNames[fieldI];

                if(
                    !loadFieldFunction(mesh,fName,scalarFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,vectorFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,tensorFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,symmTensorFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,sphericalTensorFieldsPre)
                ) {
                    Info << "Field " << fName << " not found " << endl;
                }
            }
        }

        // force the boundary conditions to be updated
        forAllIter(SLPtrList<volScalarField>,scalarFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volVectorField>,vectorFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volVectorField>,tensorFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volVectorField>,symmTensorFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volVectorField>,sphericalTensorFields,it) {
            (*it).correctBoundaryConditions();
        }

        runTime.write();

        if(timeDirs.size()>0) {
            Info << "Force writing of fields" << endl;

            forAllIter(SLPtrList<volScalarField>,scalarFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volVectorField>,vectorFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volVectorField>,tensorFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volVectorField>,symmTensorFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volVectorField>,sphericalTensorFields,it) {
                (*it).write();
            }
        }
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
