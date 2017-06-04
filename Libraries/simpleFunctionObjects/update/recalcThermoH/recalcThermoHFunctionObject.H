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
    recalcThermoHFunctionObject

Description
    Recalculate enthalpy according to the current temperature

SourceFiles
    recalcThermoHFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef recalcThermoHFunctionObject_H
#define recalcThermoHFunctionObject_H

#include "updateSimpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class recalcThermoHFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class recalcThermoHFunctionObject
:
    public updateSimpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    recalcThermoHFunctionObject(const recalcThermoHFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const recalcThermoHFunctionObject&);

protected:
    //- do the actual calculation
    void recalc();

public:

    //- Runtime type information
    TypeName("recalcThermoH");

    // Constructors

    //- Construct from components
    recalcThermoHFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
