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

#include "solvePDECommon.H"

#include "polyMesh.H"

namespace Foam {
    defineTypeNameAndDebug(solvePDECommon,0);
}

template<>
const char* Foam::NamedEnum<Foam::solvePDECommon::solveAt,3>::names[]=
{
    "startup",
    "timestep",
    "write"
};

const Foam::NamedEnum<Foam::solvePDECommon::solveAt,3> Foam::solvePDECommon::solveAtNames_;

Foam::solvePDECommon::solvePDECommon
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    active_(true),
    obr_(obr),
    name_(name)
{
    if (!isA<polyMesh>(obr))
    {
        active_=false;
        WarningIn("solvePDECommon::solvePDECommon")
            << "Not a polyMesh. Nothing I can do"
                << endl;
    }
}

Foam::solvePDECommon::~solvePDECommon()
{}

void Foam::solvePDECommon::read(const dictionary& dict)
{
    if(active_) {
        solveAt_=
            solveAtNames_.read(
                dict.lookup("solveAt")
            );
        fieldName_=word(dict.lookup("fieldName"));

        steady_=readBool(dict.lookup("steady"));
    }
}

void Foam::solvePDECommon::execute()
{
    if(solveAt_==saTimestep) {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        solve();

        // as this is executed after the general write, write the field separately 
        if(mesh.time().outputTime()) {
            writeData();
        }
    }
}


void Foam::solvePDECommon::end()
{
    execute();
}

void Foam::solvePDECommon::write()
{
    const fvMesh& mesh = refCast<const fvMesh>(obr_);
    if(
        solveAt_==saWrite
        &&
        mesh.time().outputTime()
    ) {
        solve();

        writeData();
    }
}

// ************************************************************************* //
