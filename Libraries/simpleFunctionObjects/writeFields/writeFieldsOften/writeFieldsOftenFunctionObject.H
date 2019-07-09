/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
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

Contributors/Copyright:
    2008-2011, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef writeFieldsOftenFunctionObject_H
#define writeFieldsOftenFunctionObject_H

#include "writeFieldsGeneralFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef FOAM_WRITECONTROL_IN_TIME_CHANGED
typedef Foam::Time::writeControl TimeWriteControl;
#else
typedef Foam::Time::writeControls TimeWriteControl;
#endif

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
    virtual bool outputTime(const bool forceWrite);

    //- how writting is controlled
    TimeWriteControl writeControl_;

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
