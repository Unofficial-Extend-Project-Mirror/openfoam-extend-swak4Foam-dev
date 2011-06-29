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
    writeFieldsOftenFunctionObject

Description
    Lets the user specify an additional set of fields that is to be written 
    more frequently than specified in the controlDict

    writeControl accepts the same values 

Example usage: writes Temperature every third timestep after t=11

functions 
(
    writeT {
        functionObjectLibs ("libsimpleFunctionObjects.so");
        type writeFieldsOften;
        fieldNames ( T );
        after 11;
        writeControl timeStep;
        writeIntervall 3;
    }
);

SourceFiles
    writeFieldsOftenFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef writeFieldsOftenFunctionObject_H
#define writeFieldsOftenFunctionObject_H

#include "writeFieldsGeneralFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeFieldsOftenFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class writeFieldsOftenFunctionObject
:
    public writeFieldsGeneralFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeFieldsOftenFunctionObject(const writeFieldsOftenFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const writeFieldsOftenFunctionObject&);

protected:

    //- Overwrite the default output time
    virtual bool outputTime();

    //- how writting is controlled
    Time::writeControls writeControl_;

    //- how often we write
    scalar writeInterval_;

    //- store the last output
    label outputTimeIndex_;
public:

    //- Runtime type information
    TypeName("writeFieldsOften");


    // Constructors

    //- Construct from components
    writeFieldsOftenFunctionObject
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
