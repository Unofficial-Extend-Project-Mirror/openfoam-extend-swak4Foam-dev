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
    readAndWriteDictionaryFunctionObject

Description
    Load a dictionary into memory and write it at every write time
    Manipulation, reading etc is supposed to happen elsewhere

SourceFiles
    readAndWriteDictionaryFunctionObject.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef readAndWriteDictionaryFunctionObject_H
#define readAndWriteDictionaryFunctionObject_H

#include "simpleFunctionObject.H"
#include "IOdictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class readAndWriteDictionaryFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class readAndWriteDictionaryFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    readAndWriteDictionaryFunctionObject(const readAndWriteDictionaryFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const readAndWriteDictionaryFunctionObject&);

    bool inConstant_;

    bool inSystem_;

    bool mustExist_;

    bool autowrite_;

    word dictName_;

    word localName_;

    autoPtr<IOdictionary> dict_;

public:

    //- Runtime type information
    TypeName("readAndWriteDictionary");


    // Constructors

    //- Construct from components
    readAndWriteDictionaryFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
