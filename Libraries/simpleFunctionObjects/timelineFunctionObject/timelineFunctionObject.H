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
    timelineFunctionObject

Description
    Basis for function objects that generate timelines

SourceFiles
    timelineFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef timelineFunctionObject_H
#define timelineFunctionObject_H

#include "simpleDataFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class timelineFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class timelineFunctionObject
:
    public simpleDataFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    timelineFunctionObject(const timelineFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const timelineFunctionObject&);

    //- Factor to multiply results with (to account for symmetries etc)
    scalar factor_;

protected:

    //- Current open files
    HashPtrTable<OFstream> filePtrs_;

    //- Name of the sub-dictionary of the case
    virtual word dirName()=0;

    //- Names of the files
    virtual wordList fileNames()=0;

    //- Text in the first line
    virtual string firstLine()=0;

    //- Factor to multiply results with (for symmetries etc)
    scalar factor() const { return factor_; }

public:

    //- Runtime type information
    TypeName("timelineFunctionObject");


    // Constructors

    //- Construct from components
    timelineFunctionObject
    (
        const word &name,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    virtual void flush();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
