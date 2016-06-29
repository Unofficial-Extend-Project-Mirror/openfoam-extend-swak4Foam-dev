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
    setDeltaTByTimelineFunctionObject

Description
    Set the time-step according to a linear piecewise function

SourceFiles
    setDeltaTByTimelineFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef setDeltaTByTimelineFunctionObject_H
#define setDeltaTByTimelineFunctionObject_H

#include "timeManipulationFunctionObject.H"

#include "interpolationTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class setDeltaTByTimelineFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class setDeltaTByTimelineFunctionObject
:
    public timeManipulationFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    setDeltaTByTimelineFunctionObject(const setDeltaTByTimelineFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const setDeltaTByTimelineFunctionObject&);

    interpolationTable<scalar> deltaTTable_;

public:

    //- Runtime type information
    TypeName("setDeltaTByTimeline");


    // Constructors

    //- Construct from components
    setDeltaTByTimelineFunctionObject
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
