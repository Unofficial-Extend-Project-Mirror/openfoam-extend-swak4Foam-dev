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

Contributors/Copyright:
    2010, 2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "PatchValueExpressionDriver.H"

#include "timeSelector.H"

#include "OFstream.H"

#include "printSwakVersion.H"

#include "RepositoryBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"

    argList::validOptions.insert("dict","<dictionary to use>");
    argList::validOptions.insert("cacheFields","");

#   include "setRootCase.H"

    printSwakVersion();

    word dictName="funkySetBoundaryDict";
    if(args.options().found("dict")) {
        dictName=args.options()["dict"];
    }

    bool cacheFields=args.options().found("cacheFields");
    if(cacheFields) {
        WarningIn("main()")
            << "The current behaviour is to cache fields that were read from disc. "
                << "This may lead to unexpected behaviour as previous modifications "
                << "will not be visible."
                << endl;
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

        RepositoryBase::updateRepos();

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
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                )
            );
            // deregister the dictionary so that the field can work on itself
            field.checkOut();
            {
                // this way it doesn't matter that the file is not of the right class
                IFstream inStream(field.filePath());
                field.readHeader(inStream);
                field.readData(inStream);
            }

            List<dictionary> expressions(part.lookup("expressions"));

            forAll(expressions,expressionI) {
                const dictionary &expression=expressions[expressionI];

                word target(expression["target"]);
                word patchName(expression["patchName"]);
                string expr(expression["expression"]);
                Info << "Setting " << target << " on " << patchName
                    << " the expression " << expr << endl;

                PatchValueExpressionDriver driver(expression,mesh);
                driver.setSearchBehaviour(
                    cacheFields,
                    false,
                    true             // search on disc
                );

                driver.clearVariables();
                driver.parse(expr);

                dictionary &patchDict=field.subDict("boundaryField").subDict(patchName);

                if(patchDict.found(target)) {
                    // Does not work (memory problem)
                    //                    patchDict.changeKeyword(keyType(target),keyType(word(target+"Old")),true);
                    if(patchDict.found(target+"Old")) {
                        patchDict.remove(target+"Old");
                    }
                    patchDict.changeKeyword(keyType(target),keyType(word(target+"Old")));
                }
                OStringStream result;
                string newEntry=driver.outputEntry();
                patchDict.set(target,newEntry.c_str());
            }

            {
                // this way the class is not overwritten
                word actualClass=field.headerClassName();

                OStringStream headerStream;
                field.writeHeader(headerStream);
                string newHeader=headerStream.str().replace("dictionary",actualClass);

                OFstream outStream(field.filePath());
                outStream << newHeader.c_str();
                field.writeData(outStream);
            }
        }
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
