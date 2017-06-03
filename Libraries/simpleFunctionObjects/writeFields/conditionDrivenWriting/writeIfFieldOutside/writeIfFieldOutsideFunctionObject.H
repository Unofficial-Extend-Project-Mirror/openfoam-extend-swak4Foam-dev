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
    writeIfFieldOutsideFunctionObject

Description
    Checks whether a field exceeds given limits (min and max). Triggers write
    if that is the case

SourceFiles
    writeIfFieldOutsideFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: writeIfFieldOutsideFunctionObject.H,v 78b0d113b99b 2013-02-25 16:12:41Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef writeIfFieldOutsideFunctionObject_H
#define writeIfFieldOutsideFunctionObject_H

#include "conditionDrivenWritingFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeIfFieldOutsideFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class writeIfFieldOutsideFunctionObject
:
    public conditionDrivenWritingFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeIfFieldOutsideFunctionObject(const writeIfFieldOutsideFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeIfFieldOutsideFunctionObject&);

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
    TypeName("writeIfFieldOutside");


    // Constructors

    //- Construct from components
    writeIfFieldOutsideFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

protected:
    //- should writing start now?
    virtual bool checkStartWriting();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "writeIfFieldOutsideFunctionObjectTemplates.C"
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
