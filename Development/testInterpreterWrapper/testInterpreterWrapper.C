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

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Application
    test

Description

\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "polyMesh.H"
#include "foamTime.H"
#include "IFstream.H"

#include "generalInterpreterWrapper.H"


using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc,char **argv)
{
    argList::validArgs.append("specDictionary");
    argList::validOptions.insert("debugSwitches","");

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createPolyMesh.H"

    IFstream f(args.additionalArgs()[0]);
    dictionary dict(f);

    if(args.options().found("debugSwitches")) {
        generalInterpreterWrapper::debug=1;
    }

    Info << nl << "Getting Wrapper" << endl;

    autoPtr<generalInterpreterWrapper> pInter=
        generalInterpreterWrapper::New(
            mesh,
            dict
        );

    generalInterpreterWrapper &inter=pInter();

    Info << "Wrapper loaded" << endl;

    inter.interactiveLoop("check the environment");

    Info << "End" << endl;

    return 0;
}

// ************************************************************************* //
