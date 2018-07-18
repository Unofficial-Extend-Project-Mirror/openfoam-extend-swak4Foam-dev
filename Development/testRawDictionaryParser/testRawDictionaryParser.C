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

#include "DictionaryFoamDictionaryParserDriver.H"
#include "IFstream.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int args,char **argv)
{
    if(args<2) {
        Info << "Filename needed" << endl;
        return 1;
    }
    IFstream f(argv[1]);

    DictionaryFoamDictionaryParserDriver parser(
        //        RawFoamDictionaryParserDriver::FailError
        RawFoamDictionaryParserDriver::WarnError
        //        RawFoamDictionaryParserDriver::SilentError
    );

    Info << "Start parsing" << endl;

    label result=parser.parse(f);
    //    label result=parser.parse(dictionary(f));

    Info << nl << "Parser ended with result " << result << endl;

    Info << nl << "Result:" << nl << parser.getResult();

    return 0;
}

// ************************************************************************* //
