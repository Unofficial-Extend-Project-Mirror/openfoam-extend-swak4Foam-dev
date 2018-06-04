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

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "stdoutFromCommandProvider.H"
#include "addToRunTimeSelectionTable.H"

#include "IFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(stdoutFromCommandProvider,0);

    // to keep the macro happy
    typedef dynamicFunctionObjectListProxy::dynamicDictionaryProvider dynamicFunctionObjectListProxydynamicDictionaryProvider;

    addToRunTimeSelectionTable 
    (
        dynamicFunctionObjectListProxydynamicDictionaryProvider,
        stdoutFromCommandProvider,
        dictionary
    );

    stdoutFromCommandProvider::stdoutFromCommandProvider(
        const dictionary& dict,
        const dynamicFunctionObjectListProxy &owner
    ):
        dynamicFunctionObjectListProxy::dynamicDictionaryProvider(
            dict,
            owner
        ),
        theCommand_(dict.lookup("theCommand"))
    {}
        

    string stdoutFromCommandProvider::getDictionaryText() {
        string cmd=theCommand_.expand();

        string buffer;
        FILE *output = popen(cmd.c_str(), "r");
        if(!output) {
            FatalErrorIn("stdoutFromCommandProvider::getDictionaryText()")
                << "Problem executing " << cmd
                    << endl
                    << exit(FatalError);

        }
        int c=fgetc(output);
        while(c!=EOF) {
            buffer+=string(char(c));
            c=fgetc(output);
        }
        if(ferror(output)) {
            FatalErrorIn("stdoutFromCommandProvider::getDictionaryText()")
                << "Problem while executing" << cmd
                    << endl
                    << exit(FatalError);

        }
        return buffer;
    }

} // namespace Foam

// ************************************************************************* //
