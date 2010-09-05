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
    funkySetFields

Description

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "PatchValueExpressionDriver.H"

#include "timeSelector.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"

    argList::validOptions.insert("dict","<dictionary to use>");

#   include "setRootCase.H"

    word dictName="funkySetBoundaryDict";
    if(args.options().found("dict")) {
        dictName=args.options()["dict"];
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    IOdictionary funkyDict
        (
            IOobject
            (
                dictName,
                runTime.system(),
                mesh,
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            )
        );

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "Time = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();

        forAllIter(dictionary,funkyDict,it) {
            const dictionary &part=(*it).dict();
        
            word fieldName=part["field"];
            
            Info << "\n\nPart: " << (*it).keyword() 
                << " working on field " << fieldName << endl;
            
            IOdictionary field(
                IOobject
                (
                    fieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE
                )
            );

            PtrList<entry> parts=funkyDict.lookup("expressions");
    
            field.regIOobject::write();
        }
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
