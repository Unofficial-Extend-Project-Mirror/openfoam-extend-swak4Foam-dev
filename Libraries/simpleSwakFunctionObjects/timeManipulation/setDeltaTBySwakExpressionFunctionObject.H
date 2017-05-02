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

Class
    setDeltaTBySwakExpressionFunctionObject

Description
    Set the time-step according to a swakExpression

SourceFiles
    setDeltaTBySwakExpressionFunctionObject.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef setDeltaTBySwakExpressionFunctionObject_H
#define setDeltaTBySwakExpressionFunctionObject_H

#include "timeManipulationFunctionObject.H"

#include "swakDataEntry.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
    using namespace Function1Types;
#endif

/*---------------------------------------------------------------------------*\
                           Class setDeltaTBySwakExpressionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class setDeltaTBySwakExpressionFunctionObject
:
    public timeManipulationFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    setDeltaTBySwakExpressionFunctionObject(const setDeltaTBySwakExpressionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const setDeltaTBySwakExpressionFunctionObject&);

    swakDataEntry<scalar> deltaTExpression_;

public:

    //- Runtime type information
    TypeName("setDeltaTBySwakExpression");


    // Constructors

    //- Construct from components
    setDeltaTBySwakExpressionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    virtual scalar deltaT();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
