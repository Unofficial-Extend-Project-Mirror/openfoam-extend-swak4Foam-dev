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
    writeAndEndPythonFunctionObject

Description
    Check whether a python code evaluates to true. If yes everything is written to disk
    and the run ends gracefully

SourceFiles
    writeAndEndPythonFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef writeAndEndPythonFunctionObject_H
#define writeAndEndPythonFunctionObject_H

#include "writeAndEndFunctionObject.H"

#include "pythonInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeAndEndPythonFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class writeAndEndPythonFunctionObject
:
    public writeAndEndFunctionObject,
    public pythonInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeAndEndPythonFunctionObject(const writeAndEndPythonFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeAndEndPythonFunctionObject&);

    //- the code that should be evaluated
    string conditionCode_;

    void readParameters(const dictionary &);

protected:

    //- do the actual checking
    virtual bool endRunNow();

public:

    //- Runtime type information
    TypeName("writeAndEndPython");


    // Constructors

    //- Construct from components
    writeAndEndPythonFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary &dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
