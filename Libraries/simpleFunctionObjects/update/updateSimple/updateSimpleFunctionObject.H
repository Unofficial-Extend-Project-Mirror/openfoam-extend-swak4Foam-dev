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
    updateSimpleFunctionObject

Description
    A function object base class for initializing or updating fields

SourceFiles
    updateSimpleFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef updateSimpleFunctionObject_H
#define updateSimpleFunctionObject_H

#include "simpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class updateSimpleFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class updateSimpleFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    updateSimpleFunctionObject(const updateSimpleFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const updateSimpleFunctionObject&);

    //- do this at startup or every time
    bool onlyAtStartup_;

    //- only do if this isn't a restart
    bool runIfStartTime_;

protected:
    //- do the actual recalculation
    virtual void recalc() = 0;

public:

    //- Runtime type information
    TypeName("updateSimple");

    // Constructors

    //- Construct from components
    updateSimpleFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
