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
    recalcPhiFunctionObject

Description
    Recalculate phi according to the current velocity and density

SourceFiles
    recalcPhiFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef recalcPhiFunctionObject_H
#define recalcPhiFunctionObject_H

#include "updateSimpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class recalcPhiFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class recalcPhiFunctionObject
:
    public updateSimpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    recalcPhiFunctionObject(const recalcPhiFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const recalcPhiFunctionObject&);

    //- name of the velocity field
    word UName_;

    //- name of the phi field
    word phiName_;

    //- name of the pressure field
    word pName_;

    //- name of the density field (if needed)
    word rhoName_;

    //- write the fields?
    bool writeFields_;

    //- write the old fields?
    bool writeOldFields_;

protected:
    //- do the actual phi calculation
    void recalc();

public:

    //- Runtime type information
    TypeName("recalcPhi");

    // Constructors

    //- Construct from components
    recalcPhiFunctionObject
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
