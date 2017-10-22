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
    2011-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "StdoutFromScriptProvider.H"
#include "addToRunTimeSelectionTable.H"

#include "IFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    template<class Wrapper>
    StdoutFromScriptProvider<Wrapper>::StdoutFromScriptProvider(
        const dictionary& dict,
        const dynamicFunctionObjectListProxy &owner
    ):
        dynamicFunctionObjectListProxy::dynamicDictionaryProvider(
            dict,
            owner
        ),
        Wrapper(
            const_cast<dynamicFunctionObjectListProxy&>(owner).obr(),
            dict
        )
    {
        if(!this->parallelNoRun()) {
            this->initEnvironment(owner.time());

            this->setRunTime(owner.time());
        }

        this->readCode(dict,"script",theScript_);
    }


    template<class Wrapper>
    string StdoutFromScriptProvider<Wrapper>::getDictionaryText() {
        string buffer;

        this->dictionariesToInterpreterStructs();

        this->executeCodeCaptureOutput(
            theScript_,
            buffer,
            false,
            true
        );

        this->interpreterStructsToDictionaries();

        return buffer;
    }

} // namespace Foam

// ************************************************************************* //
