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
    Foam::StoredStackExpressionResult

Description

SourceFiles
    StoredStackExpressionResultI.H
    StoredStackExpressionResult.C
    StoredStackExpressionResultIO.C

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef StoredStackExpressionResult_H
#define StoredStackExpressionResult_H

#include "StackExpressionResult.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class StoredStackExpressionResult Declaration
\*---------------------------------------------------------------------------*/

class StoredStackExpressionResult
:
    public StackExpressionResult
{
    // Private data

protected:

    //- reset at new timestep according to type
    virtual void resetInternal();

public:

     //- Runtime type information
    TypeName("StoredStackExpressionResult");

    // Constructors

    //- Construct null
    StoredStackExpressionResult();

    //- Construct as copy
    StoredStackExpressionResult(const StoredStackExpressionResult&);

    //- Construct from a dictionary
    StoredStackExpressionResult(const dictionary&);

    // Destructor

    virtual ~StoredStackExpressionResult();

    void operator=(const StoredStackExpressionResult&);

    virtual autoPtr<ExpressionResult> clone() const {
        return autoPtr<ExpressionResult>(
            new StoredStackExpressionResult(*this)
        );
    }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "StoredStackExpressionResultI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
