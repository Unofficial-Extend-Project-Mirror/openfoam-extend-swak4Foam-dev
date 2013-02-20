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
    pythonFluIntegrationFunctionObject

Description
    Integrates a python-interpreter that allows the execution of Python-scripts.

    On the Python-Side imports the Foam-module from PythonFlu and builds an
    object that references the mesh

SourceFiles
    pythonFluIntegrationFunctionObject.C

Contributors/Copyright:
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef pythonFluIntegrationFunctionObject_H
#define pythonFluIntegrationFunctionObject_H

#include "pythonIntegrationFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class pythonFluIntegrationFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class pythonFluIntegrationFunctionObject
:
    public pythonIntegrationFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    pythonFluIntegrationFunctionObject(const pythonFluIntegrationFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const pythonFluIntegrationFunctionObject&);

public:

    //- Runtime type information
    TypeName("pythonFluIntegration");


    // Constructors

    //- Construct from components
    pythonFluIntegrationFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    virtual ~pythonFluIntegrationFunctionObject();

    // Member Functions

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
