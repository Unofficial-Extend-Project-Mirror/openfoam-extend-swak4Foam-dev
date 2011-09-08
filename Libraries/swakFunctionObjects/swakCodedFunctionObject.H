/*-----------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::swakCodedFunctionObject

Description
    functionObject using dynamic code compilation. Reads and writes swak 
    global variables

SourceFiles
    swakCodedFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef swakCodedFunctionObject_H
#define swakCodedFunctionObject_H

#include "codedFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


/*---------------------------------------------------------------------------*\
                       Class swakCodedFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class swakCodedFunctionObject
:
    public codedFunctionObject
{
        //- Disallow default bitwise copy construct
        swakCodedFunctionObject(const swakCodedFunctionObject&);

        //- Disallow default bitwise assignment
        void operator=(const swakCodedFunctionObject&);

    //- variables in those namespaces should be inserted into the 
    //- Coded namespace before running the snipplet
    wordList swakToCodedNamespaces_;

    //- Namespace to write values to

    //- this global swak-namespace will get all values (if the user request such)
    word codedToSwakNamespace_;

    //- Coded variables that swak will receive

    //- the values of these variables will get written to the global swak namespace
    //- only variables that can be either interpreted as a scalar or a vector are
    //- supported
    wordList codedToSwakVariables_;

    //- should the code have Debug-Information
    bool verboseCode_;

protected:

    //- insert swak-specific code
    void injectSwakCode(const word &key,const string &pre,const string &post);

public:

    //- Runtime type information
    TypeName("swakCoded");


    // Constructors

        //- Construct for given objectRegistry and dictionary.
        //  Allow the possibility to load fields from files
        swakCodedFunctionObject
        (
            const word& name,
            const Time& time,
            const dictionary& dict
        );


    //- Destructor
    virtual ~swakCodedFunctionObject();


    // Member Functions

        //- Read and set the function object if its data have changed
        virtual bool read(const dictionary&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
