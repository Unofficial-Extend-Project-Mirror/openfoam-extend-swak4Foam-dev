/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "funkyCyclicACMIPolyPatch.H"
#include "SubField.H"
#include "Time.H"
#include "addToRunTimeSelectionTable.H"

#include "cyclicACMIFvPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(funkyCyclicACMIPolyPatch, 0);

    addToRunTimeSelectionTable(polyPatch, funkyCyclicACMIPolyPatch, word);
    addToRunTimeSelectionTable(polyPatch, funkyCyclicACMIPolyPatch, dictionary);

    addNamedToRunTimeSelectionTable(fvPatch,cyclicACMIFvPatch,polyPatch,funkyCyclicACMI);
}

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::funkyCyclicACMIPolyPatch::resetAMI
(
    const AMIPatchToPatchInterpolation::interpolationMethod& meth
) const
{
    cyclicACMIPolyPatch::resetAMI(meth);
}




// * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * * //

Foam::funkyCyclicACMIPolyPatch::funkyCyclicACMIPolyPatch
(
    const word& name,
    const label size,
    const label start,
    const label index,
    const polyBoundaryMesh& bm,
    const word& patchType,
    const transformType transform
)
:
    cyclicACMIPolyPatch(name, size, start, index, bm, patchType, transform),
    openField_(word::null)
{
}


Foam::funkyCyclicACMIPolyPatch::funkyCyclicACMIPolyPatch
(
    const word& name,
    const dictionary& dict,
    const label index,
    const polyBoundaryMesh& bm,
    const word& patchType
)
:
    cyclicACMIPolyPatch(name, dict, index, bm, patchType),
    openField_(dict.lookup("openField"))
{
}


Foam::funkyCyclicACMIPolyPatch::funkyCyclicACMIPolyPatch
(
    const funkyCyclicACMIPolyPatch& pp,
    const polyBoundaryMesh& bm
)
:
    cyclicACMIPolyPatch(pp, bm),
    openField_(pp.openField_)
{
}


Foam::funkyCyclicACMIPolyPatch::funkyCyclicACMIPolyPatch
(
    const funkyCyclicACMIPolyPatch& pp,
    const polyBoundaryMesh& bm,
    const label index,
    const label newSize,
    const label newStart,
    const word& nbrPatchName,
    const word& nonOverlapPatchName,
    const word& openField
)
:
    cyclicACMIPolyPatch(
        pp,
        bm,
        index,
        newSize,
        newStart,
        nbrPatchName,
        nonOverlapPatchName
    ),
    openField_(openField)
{
}


Foam::funkyCyclicACMIPolyPatch::funkyCyclicACMIPolyPatch
(
    const funkyCyclicACMIPolyPatch& pp,
    const polyBoundaryMesh& bm,
    const label index,
    const labelUList& mapAddressing,
    const label newStart
)
:
    cyclicACMIPolyPatch(pp, bm, index, mapAddressing, newStart),
    openField_(pp.openField_)
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::funkyCyclicACMIPolyPatch::~funkyCyclicACMIPolyPatch()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //



void Foam::funkyCyclicACMIPolyPatch::write(Ostream& os) const
{
    cyclicACMIPolyPatch::write(os);

    os.writeKeyword("openField") << openField_
        << token::END_STATEMENT << nl;
}


// ************************************************************************* //
