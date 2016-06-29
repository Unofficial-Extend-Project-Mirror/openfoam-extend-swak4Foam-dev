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
    setEndTimeWithPythonFunctionObject

Description
    Set the end time according to a number returned by a python-snipplet

SourceFiles
    setEndTimeWithPythonFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef setEndTimeWithPythonFunctionObject_H
#define setEndTimeWithPythonFunctionObject_H

#include "timeManipulationWithPythonFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class setEndTimeWithPythonFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class setEndTimeWithPythonFunctionObject
:
    public timeManipulationWithPythonFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    setEndTimeWithPythonFunctionObject(const setEndTimeWithPythonFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const setEndTimeWithPythonFunctionObject&);

    //- the code that should be evaluated in the beginning
    string initCode_;

    //- the code that should be evaluated
    string endTimeCode_;

    void readParameters(const dictionary &);

public:

    //- Runtime type information
    TypeName("setEndTimeWithPython");


    // Constructors

    //- Construct from components
    setEndTimeWithPythonFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    virtual scalar endTime();

    bool read(const dictionary &dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
