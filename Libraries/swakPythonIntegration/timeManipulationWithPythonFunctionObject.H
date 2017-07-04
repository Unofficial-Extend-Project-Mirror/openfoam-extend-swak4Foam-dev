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
    timeManipulationWithPythonFunctionObject

Description
    Basis for function objects that manipulate the runTime via Python-snipplets
SourceFiles
    timeManipulationWithPythonFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef timeManipulationWithPythonFunctionObject_H
#define timeManipulationWithPythonFunctionObject_H

#include "timeManipulationFunctionObject.H"

#include "pythonInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class timeManipulationWithPythonFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class timeManipulationWithPythonFunctionObject
:
    public timeManipulationFunctionObject,
    public pythonInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    timeManipulationWithPythonFunctionObject(const timeManipulationWithPythonFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const timeManipulationWithPythonFunctionObject&);

public:

    //- Runtime type information
    TypeName("timeManipulationWithPython");


    // Constructors

    //- Construct from components
    timeManipulationWithPythonFunctionObject
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
