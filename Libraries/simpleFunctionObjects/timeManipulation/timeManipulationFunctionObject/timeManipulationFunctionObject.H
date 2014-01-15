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
    timeManipulationFunctionObject

Description
    Basis for functionObjects that manipulate the runTime for instance by
    - setting the timesteps
    - setting the endTime
    Concrete subclasses have to override the runTime

SourceFiles
    timeManipulationFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef timeManipulationFunctionObject_H
#define timeManipulationFunctionObject_H

#include "simpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class timeManipulationFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class timeManipulationFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    timeManipulationFunctionObject(const timeManipulationFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const timeManipulationFunctionObject&);

    bool tolerateAdaptiveTimestep_;

    scalar myEndTime_;

public:

    //- Runtime type information
    TypeName("timeManipulation");


    // Constructors

    //- Construct from components
    timeManipulationFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void write();

    //- new timestep. To be overridden. This is only a dummy-implementation
    virtual scalar deltaT();

    //- new endTime. To be overridden. This is only a dummy-implementation
    virtual scalar endTime();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
