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
    WriteIfInterpreterFunctionObject

Description

    Check whether a code evaluates to true. If yes write
    timesteps

SourceFiles
    WriteIfInterpreterFunctionObject.C

Contributors/Copyright:
    2011-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef WriteIfInterpreterFunctionObject_H
#define WriteIfInterpreterFunctionObject_H

#include "conditionDrivenWritingFunctionObject.H"

#include "pythonInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class WriteIfInterpreterFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template<class Wrapper>
class WriteIfInterpreterFunctionObject
:
    public conditionDrivenWritingFunctionObject,
    public Wrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    WriteIfInterpreterFunctionObject(const WriteIfInterpreterFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const WriteIfInterpreterFunctionObject&);

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
    TypeName("WriteIfInterpreter");


    // Constructors

    //- Construct from components
    WriteIfInterpreterFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary &dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#include "WriteIfInterpreterFunctionObject.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
