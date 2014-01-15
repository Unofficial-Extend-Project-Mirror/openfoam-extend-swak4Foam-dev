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
    panicDumpFunctionObject

Description
    Checks whether a field exceeds given limits (min and max). If the 
    limits are exceeded everything is written to disk and the run is aborted

SourceFiles
    panicDumpFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef panicDumpFunctionObject_H
#define panicDumpFunctionObject_H

#include "simpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class panicDumpFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class panicDumpFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    panicDumpFunctionObject(const panicDumpFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const panicDumpFunctionObject&);

    //- Name of the field that is to be checked
    word fieldName_;

    //- maximum value that is to be looked for
    scalar maximum_;

    //- minimum value that is to be looked for
    scalar minimum_;

    //- do the actual checking
    template <class T>
    scalar getMin() const;

    //- do the actual checking
    template <class T>
    scalar getMax() const;

    //- wrapper for the checking
    template <class T>
    bool check() const;

public:

    //- Runtime type information
    TypeName("panicDump");


    // Constructors

    //- Construct from components
    panicDumpFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "panicDumpFunctionObjectTemplates.C"
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
