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
    2010-2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
#include "swak.H"
#include "dynamicFvMesh.H"

#ifdef FOAM_HAS_FVOPTIONS
#ifdef FOAM_FVOPTIONS_IN_FV
#include "fvOptions.H"
#else
#include "fvIOoptionList.H"
#endif
#endif

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
    argList::validOptions.insert("useDynamicMesh","");
#ifdef FOAM_HAS_FVOPTIONS
    argList::validOptions.insert("useFvOptions","");
#endif

#   include "setRootCase.H"
#   include "createTime.H"

    bool useDynamicMesh=args.options().found("useDynamicMesh");

    autoPtr<fvMesh> meshPtr;

    {
        Foam::word regionName;

        if (args.optionReadIfPresent("region", regionName))
        {
            Foam::Info
                << "Create mesh " << regionName << " for time = "
                    << runTime.timeName() << Foam::nl << Foam::endl;
        }
        else
        {
            regionName = Foam::fvMesh::defaultRegion;
            Foam::Info
                << "Create mesh for time = "
                    << runTime.timeName() << Foam::nl << Foam::endl;
        }

        if(useDynamicMesh) {
            Info << "Dynamic mesh" << endl;
            meshPtr.set(
                dynamicFvMesh::New
                (
                    IOobject
                    (
                        regionName,
                        runTime.timeName(),
                        runTime,
                        IOobject::MUST_READ
                    )
                ).ptr()
            );
        } else {
            meshPtr.set(
                new fvMesh
                (
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
    }

    fvMesh &mesh=meshPtr();

    pointMesh pMesh(mesh);

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

    SLPtrList<volScalarField> scalarVolFields;
    SLPtrList<volVectorField> vectorVolFields;
    SLPtrList<volTensorField> tensorVolFields;
    SLPtrList<volSymmTensorField> symmTensorVolFields;
    SLPtrList<volSphericalTensorField> sphericalTensorVolFields;

    SLPtrList<surfaceScalarField> scalarSurfaceFields;
    SLPtrList<surfaceVectorField> vectorSurfaceFields;
    SLPtrList<surfaceTensorField> tensorSurfaceFields;
    SLPtrList<surfaceSymmTensorField> symmTensorSurfaceFields;
    SLPtrList<surfaceSphericalTensorField> sphericalTensorSurfaceFields;

    SLPtrList<pointScalarField> scalarPointFields;
    SLPtrList<pointVectorField> vectorPointFields;
    SLPtrList<pointTensorField> tensorPointFields;
    SLPtrList<pointSymmTensorField> symmTensorPointFields;
    SLPtrList<pointSphericalTensorField> sphericalTensorPointFields;

    forAll(fieldNames,fieldI) {
        word fName=fieldNames[fieldI];

        if(
            !loadFieldFunction(mesh,fName,scalarVolFields)
            &&
            !loadFieldFunction(mesh,fName,vectorVolFields)
            &&
            !loadFieldFunction(mesh,fName,tensorVolFields)
            &&
            !loadFieldFunction(mesh,fName,symmTensorVolFields)
            &&
            !loadFieldFunction(mesh,fName,sphericalTensorVolFields)
            &&
            !loadFieldFunction(mesh,fName,scalarSurfaceFields)
            &&
            !loadFieldFunction(mesh,fName,vectorSurfaceFields)
            &&
            !loadFieldFunction(mesh,fName,tensorSurfaceFields)
            &&
            !loadFieldFunction(mesh,fName,symmTensorSurfaceFields)
            &&
            !loadFieldFunction(mesh,fName,sphericalTensorSurfaceFields)
            &&
            !loadPointFieldFunction(mesh,pMesh,fName,scalarPointFields)
            &&
            !loadPointFieldFunction(mesh,pMesh,fName,vectorPointFields)
            &&
            !loadPointFieldFunction(mesh,pMesh,fName,tensorPointFields)
            &&
            !loadPointFieldFunction(mesh,pMesh,fName,symmTensorPointFields)
            &&
            !loadPointFieldFunction(mesh,pMesh,fName,sphericalTensorPointFields)
        ) {
            Info << "Field " << fName << " not found " << endl;
        }
    }

    if(
        scalarSurfaceFields.size()>0
        ||
        vectorSurfaceFields.size()>0
        ||
        tensorSurfaceFields.size()>0
        ||
        symmTensorSurfaceFields.size()>0
        ||
        sphericalTensorSurfaceFields.size()>0
    ){
        WarningIn(args.executable())
            << "Surface fields loaded. These will not be corrected"
                << endl;
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

#ifdef FOAM_HAS_FVOPTIONS
    #include "createFvOptions.H"
    if(fvOptions.PtrList::size()>0) {
        Info << "Read fvOptions with " << fvOptions.PtrList::size() << " elements"
            << endl;
        if(
            !args.options().found("useFvOptions")
            &&
            !replayDict.lookupOrDefault<bool>("useFvOptions",false)
        ) {
            Info << "Clearing fvOptions. Use 'useFvOptions' (commandline or dictionary) if you need them" << endl;
            fvOptions.PtrList::clear();
        }
    }
#endif

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

        if(useDynamicMesh) {
            Info << "Moving mesh" << endl;
            dynamicCast<dynamicFvMesh&>(mesh).update();

            #include "meshCourantNo.H"
        }

        SLPtrList<volScalarField> scalarVolFieldsPre;
        SLPtrList<volVectorField> vectorVolFieldsPre;
        SLPtrList<volTensorField> tensorVolFieldsPre;
        SLPtrList<volSymmTensorField> symmTensorVolFieldsPre;
        SLPtrList<volSphericalTensorField> sphericalTensorVolFieldsPre;

        SLPtrList<surfaceScalarField> scalarSurfaceFieldsPre;
        SLPtrList<surfaceVectorField> vectorSurfaceFieldsPre;
        SLPtrList<surfaceTensorField> tensorSurfaceFieldsPre;
        SLPtrList<surfaceSymmTensorField> symmTensorSurfaceFieldsPre;
        SLPtrList<surfaceSphericalTensorField> sphericalTensorSurfaceFieldsPre;

        SLPtrList<pointScalarField> scalarPointFieldsPre;
        SLPtrList<pointVectorField> vectorPointFieldsPre;
        SLPtrList<pointTensorField> tensorPointFieldsPre;
        SLPtrList<pointSymmTensorField> symmTensorPointFieldsPre;
        SLPtrList<pointSphericalTensorField> sphericalTensorPointFieldsPre;

        if(preloadFieldNames.size()>0) {
            Info << "Preloading fields" << endl;

            forAll(preloadFieldNames,fieldI) {
                word fName=preloadFieldNames[fieldI];

                if(
                    !loadFieldFunction(mesh,fName,scalarVolFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,vectorVolFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,tensorVolFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,symmTensorVolFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,sphericalTensorVolFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,scalarSurfaceFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,vectorSurfaceFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,tensorSurfaceFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,symmTensorSurfaceFieldsPre)
                    &&
                    !loadFieldFunction(mesh,fName,sphericalTensorSurfaceFieldsPre)
                    &&
                    !loadPointFieldFunction(mesh,pMesh,fName,scalarPointFieldsPre)
                    &&
                    !loadPointFieldFunction(mesh,pMesh,fName,vectorPointFieldsPre)
                    &&
                    !loadPointFieldFunction(mesh,pMesh,fName,tensorPointFieldsPre)
                    &&
                    !loadPointFieldFunction(mesh,pMesh,fName,symmTensorPointFieldsPre)
                    &&
                    !loadPointFieldFunction(mesh,pMesh,fName,sphericalTensorPointFieldsPre)
                ) {
                    Info << "Field " << fName << " not found " << endl;
                }
            }
        }

        // force the boundary conditions to be updated
        forAllIter(SLPtrList<volScalarField>,scalarVolFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volVectorField>,vectorVolFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volTensorField>,tensorVolFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volSymmTensorField>,symmTensorVolFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<volSphericalTensorField>,sphericalTensorVolFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<pointScalarField>,scalarPointFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<pointVectorField>,vectorPointFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<pointTensorField>,tensorPointFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<pointSymmTensorField>,symmTensorPointFields,it) {
            (*it).correctBoundaryConditions();
        }
        forAllIter(SLPtrList<pointSphericalTensorField>,sphericalTensorPointFields,it) {
            (*it).correctBoundaryConditions();
        }

        runTime.write();

        if(timeDirs.size()>0) {
            Info << "Force writing of fields" << endl;

            forAllIter(SLPtrList<volScalarField>,scalarVolFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volVectorField>,vectorVolFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volTensorField>,tensorVolFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volSymmTensorField>,symmTensorVolFields,it) {
                (*it).write();
            }
            forAllIter(SLPtrList<volSphericalTensorField>,sphericalTensorVolFields,it) {
                (*it).write();
            }
        }

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    dummyPhi.clear();

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
