/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
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

Contributors/Copyright:
    2011-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "stdoutFromPythonScriptProvider.H"
#include "addToRunTimeSelectionTable.H"

#include "IFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(stdoutFromPythonScriptProvider,0);

    // to keep the macro happy
    typedef dynamicFunctionObjectListProxy::dynamicDictionaryProvider dynamicFunctionObjectListProxydynamicDictionaryProvider;

    addToRunTimeSelectionTable
    (
        dynamicFunctionObjectListProxydynamicDictionaryProvider,
        stdoutFromPythonScriptProvider,
        dictionary
    );

    stdoutFromPythonScriptProvider::stdoutFromPythonScriptProvider(
        const dictionary& dict,
        const dynamicFunctionObjectListProxy &owner
    ):
        dynamicFunctionObjectListProxy::dynamicDictionaryProvider(
            dict,
            owner
        ),
        pythonInterpreterWrapper(
            const_cast<dynamicFunctionObjectListProxy&>(owner).obr(),
            dict
        )
    {
        if(!parallelNoRun()) {
            initEnvironment(owner.time());

            setRunTime(owner.time());
        }

        readCode(dict,"script",thePythonScript_);
    }


    string stdoutFromPythonScriptProvider::getDictionaryText() {
        string buffer;

        executeCodeCaptureOutput(
            thePythonScript_,
            buffer,
            false,
            true
        );

        return buffer;
    }

} // namespace Foam

// ************************************************************************* //
