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
    writeIfPythonFunctionObject

Description

    Check whether a python code evaluates to true. If yes write
    timesteps

SourceFiles
    writeIfPythonFunctionObject.C

Contributors/Copyright:
    2011-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef writeIfPythonFunctionObject_H
#define writeIfPythonFunctionObject_H

#include "conditionDrivenWritingFunctionObject.H"

#include "pythonInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeIfPythonFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class writeIfPythonFunctionObject
:
    public conditionDrivenWritingFunctionObject,
    public pythonInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeIfPythonFunctionObject(const writeIfPythonFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeIfPythonFunctionObject&);

    //- code to be evaluated during construction
    string initCode_;

    //- the code that should be evaluated
    string writeCode_;

    //- the logical expression
    string stopWriteCode_;

    //- the logical expression
    string stopCooldownCode_;

    void readParameters(const dictionary &);

    bool executeCode(const string code);
protected:

    //- should writing start now?
    virtual bool checkStartWriting();

    //- switch off writing?
    virtual bool checkStopWriting();

    //- switch off cooldown?
    virtual bool checkStopCooldown();

public:

    //- Runtime type information
    TypeName("writeIfPython");


    // Constructors

    //- Construct from components
    writeIfPythonFunctionObject
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
