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
    initPotentialFlowFunctionObject

Description
    Calculate the potential flow field solution for a field. Either just at
    the start or at every time step

SourceFiles
    initPotentialFlowFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef initPotentialFlowFunctionObject_H
#define initPotentialFlowFunctionObject_H

#include "updateSimpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class initPotentialFlowFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class initPotentialFlowFunctionObject
:
    public updateSimpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    initPotentialFlowFunctionObject(const initPotentialFlowFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const initPotentialFlowFunctionObject&);

    //- name of the velocity field
    word UName_;

    //- name of the pressure
    word pName_;

    //- write the fields?
    bool writeFields_;

    //- write the old fields?
    bool writeOldFields_;

    //- override pressure?
    bool overrideP_;

    //- initialize the U BCs?
    bool initialiseUBCs_;

protected:
    //- do the actual potential flow calculation
    void recalc();

public:

    //- Runtime type information
    TypeName("initPotentialFlow");

    // Constructors

    //- Construct from components
    initPotentialFlowFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
