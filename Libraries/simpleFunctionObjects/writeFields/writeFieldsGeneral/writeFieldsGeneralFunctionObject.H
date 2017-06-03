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
    writeFieldsGeneralFunctionObject

Description
    General Class for writting Fields

SourceFiles
    writeFieldsGeneralFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2014 David Huckaby <e.david.huckaby@netl.doe.gov>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef writeFieldsGeneralFunctionObject_H
#define writeFieldsGeneralFunctionObject_H

#include "simpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeFieldsGeneralFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class writeFieldsGeneralFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeFieldsGeneralFunctionObject(const writeFieldsGeneralFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeFieldsGeneralFunctionObject&);

    //- Name of the fields that are to be written
    wordList fieldNames_;

    //- Name of the fields that are to be written
    wordList cloudNames_;

    //- wrapper for the writing
    template <class T>
    bool writeField(const word &name) const;

    //- wrapper for writing clouds
    template <class T>
    bool writeCloud(const word &name) const;

protected:
    // //- Overwrite the default output time
    // virtual bool outputTime();
public:

    // Constructors

    //- Construct from components
    writeFieldsGeneralFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();

    const wordList &fieldNames() const { return fieldNames_; }

    const wordList &cloudNames() const { return cloudNames_; }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "writeFieldsGeneralFunctionObjectTemplates.C"
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
