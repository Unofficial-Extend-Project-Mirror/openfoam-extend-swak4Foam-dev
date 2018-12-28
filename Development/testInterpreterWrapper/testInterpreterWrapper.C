/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Application
    test

Description

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "polyMesh.H"
#include "swakTime.H"
#include "IFstream.H"

#include "generalInterpreterWrapper.H"

#include "swak.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc,char **argv)
{
    argList::validArgs.append("specDictionary");
    argList::validArgs.append("dataDictionary");
    argList::validOptions.insert("debugSwitches","");

#ifndef FOAM_DLLIBRARY_USES_STATIC_METHODS
    dlLibraryTable table;
#endif

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createPolyMesh.H"

    Info << nl << "Getting Wrapper " << args.args()[1] << endl;

    IFstream f(args.args()[1]);
    dictionary dict(f);

    if(dict.found("libs")) {
        List<fileName> libList(dict["libs"]);
        forAll(libList,i) {
            const fileName &theLib=libList[i];
            Info << "Loading library " << theLib << endl;
#ifdef FOAM_DLLIBRARY_USES_STATIC_METHODS
            dlLibraryTable::open(theLib);
#else
            table.open(theLib);
#endif
        }
    }
    if(args.options().found("debugSwitches")) {
        generalInterpreterWrapper::debug=1;
    }

    autoPtr<generalInterpreterWrapper> pInter=
        generalInterpreterWrapper::New(
            mesh,
            dict
        );

    generalInterpreterWrapper &inter=pInter();

    Info << "Wrapper loaded" << endl;

    Info << "Load data from " << args.args()[2] << endl;
    IFstream df(args.args()[2]);
    dictionary dataDict(df);
    inter.insertDictionary(
        "data",
        dataDict
    );

    inter.interactiveLoop("check the environment");

    dictionary back;

    Info << "Getting the data back" << endl;
    inter.extractDictionary(
        "data",
        back
    );

    Info << "Got it back" << endl;

    Info << back;

    Info << "End" << endl;

    return 0;
}

// ************************************************************************* //
