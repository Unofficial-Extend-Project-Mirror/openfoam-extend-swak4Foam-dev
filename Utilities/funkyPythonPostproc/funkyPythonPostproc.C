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
    funkyPythonPostproc

Description

Contributors/Copyright:
    2013-2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "swak.H"

#include "pythonInterpreterWrapper.H"

#include "timeSelector.H"

#include "IFstream.H"

#include "printSwakVersion.H"

#include "RepositoryBase.H"

#include "simpleDataFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

#include "loadFieldFunction.H"

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);
    Foam::argList::validArgs.append("dictionary with specs");
#   include "addRegionOption.H"
    argList::validOptions.insert("interactive","");
    argList::validOptions.insert("debugOnException","");
    argList::validOptions.insert("postProcDir","dirname");

#   include "setRootCase.H"

    printSwakVersion();

    IFstream theFile(args.args()[1]);
    dictionary spec(theFile);

    if(args.options().found("postProcDir")) {
        simpleDataFunctionObject::setPostProcDir(
            word(args.options()["postProcDir"])
        );
    } else {
        simpleDataFunctionObject::setPostProcDir(
            // a bit clumsy because some foam-versions have no fileName.name(bool)
            fileName(fileName(args.args()[1]).name()).lessExt()+"Output"
        );
    }
    bool interactive(args.options().found("interactive"));
    bool failOnException(!args.options().found("debugOnException"));

    if (!args.options().found("time") && !args.options().found("latestTime")) {
        FatalErrorIn("main()")
            << args.executable()
                << ": time/latestTime option is required" << endl
            << exit(FatalError);
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    if(!spec.found("libs")) {
        FatalErrorIn(args.executable())
            << "No entry 'libs' in " << spec.name()
                << endl
                << exit(FatalError);
    }

#ifdef FOAM_DLLIBRARY_USES_STATIC_METHODS
    dlLibraryTable::open(spec,"libs");
#else
    dlLibraryTable table(spec,"libs");
#endif

    wordList preloadFieldNames(spec.lookup("preloadFields"));

    if(!spec.found("functions")) {
        FatalErrorIn(args.executable())
            << "No entry 'functions' in " << spec.name()
                << endl
                << exit(FatalError);
    }

    Info << "Preloading fields" << endl;
    SLPtrList<volScalarField> scalarFieldsPre;
    SLPtrList<volVectorField> vectorFieldsPre;
    SLPtrList<volTensorField> tensorFieldsPre;
    SLPtrList<volSymmTensorField> symmTensorFieldsPre;
    SLPtrList<volSphericalTensorField> sphericalTensorFieldsPre;

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

    functionObjectList functions(
        mesh.time(),
        spec
    );

    const dictionary &pySpec=spec.subDict("python");

    Info << "Function objects: start" << endl;
    functions.start();
    Info << functions.size() << " function objects read" << endl;
    Info << endl;

    pythonInterpreterWrapper python(
        mesh,
        pySpec,
        true  // force reading of to namespace
    );
    python.setInteractive(
        interactive,
        !failOnException
    );

    python.initEnvironment(mesh.time());
    python.setRunTime(mesh.time());

    string startCode=python.readCode(
        "start"
    );
    string executeCode=python.readCode(
        "execute"
    );
    string endCode=python.readCode(
        "end"
    );
    if(startCode!="") {
        Info << "Executing start code" << endl;
        bool ok=python.executeCode(
            startCode,
            true,
            failOnException
        );
    }

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);
        python.setRunTime(mesh.time());

        Foam::Info << "\nTime = " << runTime.timeName() << Foam::endl;

        RepositoryBase::updateRepos();

        mesh.readUpdate();
        Info << "Reloading fields" << endl;
        // scalarFieldsPre.clear();
        // loadFieldFunction(mesh,"p",scalarFieldsPre);
        Info << reloadFieldsFunction(mesh,scalarFieldsPre).c_str() << flush;
        Info << reloadFieldsFunction(mesh,vectorFieldsPre).c_str() << flush;
        Info << reloadFieldsFunction(mesh,tensorFieldsPre).c_str() << flush;
        Info << reloadFieldsFunction(mesh,symmTensorFieldsPre).c_str() << flush;
        Info << reloadFieldsFunction(mesh,sphericalTensorFieldsPre).c_str() << flush;

        Info << endl << endl;

        Info << "Function objects: execute " << functions.size() << " function objects" << endl;
        functions.execute(
#ifndef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
            true
#endif
        );
        Info << endl;

        if(executeCode!="") {
            Info << "Executing execute code" << endl;
            bool ok=python.executeCode(
                executeCode,
                true,
                failOnException
            );
        }

        Info << endl;
    }

    Info << "Function objects: end " << functions.size() << " function objects" << endl << endl;
    functions.end();
    Info << endl;

    if(endCode!="") {
        Info << "Executing end code" << endl;
        bool ok=python.executeCode(
            endCode,
            true,
            failOnException
        );
        Info << endl;
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
