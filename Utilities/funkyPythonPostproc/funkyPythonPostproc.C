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
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "pythonInterpreterWrapper.H"

#include "timeSelector.H"

#include "IFstream.H"

#include "printSwakVersion.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

#include "loadFieldFunction.H"

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);
    Foam::argList::validArgs.append("dictionary with specs");
#   include "addRegionOption.H"
    argList::validOptions.insert("interactive","");

#   include "setRootCase.H"

    printSwakVersion();

    IFstream theFile(args.args()[1]);
    dictionary spec(theFile);
    bool interactive(args.options().found("interactive"));

    if (!args.options().found("time") && !args.options().found("latestTime")) {
        FatalErrorIn("main()")
            << args.executable()
                << ": time/latestTime option is required" << endl
            << exit(FatalError);
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

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

    Info << "Function objects: start" << endl << endl;
    functions.start();
    Info << endl;

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info << "\nTime = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();
        Info << "Reloading fields" << endl;
        forAllIter(SLPtrList<volScalarField>,scalarFieldsPre,i){
            Info << (*i).name() << " ... " << flush;
            (*i).read();
        }
        forAllIter(SLPtrList<volVectorField>,vectorFieldsPre,i){
            Info << (*i).name() << " ... " << flush;
            (*i).read();
        }
        forAllIter(SLPtrList<volTensorField>,tensorFieldsPre,i){
            Info << (*i).name() << " ... " << flush;
            (*i).read();
        }
        forAllIter(SLPtrList<volSymmTensorField>,symmTensorFieldsPre,i){
            Info << (*i).name() << " ... " << flush;
            (*i).read();
        }
        forAllIter(SLPtrList<volSphericalTensorField>,sphericalTensorFieldsPre,i){
            Info << (*i).name() << " ... " << flush;
            (*i).read();
        }

        Info << endl << endl;

        Info << "Function objects: execute" << endl << endl;
        functions.execute();
        Info << endl;

        Info << endl;
    }

    Info << "Function objects: end" << endl << endl;
    functions.end();
    Info << endl;


    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
