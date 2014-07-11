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

Class
    Foam::entryToExpression

Description
    Helper class that gets an entry and converts it into a valid
    swak-expression

SourceFiles
    entryToExpression.C

Contributors/Copyright:
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef entryToExpression_H
#define entryToExpression_H

#include "swak.H"

#include "entry.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class entryToExpression Declaration
\*---------------------------------------------------------------------------*/

class entryToExpression
{
    // Private data

    //- Construct as copy
    entryToExpression(const entryToExpression&);

    void operator=(const entryToExpression&);

protected:

    template<class Type>
    string toStr(
        const Type &data,
        const word &prefix
    );

public:
    //- Runtime type information
    TypeName("entryToExpression");

    declareRunTimeSelectionTable
    (
        autoPtr,
        entryToExpression,
        nothing,
        (
        ),
        ()
    );

    // Constructors

    //- Construct null
    entryToExpression();

   //- Return a reference to the selected value driver
    static autoPtr<entryToExpression> New
    (
        const word& name
    );

    // Destructor

    virtual ~entryToExpression();

    virtual string toExpr(const entry &) = 0;

    static string fromEntry(const entry &);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "entryToExpressionI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
