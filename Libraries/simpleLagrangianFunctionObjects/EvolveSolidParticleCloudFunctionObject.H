//  OF-extend Revision: $Id$
/*---------------------------------------------------------------------------*\
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
    EvolveSolidParticleCloudFunctionObject

Description
    Function object that creates and evolves a SolidParticleCloud at every timestep


SourceFiles
    EvolveSolidParticleCloudFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef EvolveSolidParticleCloudFunctionObject_H
#define EvolveSolidParticleCloudFunctionObject_H

#include "EvolveCloudFunctionObject.H"
#include "solidParticleCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class EvolveSolidParticleCloudFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class EvolveSolidParticleCloudFunctionObject
:
    public EvolveCloudFunctionObject<solidParticleCloud>
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    EvolveSolidParticleCloudFunctionObject(const EvolveSolidParticleCloudFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const EvolveSolidParticleCloudFunctionObject&);

public:

    //- Runtime type information
    TypeName("EvolveSolidParticleCloudFunctionObject");


    // Constructors

    //- Construct from components
    EvolveSolidParticleCloudFunctionObject
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
