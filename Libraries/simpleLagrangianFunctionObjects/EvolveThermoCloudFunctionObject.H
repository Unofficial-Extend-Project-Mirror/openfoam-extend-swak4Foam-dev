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
    EvolveThermoCloudFunctionObject

Description
    Function object that creates and evolves a ThermoCloud at every timestep

SourceFiles
    EvolveThermoCloudFunctionObject.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $ 
\*---------------------------------------------------------------------------*/

#ifndef EvolveThermoCloudFunctionObject_H
#define EvolveThermoCloudFunctionObject_H

#include "EvolveCloudFunctionObject.H"
#include "basicThermoCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class EvolveThermoCloudFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class EvolveThermoCloudFunctionObject
:
    public EvolveCloudFunctionObject<basicThermoCloud>
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    EvolveThermoCloudFunctionObject(const EvolveThermoCloudFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const EvolveThermoCloudFunctionObject&);

public:

    //- Runtime type information
    TypeName("EvolveThermoCloudFunctionObject");


    // Constructors

    //- Construct from components
    EvolveThermoCloudFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
