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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "entryToExpression.H"

#include "primitiveEntry.H"
#include "OStringStream.H"

namespace Foam {

defineTypeNameAndDebug(entryToExpression,0);

defineRunTimeSelectionTable(entryToExpression, nothing);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

entryToExpression::entryToExpression()
{
}


autoPtr<entryToExpression> entryToExpression::New
(
    const word& name
)
{
    nothingConstructorTable::iterator cstrIter =
        nothingConstructorTablePtr_->find(name);

    if (cstrIter == nothingConstructorTablePtr_->end())
    {
        FatalErrorIn
            (
                "autoPtr<entryToExpression> entryToExpression::New"
            )   << "Unknown  entryToExpression type " << name
                << endl << endl
                << "Valid entryToExpression-types are :" << endl
#ifdef FOAM_HAS_SORTED_TOC
                << nothingConstructorTablePtr_->sortedToc() // does not work in 1.6
#else
                << nothingConstructorTablePtr_->toc()
#endif
                << exit(FatalError);
    }

    return autoPtr<entryToExpression>
        (
            cstrIter()()
        );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

entryToExpression::~entryToExpression()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

string entryToExpression::fromEntry(const entry &e)
{
    const primitiveEntry &pe=dynamicCast<const primitiveEntry&>(e);
    OStringStream o;

    for (label i=0; i<pe.size(); i++) {
        o << pe[i];

        if (i < pe.size()-1){
            o << token::SPACE;
        }
    }
    return o.str();
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
