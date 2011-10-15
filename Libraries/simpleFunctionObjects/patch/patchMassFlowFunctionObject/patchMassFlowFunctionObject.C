//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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

void patchMassFlowFunctionObject::write()
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
        unsigned int w = IOstream::defaultPrecision() + 7;

        OFstream& probeStream = *filePtrs_["massFlow"];

        probeStream << setw(w) << phi.time().value();

        forAll(vals, probeI)
        {
            probeStream << setw(w) << vals[probeI];
        }
        probeStream << nl;
    }

    if(verbose()) {
        Info << " MassFlows: ";

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
