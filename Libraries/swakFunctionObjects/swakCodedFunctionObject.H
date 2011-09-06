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
protected:

//         //- Load specified library and execute globalFuncName(true)
//         void* loadLibrary
//         (
//             const fileName& libPath,
//             const string& globalFuncName,
//             const dictionary& contextDict
//         ) const;

//         //- Execute globalFuncName(false) and unload specified library
//         void unloadLibrary
//         (
//             const fileName& libPath,
//             const string& globalFuncName,
//             const dictionary& contextDict
//         ) const;


//         //- Create library based on the dynamicCodeContext
//         void createLibrary(dynamicCode&, const dynamicCodeContext&) const;

//         //- Update library as required
//         void updateLibrary() const;

//         //- Read relevant dictionary entries
//         void readDict();

        //- Disallow default bitwise copy construct
        swakCodedFunctionObject(const swakCodedFunctionObject&);

        //- Disallow default bitwise assignment
        void operator=(const swakCodedFunctionObject&);


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

//         //- Dynamically compiled functionObject
//         functionObject& redirectFunctionObject() const;

//         //- Called at the start of the time-loop
//         virtual bool start();

//         //- Called at each ++ or += of the time-loop. forceWrite overrides the
//         //  outputControl behaviour.
//         virtual bool execute(const bool forceWrite);

//         //- Called when Time::run() determines that the time-loop exits.
//         //  By default it simply calls execute().
//         virtual bool end();

        //- Read and set the function object if its data have changed
        virtual bool read(const dictionary&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
