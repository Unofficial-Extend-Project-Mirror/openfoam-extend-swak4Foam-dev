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
    2008-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "patchMassFlowFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"
#include "surfaceFields.H"
#include "IOmanip.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(patchMassFlowFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        patchMassFlowFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

patchMassFlowFunctionObject::patchMassFlowFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    patchFunctionObject(name,t,dict),
    solver_(obr_,dict)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void patchMassFlowFunctionObject::writeSimple()
{
    scalarField vals(patchNames_.size(), -GREAT);

    const surfaceScalarField &phi=obr_.lookupObject<surfaceScalarField>(solver_.phi());

    forAll(patchIndizes_,i) {
        label patchI=patchIndizes_[i];

        if(patchI>=0) {
            vals[i]=sum(
                phi.boundaryField()[patchI]
            );
            reduce(vals[i],sumOp<scalar>());
        }
    }

    forAll(vals,i) {
        vals[i]*=factor();
    }

    if (Pstream::master())
    {
        writeTime("massFlow",phi.time().value());
        writeData("massFlow",vals);
        endData("massFlow");
    }

    if(verbose()) {
        Info<< regionString()
            << " MassFlows: ";

        forAll(patchNames_, patchI)
        {
            Info << "  " << patchNames_[patchI] << " = "
                << vals[patchI];
        }

        Info << endl;
    }

}

    //- Names of the files
wordList patchMassFlowFunctionObject::fileNames()
{
    return wordList(1,"massFlow");
}

word patchMassFlowFunctionObject::dirName()
{
    return word("patchMassFlows");
}


} // namespace Foam

// ************************************************************************* //
